//
// Created by Crs_1 on 2024/2/6.
//

#ifndef DS_EDITOR_LITE_AUDIOEXPORTER_H
#define DS_EDITOR_LITE_AUDIOEXPORTER_H

#include <QObject>

class AudioExporter : public QObject {
    Q_OBJECT
public:
    explicit AudioExporter(QObject *parent = nullptr);
    ~AudioExporter() override;

    struct Option {
        QString fileDirectory;
        QString fileName;

        int formatFlag;
        int vbrQuality;
        double sampleRate;
        QString extensionName;

        enum SourceOption {
            AllTracks,
            SelectedTracks,
            CustomTracks,
        };

        SourceOption sourceOption;
        QList<int> selectedTrackIndices;

        enum MixingOption {
            Mixed,
            Seperated,
            SeparatedThroughMasterTrack,
        };

        MixingOption mixingOption;
        QString affix;
        bool enableMuteSolo;

        enum TimeRangeOption {
            All,
            LoopInterval,
            CustomRange,
        };

        TimeRangeOption timeRangeOption;
        qint64 rangeStartTick;
        qint64 rangeEndTick;
    };

    static QList<QPair<QString, Option>> builtInPresets();

    void savePreset(const QString &name) const;
    bool loadPreset(const QString &name);
    static bool deletePreset(const QString &name);
    static QVariant lastUsedPreset();
    static QStringList presets();

    void setOption(const Option &option);
    Option option() const;

    QStringList outputFileList() const;

    struct Format {
        QString formatName;
        int flag;
        bool isVBRAvailable;
        QString extensionName;
        QList<QPair<QString, int>> options;
        int findOptionIndex(int flag) const;
    };

    static QList<Format> formats();
    static int findFormatIndex(int flag);

    enum Status {
        Success,
        Interrupted,
        Failure,
    };

    Status exec();

public slots:
    void interrupt();

private:
    Option m_option = {};
};



#endif // DS_EDITOR_LITE_AUDIOEXPORTER_H
