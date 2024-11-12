#include <algorithm>
#include <cmath>
#include <queue>
#include <vector>

#include <sndfile.hh>

#include "MathUtils.h"
#include "Slicer.h"

template <class T>
inline uint64_t argmin_range_view(const std::vector<T> &v, uint64_t begin, uint64_t end);

template <class T>
inline std::vector<T> multichannel_to_mono(const std::vector<T> &v, unsigned int channels);

template <class T>
inline std::vector<double> get_rms(const std::vector<T> &arr, uint64_t frame_length = 2048, uint64_t hop_length = 512);

class MovingRMS {
private:
    uint64_t m_windowSize;
    uint64_t m_numElements;
    double m_squareSum;
    std::queue<double> m_queue;

public:
    explicit MovingRMS(uint64_t windowSize);
    void push(double num);
    double rms();
    uint64_t capacity();
    uint64_t size();
};

MovingRMS::MovingRMS(uint64_t windowSize) {
    m_windowSize = windowSize;
    m_numElements = 0;
    m_squareSum = 0.0;
}

void MovingRMS::push(double num) {
    double frontItem = 0.0;
    double numSquared = num * num;
    if (m_numElements < m_windowSize) {
        m_queue.push(numSquared);
        ++m_numElements;
    } else {
        frontItem = m_queue.front();
        m_queue.pop();
        m_queue.push(numSquared);
    }
    m_squareSum += numSquared - frontItem;
}

double MovingRMS::rms() {
    if ((m_windowSize == 0) || (m_squareSum < 0)) {
        return 0.0;
    }
    return std::sqrt(std::max(0.0, (double)m_squareSum / (double)m_windowSize));
}

uint64_t MovingRMS::capacity() { return m_windowSize; }

uint64_t MovingRMS::size() { return m_numElements; }

Slicer::Slicer(SndfileHandle *decoder, double threshold, uint64_t minLength, uint64_t minInterval, uint64_t hopSize,
               uint64_t maxSilKept) {
    m_errCode = SlicerErrorCode::SLICER_OK;
    if ((!((minLength >= minInterval) && (minInterval >= hopSize))) || (maxSilKept < hopSize)) {
        // The following condition must be satisfied: m_minLength >= m_minInterval >= m_hopSize
        // The following condition must be satisfied: m_maxSilKept >= m_hopSize
        m_errCode = SlicerErrorCode::SLICER_INVALID_ARGUMENT;
        m_errMsg = "ValueError: The following conditions must be satisfied: "
                   "(min_length >= min_interval >= hop_size) and (max_sil_kept >= hop_size).";
        return;
    }

    m_decoder = decoder;
    if (!m_decoder) {
        m_errCode = SlicerErrorCode::SLICER_AUDIO_ERROR;
        m_errMsg = "Invalid audio decoder!";
        return;
    }

    m_decoder->seek(0, SEEK_SET);
    int sr = m_decoder->samplerate();
    if (sr <= 0) {
        m_errCode = SlicerErrorCode::SLICER_AUDIO_ERROR;
        m_errMsg = "Invalid audio file!";
        return;
    }

    m_threshold = std::pow(10, threshold / 20.0);
    m_hopSize = divIntRound<uint64_t>(hopSize * (uint64_t)sr, (uint64_t)1000);
    m_winSize = std::min(divIntRound<uint64_t>(minInterval * (uint64_t)sr, (uint64_t)1000), (uint64_t)4 * m_hopSize);
    m_minLength = divIntRound<uint64_t>(minLength * (uint64_t)sr, (uint64_t)1000 * m_hopSize);
    m_minInterval = divIntRound<uint64_t>(minInterval * (uint64_t)sr, (uint64_t)1000 * m_hopSize);
    m_maxSilKept = divIntRound<uint64_t>(maxSilKept * (uint64_t)sr, (uint64_t)1000 * m_hopSize);
}


