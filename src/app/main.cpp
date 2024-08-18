//
// Created by fluty on 2023/8/27.
//

#include "Controller/AppController.h"


#include <QApplication>
#include <QStyleFactory>
#include <QScreen>
#include <QTranslator>

#include "UI/Window/MainWindow.h"
#include "Modules/Audio/AudioSystem.h"
#include "Utils/Logger.h"
#include "Model/AppModel/AppModel.h"
#include "Model/AppOptions/AppOptions.h"
#include "UI/Utils/ThemeManager.h"
#include "UI/Window/TaskWindow.h"
#include "Controller/TracksViewController.h"

#include <Modules/Audio/subsystem/OutputSystem.h>
#include <Modules/Audio/subsystem/MidiSystem.h>
#include <Modules/Audio/utils/DeviceTester.h>
#include <Modules/Audio/AudioContext.h>

int main(int argc, char *argv[]) {
    QElapsedTimer mstimer;
    mstimer.start();
    // output log to file
    qInstallMessageHandler(Logger::handler);
    qputenv("QT_ASSUME_STDERR_HAS_CONSOLE", "1");
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "1");
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    if (QSysInfo::productType() == "windows")
        QGuiApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    QApplication a(argc, argv);
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents);
    QApplication::setEffectEnabled(Qt::UI_AnimateTooltip, false);
    QApplication::setOrganizationName("OpenVPI");
    QApplication::setApplicationName("DS Editor Lite");
    QApplication::setEffectEnabled(Qt::UI_AnimateCombo, false);
    if (QSysInfo::productType() == "macos")
        QApplication::setStyle(QStyleFactory::create("windows"));

    Logger::logSystemInfo();

    auto f = QFont();
    f.setHintingPreference(QFont::PreferNoHinting);
    auto factor = (QSysInfo::productType() == "windows") ? 1.0 : 96.0 / 72;
    f.setPointSizeF(10 * factor);
    QApplication::setFont(f);

    auto translator = new QTranslator;
    if (translator->load(":translate/translation_zh_CN.qm"))
        QApplication::installTranslator(translator);

    AudioSystem as;
    AudioSystem::outputSystem()->initialize();
    AudioSystem::midiSystem()->initialize();

    new DeviceTester(&as);
    new AudioContext(&as);

    // 需要存储自定义的信息时，根据唯一名称获取到 editor 对象
    // auto editor = appModel->workspaceEditor("flutydeer.filllyrics");
    // auto workspace = editor->privateWorkspace();
    // workspace->insert("version_code", 1);
    // workspace->insert("isFirstRun", false);
    // workspace->insert("recent_model_path", "D:/Model/test.onnx");
    // editor->commit();

    // auto globalWorkspace = appModel->globalWorkspace();
    // for (const auto &key : globalWorkspace.keys())
    //     qDebug() << "AppModel workspace contains key:" << key;

    // 可根据唯一名称取出存在全局 workspace 中的数据（只读）
    // auto privateWorkspace = appModel->getPrivateWorkspaceById("flutydeer.testplugin");
    // qDebug() << privateWorkspace.value("recent_model_path").toString();

    QObject::connect(appOptions, &AppOptions::optionsChanged, ThemeManager::instance(),
                     &ThemeManager::onAppOptionsChanged);

    MainWindow w;
    trackController->setParentWidget(&w);
    auto scr = QApplication::screenAt(QCursor::pos());
    auto availableRect = scr->availableGeometry();
    auto left = (availableRect.width() - w.width()) / 2;
    auto top = (availableRect.height() - w.height()) / 2;
    w.move(left, top);
    w.show();

    auto taskWindow = new TaskWindow;
    taskWindow->move(availableRect.width() - taskWindow->width() - 8,
                     availableRect.height() - taskWindow->height() - 8);
    taskWindow->show();

    auto args = QApplication::arguments();
    if (args.count() > 1) {
        auto filePath = QApplication::arguments().at(1);
        qDebug() << filePath;
        if (!filePath.isEmpty()) {
            appController->openProject(filePath);
            auto tracks = appModel->tracks();
            if (!tracks.isEmpty()) {
                auto clips = tracks.first()->clips();
                if (clips.count() > 0) {
                    trackController->setActiveClip(clips.toList().first()->id());
                    appController->onPanelClicked(AppGlobal::ClipEditor);
                }
            }
        }
    }

    const auto time = static_cast<double>(mstimer.nsecsElapsed()) / 1000000.0;
    qDebug() << "App launched in" << time << "ms";

    return QApplication::exec();
}