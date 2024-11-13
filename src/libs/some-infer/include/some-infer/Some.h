#ifndef RMVPE_H
#define RMVPE_H

#include <filesystem>

#include <audio-util/SndfileVio.h>

#include <some-infer/SomeGlobal.h>
#include <some-infer/SomeModel.h>

namespace Some
{
    class Some {
    public:
        explicit SOME_INFER_EXPORT Some(const std::filesystem::path &modelPath, int device_id);
        SOME_INFER_EXPORT ~Some();

        bool SOME_INFER_EXPORT get_midi(const std::filesystem::path &filepath, std::vector<float> &note_midi,
                                      std::vector<bool> &note_rest, std::vector<float> &note_dur,
                                      std::string &msg) const;

    private:
        bool get_midi(AudioUtil::SF_VIO sf_vio, std::vector<float> &note_midi, std::vector<bool> &note_rest,
                    std::vector<float> &note_dur, std::string &msg) const;

        std::unique_ptr<SomeModel> m_some;
    };
} // namespace Some

#endif // RMVPE_H