MarkerList Slicer::slice() {
    if (m_errCode == SlicerErrorCode::SLICER_INVALID_ARGUMENT) {
        return {};
    }

    uint64_t frames = m_decoder->frames();
    int channels = m_decoder->channels();

    if ((frames + m_hopSize - 1) / m_hopSize <= m_minLength) {
        return {{0, frames}};
    }

    uint64_t rms_size = frames / m_hopSize + 1;
    std::vector<double> rms_list(rms_size);
    uint64_t rms_index = 0;

    MovingRMS movingRms(m_winSize);
    uint64_t padding = m_winSize / 2;
    for (uint64_t i = 0; i < padding; i++) {
        movingRms.push(0.0);
    }

    uint64_t samplesRead = 0;
    {
        std::vector<double> initialBuffer(padding * channels);
        samplesRead = m_decoder->read(initialBuffer.data(), padding * channels);
        uint64_t framesRead = samplesRead / channels;
        for (uint64_t i = 0; i < framesRead; i++) {
            double monoSample = 0.0;
            for (int j = 0; j < channels; j++) {
                monoSample += initialBuffer[i * channels + j] / static_cast<double>(channels);
            }
            movingRms.push(monoSample);
        }
    }

    rms_list[rms_index++] = movingRms.rms();

    std::vector<double> buffer(m_hopSize * channels);

    do {
        samplesRead = m_decoder->read(buffer.data(), m_hopSize * channels);
        if (samplesRead == 0) {
            break;
        }
        uint64_t framesRead = samplesRead / channels;
        for (uint64_t i = 0; i < framesRead; i++) {
            double monoSample = 0.0;
            for (int j = 0; j < channels; j++) {
                monoSample += buffer[i * channels + j] / static_cast<double>(channels);
            }
            movingRms.push(monoSample);
        }
        for (uint64_t i = framesRead; i < buffer.size() / channels; i++) {
            movingRms.push(0.0);
        }
        rms_list[rms_index++] = movingRms.rms();
        // qDebug() << samplesRead << "<->" << rms_list[rms_index - 1];
    }
    while (rms_index < rms_list.size());

    while (rms_index < rms_list.size()) {
        for (uint64_t i = 0; i < m_hopSize; i++) {
            movingRms.push(0.0);
        }
        rms_list[rms_index++] = movingRms.rms();
    }

    // uint64_t frames = waveform.size() / channels;
    // std::vector<float> samples = multichannel_to_mono<float>(waveform, channels);


    // std::vector<double> rms_list = get_rms<float>(samples, (uint64_t) m_winSize, (uint64_t) m_hopSize);

    MarkerList sil_tags;
    uint64_t silence_start = 0;
    bool has_silence_start = false;
    uint64_t clip_start = 0;

    uint64_t pos = 0, pos_l = 0, pos_r = 0;

    for (uint64_t i = 0; i < rms_list.size(); i++) {
        double rms = rms_list[i];
        // Keep looping while frame is silent.
        if (rms < m_threshold) {
            // Record start of silent frames.
            if (!has_silence_start) {
                silence_start = i;
                has_silence_start = true;
            }
            continue;
        }
        // Keep looping while frame is not silent and silence start has not been recorded.
        if (!has_silence_start) {
            continue;
        }
        // Clear recorded silence start if interval is not enough or clip is too short
        bool is_leading_silence = ((silence_start == 0) && (i > m_maxSilKept));
        bool need_slice_middle = (((i - silence_start) >= m_minInterval) && ((i - clip_start) >= m_minLength));
        if ((!is_leading_silence) && (!need_slice_middle)) {
            has_silence_start = false;
            continue;
        }

        // Need slicing. Record the range of silent frames to be removed.
        if ((i - silence_start) <= m_maxSilKept) {
            pos = argmin_range_view<double>(rms_list, silence_start, i + 1) + silence_start;
            if (silence_start == 0) {
                sil_tags.emplace_back(0, pos);
            } else {
                sil_tags.emplace_back(pos, pos);
            }
            clip_start = pos;
        } else if ((i - silence_start) <= (m_maxSilKept * 2)) {
            pos = argmin_range_view<double>(rms_list, i - m_maxSilKept, silence_start + m_maxSilKept + 1);
            pos += i - m_maxSilKept;
            pos_l =
                argmin_range_view<double>(rms_list, silence_start, silence_start + m_maxSilKept + 1) + silence_start;
            pos_r = argmin_range_view<double>(rms_list, i - m_maxSilKept, i + 1) + i - m_maxSilKept;
            if (silence_start == 0) {
                clip_start = pos_r;
                sil_tags.emplace_back(0, clip_start);
            } else {
                clip_start = std::max(pos_r, pos);
                sil_tags.emplace_back(std::min(pos_l, pos), clip_start);
            }
        } else {
            pos_l =
                argmin_range_view<double>(rms_list, silence_start, silence_start + m_maxSilKept + 1) + silence_start;
            pos_r = argmin_range_view<double>(rms_list, i - m_maxSilKept, i + 1) + i - m_maxSilKept;
            if (silence_start == 0) {
                sil_tags.emplace_back(0, pos_r);
            } else {
                sil_tags.emplace_back(pos_l, pos_r);
            }
            clip_start = pos_r;
        }
        has_silence_start = false;
    }
    // Deal with trailing silence.
    uint64_t total_frames = rms_list.size();
    if (has_silence_start && ((total_frames - silence_start) >= m_minInterval)) {
        uint64_t silence_end = std::min(total_frames - 1, silence_start + m_maxSilKept);
        pos = argmin_range_view<double>(rms_list, silence_start, silence_end + 1) + silence_start;
        sil_tags.emplace_back(pos, total_frames + 1);
    }
    // Apply and return slices.
    if (sil_tags.empty()) {
        return {{0, frames}};
    } else {
        MarkerList chunks;
        uint64_t begin = 0, end = 0;
        uint64_t s0 = sil_tags[0].first;
        if (s0 > 0) {
            begin = 0;
            end = s0;
            chunks.emplace_back(begin * m_hopSize, std::min(frames, end * m_hopSize));
        }
        for (auto i = 0; i < sil_tags.size() - 1; i++) {
            begin = sil_tags[i].second;
            end = sil_tags[i + 1].first;
            chunks.emplace_back(begin * m_hopSize, std::min(frames, end * m_hopSize));
        }
        if (sil_tags.back().second < total_frames) {
            begin = sil_tags.back().second;
            end = total_frames;
            chunks.emplace_back(begin * m_hopSize, std::min(frames, end * m_hopSize));
        }
        return chunks;
    }
}

