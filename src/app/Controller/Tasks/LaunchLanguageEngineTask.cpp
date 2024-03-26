//
// Created by fluty on 24-3-19.
//

#include "LaunchLanguageEngineTask.h"

#include <QApplication>

#include <G2pMgr/IG2pManager.h>
#include <LangMgr/ILanguageManager.h>
#include "Modules/Language/LangSetting/ILangSetManager.h"

#include <QThread>

LaunchLanguageEngineTask::LaunchLanguageEngineTask(QObject *parent) : ITask(parent) {
    TaskStatus status;
    status.title = "Launching language engine...";
    status.message = "";
    status.isIndetermine = true;
    setStatus(status);
}
void LaunchLanguageEngineTask::runTask() {
    qDebug() << "RunLanguageEngineTask::runTask";
    const auto g2pMgr = G2pMgr::IG2pManager::instance();
    const auto langMgr = LangMgr::ILanguageManager::instance();
    const auto langSet = LangSetting::ILangSetManager::instance();

    QString errorMsg;
    g2pMgr->initialize(errorMsg);

    qDebug() << "G2pMgr: errorMsg" << errorMsg << "initialized:" << g2pMgr->initialized();

    langMgr->initialize(errorMsg);
    qDebug() << "LangMgr: errorMsg" << errorMsg << "initialized:" << langMgr->initialized();

    success = g2pMgr->initialized() && langMgr->initialized();
    errorMessage = errorMsg;

    // QThread::sleep(5);

    emit finished(false);
}