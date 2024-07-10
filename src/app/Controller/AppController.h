//
// Created by FlutyDeer on 2023/12/1.
//

#ifndef TRACKSCONTROLLER_H
#define TRACKSCONTROLLER_H

#include <QObject>

#include "Utils/Singleton.h"
#include "Global/AppGlobal.h"


class IPanel;
class LaunchLanguageEngineTask;
class AppModel;
class DecodeAudioTask;
class AudioClip;

class AppController final : public QObject, public Singleton<AppController> {
    Q_OBJECT

public:
    explicit AppController();
    ~AppController() override = default;

    [[nodiscard]] QString lastProjectPath() const;

    [[nodiscard]] bool isLanguageEngineReady() const;
    void registerPanel(IPanel *panel);

public slots:
    void onNewProject();
    void openProject(const QString &filePath);
    void saveProject(const QString &filePath);

    void importMidiFile(const QString &filePath);
    void exportMidiFile(const QString &filePath);

    void importAproject(const QString &filePath);

    void onSetTempo(double tempo);
    void onSetTimeSignature(int numerator, int denominator);
    void onSetQuantize(int quantize);
    void onTrackSelectionChanged(int trackIndex);
    void onPanelClicked(AppGlobal::PanelType panel);

signals:
    void activatedPanelChanged(AppGlobal::PanelType panel);

private:
    bool isPowerOf2(int num);
    void decodeAllAudioClips(AppModel &model);
    void createAndStartDecodeAudioTask(AudioClip *clip);
    static void handleDecodeAudioTaskFinished(DecodeAudioTask *task, bool terminate);
    void handleRunLanguageEngineTaskFinished(LaunchLanguageEngineTask *task);

    QString m_lastProjectPath;
    bool m_isLanguageEngineReady = false;
    QList<IPanel *> m_panels;
    AppGlobal::PanelType m_activatedPanel = AppGlobal::TracksEditor;
};

// using ControllerSingleton = Singleton<Controller>;

#endif // TRACKSCONTROLLER_H