SlicerErrorCode Slicer::getErrorCode() { return m_errCode; }

std::string Slicer::getErrorMsg() { return m_errMsg; }

template <class T>
inline std::vector<double> get_rms(const std::vector<T> &arr, uint64_t frame_length, uint64_t hop_length) {
    uint64_t arr_length = arr.size();

    uint64_t padding = frame_length / 2;

    uint64_t rms_size = arr_length / hop_length + 1;

    std::vector<double> rms = std::vector<double>(rms_size);

    uint64_t left = 0;
    uint64_t right = 0;
    uint64_t hop_count = 0;

    uint64_t rms_index = 0;
    double val = 0;

    // Initial condition: the frame is at the beginning of padded array
    while ((right < padding) && (right < arr_length)) {
        val += (double)arr[right] * arr[right];
        right++;
    }
    rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / (double)frame_length)));

    // Left side or right side of the frame has not touched the sides of original array
    while ((right < frame_length) && (right < arr_length) && (rms_index < rms_size)) {
        val += (double)arr[right] * arr[right];
        hop_count++;
        if (hop_count == hop_length) {
            rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / (double)frame_length)));
            hop_count = 0;
        }
        right++; // Move right 1 step at a time.
    }

    if (frame_length < arr_length) {
        while ((right < arr_length) && (rms_index < rms_size)) {
            val += (double)arr[right] * arr[right] - (double)arr[left] * arr[left];
            hop_count++;
            if (hop_count == hop_length) {
                rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / (double)frame_length)));
                hop_count = 0;
            }
            left++;
            right++;
        }
    } else {
        while ((right < frame_length) && (rms_index < rms_size)) {
            hop_count++;
            if (hop_count == hop_length) {
                rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / (double)frame_length)));
                hop_count = 0;
            }
            right++;
        }
    }

    while ((left < arr_length) && (rms_index < rms_size) /* && (right < arr_length + padding)*/) {
        val -= (double)arr[left] * arr[left];
        hop_count++;
        if (hop_count == hop_length) {
            rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / (double)frame_length)));
            hop_count = 0;
        }
        left++;
        right++;
    }

    return rms;
}

template <class T>
inline uint64_t argmin_range_view(const std::vector<T> &v, uint64_t begin, uint64_t end) {
    // Ensure vector access is not out of bound
    auto size = v.size();
    if (begin > size)
        begin = size;
    if (end > size)
        end = size;
    if (begin >= end)
        return 0;

    auto min_it = std::min_element(v.begin() + begin, v.begin() + end);
    return std::distance(v.begin() + begin, min_it);
}

template <class T>
inline std::vector<T> multichannel_to_mono(const std::vector<T> &v, unsigned int channels) {
    uint64_t frames = v.size() / channels;
    std::vector<T> out(frames);

    for (uint64_t i = 0; i < frames; i++) {
        T s = 0;
        for (unsigned int j = 0; j < channels; j++) {
            s += (T)v[i * channels + j] / (T)channels;
        }
        out[i] = s;
    }

    return out;
}
