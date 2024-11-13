#include <rmvpe-infer/Rmvpe.h>

#include <CDSPResampler.h>
#include <sndfile.hh>

#include <iostream>

#include <audio-util/Resample.h>
#include <audio-util/Slicer.h>

namespace Rmvpe
{
    Rmvpe::Rmvpe(const std::filesystem::path &modelPath, int device_id) {
        m_rmvpe = std::make_unique<RmvpeModel>(modelPath, device_id);

        if (!m_rmvpe) {
            std::cout << "Cannot load ASR Model, there must be files model.onnx and vocab.txt" << std::endl;
        }
    }

    Rmvpe::~Rmvpe() = default;

    bool Rmvpe::get_f0(AudioUtil::SF_VIO sf_vio, float threshold, std::vector<float> &f0, std::vector<bool> &uv,
                       std::string &msg) const {
        if (!m_rmvpe) {
            return false;
        }
        SndfileHandle sf(sf_vio.vio, &sf_vio.data, SFM_READ, SF_FORMAT_WAV | SF_FORMAT_PCM_16, 1, 16000);
        AudioUtil::Slicer slicer(&sf, -40, 5000, 300, 10, 1000);

        const auto chunks = slicer.slice();

        if (chunks.empty()) {
            msg = "slicer: no audio chunks for output!";
            return false;
        }

        const auto frames = sf.frames();
        const auto totalSize = frames;

        for (const auto &chunk : chunks) {
            const auto beginFrame = chunk.first;
            const auto endFrame = chunk.second;
            const auto frameCount = endFrame - beginFrame;
            if (frameCount <= 0 || beginFrame > totalSize || endFrame > totalSize) {
                continue;
            }

            AudioUtil::SF_VIO sfChunk;
            auto wf = SndfileHandle(sfChunk.vio, &sfChunk.data, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16, 1, 16000);
            sf.seek(static_cast<sf_count_t>(beginFrame), SEEK_SET);
            std::vector<float> tmp(frameCount);
            sf.read(tmp.data(), static_cast<sf_count_t>(tmp.size()));
            const auto bytesWritten = wf.write(tmp.data(), static_cast<sf_count_t>(tmp.size()));

            // if (bytesWritten > 60 * 16000) {
            //     msg = "The audio contains continuous pronunciation segments that exceed 60 seconds. Please manually "
            //           "segment and rerun the recognition program.";
            //     return false;
            // }

            std::vector<float> temp_f0;
            std::vector<bool> temp_uv;
            const bool success = m_rmvpe->forward(tmp, threshold, temp_f0, temp_uv, msg);
            if (!success)
                return false;
            f0.insert(f0.end(), temp_f0.begin(), temp_f0.end());
            uv.insert(uv.end(), temp_uv.begin(), temp_uv.end());
        }
        return true;
    }

    bool Rmvpe::get_f0(const std::filesystem::path &filepath, float threshold, std::vector<float> &f0,
                       std::vector<bool> &uv, std::string &msg) const {
        return get_f0(AudioUtil::resample(filepath, 1, 16000), threshold, f0, uv, msg);
    }
} // namespace Rmvpe
