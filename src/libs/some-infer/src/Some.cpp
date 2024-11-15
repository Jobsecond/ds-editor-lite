#include <some-infer/Some.h>

#include <sndfile.hh>

#include <iostream>

#include <audio-util/Resample.h>
#include <audio-util/Slicer.h>
#include <some-infer/SomeModel.h>

namespace Some
{
    Some::Some(const std::filesystem::path &modelPath, ExecutionProvider provider, int device_id) {
        m_some = std::make_unique<SomeModel>(modelPath, provider, device_id);

        if (!is_open()) {
            std::cout << "Cannot load SOME Model, there must be files " + modelPath.string() << std::endl;
        }
    }

    Some::~Some() = default;

    bool Some::is_open() const { return m_some && m_some->is_open(); }

    std::vector<double> cumulativeSum(const std::vector<float> &durations) {
        std::vector<double> cumsum(durations.size());
        cumsum[0] = static_cast<double>(durations[0]);
        for (size_t i = 1; i < durations.size(); ++i) {
            cumsum[i] = durations[i] + cumsum[i - 1];
        }
        return cumsum;
    }

    std::vector<int> calculateNoteTicks(const std::vector<float> &note_durations, const float tempo) {
        const std::vector<double> cumsum = cumulativeSum(note_durations);

        std::vector<int64_t> scaled_ticks(cumsum.size());
        for (size_t i = 0; i < cumsum.size(); ++i) {
            scaled_ticks[i] = static_cast<int64_t>(std::round(cumsum[i] * tempo * 8));
        }

        std::vector<int> note_ticks(scaled_ticks.size());
        note_ticks[0] = static_cast<int>(scaled_ticks[0]);
        for (size_t i = 1; i < scaled_ticks.size(); ++i) {
            note_ticks[i] = static_cast<int>(scaled_ticks[i] - scaled_ticks[i - 1]);
        }

        return note_ticks;
    }

    static std::vector<Midi> build_midi_note(const double &offset_s, const std::vector<float> &note_midi,
                                             const std::vector<float> &note_dur, const std::vector<bool> &note_rest,
                                             const float tempo) {
        std::vector<Midi> midi_data;
        int start_tick = static_cast<int>(offset_s * tempo * 8);
        const std::vector<int> note_ticks = calculateNoteTicks(note_dur, tempo);

        for (size_t i = 0; i < note_midi.size(); ++i) {
            if (note_rest[i]) {
                start_tick += note_ticks[i];
                continue;
            }
            midi_data.push_back(Midi{static_cast<int>(std::round(note_midi[i])), start_tick, note_ticks[i]});
            start_tick += note_ticks[i];
        }

        return midi_data;
    }

    static uint64_t calculateSumOfDifferences(const AudioUtil::MarkerList &markers) {
        uint64_t sum = 0;
        for (const auto &[fst, snd] : markers) {
            sum += (snd - fst);
        }
        return sum;
    }

    bool Some::get_midi(AudioUtil::SF_VIO sf_vio, std::vector<Midi> &midis, float tempo, std::string &msg,
                        void (*progressChanged)(int)) const {
        if (!m_some) {
            return false;
        }

        SndfileHandle sf(sf_vio.vio, &sf_vio.data, SFM_READ, SF_FORMAT_WAV | SF_FORMAT_PCM_16, 1, 44100);
        AudioUtil::Slicer slicer(&sf, -40, 5000, 300, 10, 1000);

        const auto chunks = slicer.slice();

        if (chunks.empty()) {
            msg = "slicer: no audio chunks for output!";
            return false;
        }

        const auto frames = sf.frames();
        const auto totalSize = frames;

        int processedFrames = 0; // To track processed frames
        const auto slicerFrames = calculateSumOfDifferences(chunks);

        for (const auto &[fst, snd] : chunks) {
            const auto beginFrame = fst;
            const auto endFrame = snd;
            const auto frameCount = endFrame - beginFrame;
            if (frameCount <= 0 || beginFrame > totalSize || endFrame > totalSize) {
                continue;
            }

            AudioUtil::SF_VIO sfChunk;
            auto wf = SndfileHandle(sfChunk.vio, &sfChunk.data, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16, 1, 16000);
            sf.seek(static_cast<sf_count_t>(beginFrame), SEEK_SET);
            std::vector<float> tmp(frameCount);
            sf.read(tmp.data(), static_cast<sf_count_t>(tmp.size()));
            wf.write(tmp.data(), static_cast<sf_count_t>(tmp.size()));

            std::vector<float> temp_midi;
            std::vector<bool> temp_rest;
            std::vector<float> temp_dur;

            const bool success = m_some->forward(tmp, temp_midi, temp_rest, temp_dur, msg);
            if (!success)
                return false;

            std::vector<Midi> temp_midis =
                build_midi_note(static_cast<double>(fst) / 44100.0, temp_midi, temp_dur, temp_rest, tempo);
            midis.insert(midis.end(), temp_midis.begin(), temp_midis.end());

            // Update the processed frames and calculate progress
            processedFrames += static_cast<int>(frameCount);
            int progress =
                static_cast<int>((static_cast<float>(processedFrames) / static_cast<float>(slicerFrames)) * 100);

            // Call the progress callback with the updated progress
            if (progressChanged) {
                progressChanged(progress); // Trigger the callback with the progress value
            }
        }
        return true;
    }

    bool Some::get_midi(const std::filesystem::path &filepath, std::vector<Midi> &midis, const float tempo,
                        std::string &msg, void (*progressChanged)(int)) const {
        return get_midi(AudioUtil::resample(filepath, 1, 44100), midis, tempo, msg, progressChanged);
    }
} // namespace Some
