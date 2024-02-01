#include "AudioSystem.h"

#include <QTimer>
#include <QMessageBox>
#include <QPushButton>

#include <TalcsCore/TransportAudioSource.h>
#include <TalcsCore/PositionableMixerAudioSource.h>
#include <TalcsCore/MixerAudioSource.h>
#include <TalcsCore/SineWaveAudioSource.h>
#include <TalcsDevice/AudioDriverManager.h>
#include <TalcsDevice/AudioDriver.h>
#include <TalcsDevice/AudioDevice.h>
#include <TalcsDevice/AudioSourcePlayback.h>

#include "Audio/Dialogs/AudioSettingsDialog.h"

static AudioSystem *m_instance = nullptr;

AudioSystem::AudioSystem(QObject *parent) : QObject(parent), m_settings("OpenVPI", "DsEditorLite") {
    m_drvMgr = talcs::AudioDriverManager::createBuiltInDriverManager(this);
    m_masterTrack = new talcs::PositionableMixerAudioSource;
    m_tpSrc = new talcs::TransportAudioSource(m_masterTrack, true);
    m_preMixer = new talcs::MixerAudioSource;
    m_preMixer->addSource(m_tpSrc, true);
    m_playback.reset(new talcs::AudioSourcePlayback(m_preMixer, true, false));
    m_instance = this;
}

AudioSystem::~AudioSystem() {
    m_dev.reset();
}

AudioSystem *AudioSystem::instance() {
    return m_instance;
}

bool AudioSystem::findProperDriver() {
    for (int i = -1;; i++) {
        if (i >= m_drvMgr->drivers().size()) {
            m_drv = nullptr;
            return false;
        }
        if (i == -1) {
            auto savedDrvName = m_settings.value("audio/driverName").toString();
            if (savedDrvName.isEmpty())
                continue;
            m_drv = m_drvMgr->driver(savedDrvName);
        } else {
            m_drv = m_drvMgr->driver(m_drvMgr->drivers()[i]);
        }
        if (m_drv && m_drv->initialize()) {
            connect(m_drv, &talcs::AudioDriver::deviceChanged, this, &AudioSystem::handleDeviceHotPlug);
            return true;
        }
    }
}

bool AudioSystem::findProperDevice() {
    for (int i = -2;; i++) {
        std::unique_ptr<talcs::AudioDevice> dev;
        if (i >= m_drv->devices().size())
            return false;
        if (i == -2) {
            auto savedDeviceName = m_settings.value("audio/deviceName").toString();
            if (!savedDeviceName.isEmpty())
                dev.reset(m_drv->createDevice(savedDeviceName));
        } else if (i == -1) {
            if (!m_drv->defaultDevice().isEmpty())
                dev.reset(m_drv->createDevice(m_drv->defaultDevice()));
        } else {
            dev.reset(m_drv->createDevice(m_drv->devices()[i]));
        }
        if (!dev || !dev->isInitialized())
            continue;
        if (i == -2) {
            auto savedBufferSize = m_settings.value("audio/adoptedBufferSize", dev->preferredBufferSize()).value<qint64>();
            auto savedSampleRate = m_settings.value("audio/adoptedSampleRate", dev->preferredSampleRate()).value<double>();
            if (!dev->open(savedBufferSize, savedSampleRate))
                if (!dev->open(dev->preferredBufferSize(), dev->preferredSampleRate()))
                    continue;
        } else {
            if (!dev->open(dev->preferredBufferSize(), dev->preferredSampleRate()))
                continue;
        }

        m_dev = std::move(dev);
        postSetDevice();
        return true;
    }
}

bool AudioSystem::initialize(bool isVstMode) {
    if (isVstMode) {
        return false;
    } else {
        if (!findProperDriver())
            return false;
        if (!findProperDevice())
            return false;
        return true;
    }
}

talcs::AudioDriverManager *AudioSystem::driverManager() const {
    return m_drvMgr;
}

talcs::AudioDevice *AudioSystem::device() const {
    return m_dev.get();
}

talcs::AudioDriver *AudioSystem::driver() const {
    return m_drv;
}

talcs::TransportAudioSource *AudioSystem::transport() const {
    return m_tpSrc;
}

bool AudioSystem::isDeviceAutoClosed() const {
    return m_isDeviceAutoClosed;
}

bool AudioSystem::checkStatus() const {
    return m_drv && m_dev && m_dev->isOpen();
}

bool AudioSystem::setDriver(const QString &driverName) {
    if (driverName == m_drv->name())
        return true;
    m_dev.reset();
    if (m_drv) {
        m_drv->finalize();
        disconnect(m_drv, nullptr, this, nullptr);
    }
    m_drv = m_drvMgr->driver(driverName);
    if (!m_drv->initialize())
        return false;
    connect(m_drv, &talcs::AudioDriver::deviceChanged, this, &AudioSystem::handleDeviceHotPlug);
    return findProperDevice();
}

bool AudioSystem::setDevice(const QString &deviceName) {
    if (!m_drv)
        return false;
    m_dev.reset(m_drv->createDevice(deviceName));
    if (!m_dev || !m_dev->isInitialized())
        return false;
    if (!m_dev->open(m_dev->preferredBufferSize(), m_dev->preferredSampleRate()))
        return false;
    postSetDevice();
    connect(m_dev.get(), &talcs::AudioDevice::closed, this, [=] {
        m_isDeviceAutoClosed = true;
    });
    return true;
}
void AudioSystem::postSetDevice() {
    m_adoptedBufferSize = m_dev->bufferSize();
    m_adoptedSampleRate = m_dev->sampleRate();
    m_isDeviceAutoClosed = false;
    m_preMixer->open(m_adoptedBufferSize, m_adoptedSampleRate);
    m_settings.setValue("audio/driverName", m_drv->name());
    m_settings.setValue("audio/deviceName", m_dev->name());
    m_settings.setValue("audio/adoptedBufferSize", m_adoptedBufferSize);
    m_settings.setValue("audio/adoptedSampleRate", m_adoptedSampleRate);
}

qint64 AudioSystem::adoptedBufferSize() const {
    return m_adoptedBufferSize;
}
void AudioSystem::setAdoptedBufferSize(qint64 bufferSize) {
    if (device()->isOpen()) {
        device()->open(bufferSize, device()->sampleRate());
    }
    m_adoptedBufferSize = bufferSize;
    m_settings.setValue("audio/adoptedBufferSize", m_adoptedBufferSize);
    if (m_adoptedBufferSize && m_adoptedSampleRate)
        m_preMixer->open(m_adoptedBufferSize, m_adoptedSampleRate);
}
double AudioSystem::adoptedSampleRate() const {
    return m_adoptedSampleRate;
}
void AudioSystem::setAdoptedSampleRate(double sampleRate) {
    if (device()->isOpen()) {
        device()->open(device()->bufferSize(), sampleRate);
    }
    m_adoptedSampleRate = sampleRate;
    m_settings.setValue("audio/adoptedSampleRate", m_adoptedSampleRate);
    if (m_adoptedBufferSize && m_adoptedSampleRate)
        m_preMixer->open(m_adoptedBufferSize, m_adoptedSampleRate);
}

void AudioSystem::openAudioSettings() {
    m_tpSrc->pause();
    if (m_dev)
        m_dev->stop();
    AudioSettingsDialog dlg;
    dlg.setHotPlugMode(m_settings.value("audio/hotPlugMode", NotifyOnAnyChange).value<HotPlugMode>());
    dlg.setCloseDeviceAtBackground(m_settings.value("audio/closeDeviceAtBackground", false).toBool());
    dlg.setCloseDeviceOnPlaybackStop(m_settings.value("audio/closeDeviceOnPlaybackStop", false).toBool());
    dlg.setFileBufferingSizeMsec(m_settings.value("audio/fileBufferingSizeMsec", 1000.0).toDouble());
    if (dlg.exec() == QDialog::Accepted) {
        m_settings.setValue("audio/hotPlugMode", dlg.hotPlugMode());
        m_settings.setValue("audio/closeDeviceAtBackground", dlg.closeDeviceAtBackground());
        m_settings.setValue("audio/closeDeviceOnPlaybackStop", dlg.closeDeviceOnPlaybackStop());
        m_settings.setValue("audio/fileBufferingSizeMsec", dlg.fileBufferingSizeMsec());
    }
}

void AudioSystem::testDevice() {
    if (!m_dev)
        return;
    auto testMixer = new talcs::MixerAudioSource;
    testMixer->addSource(new talcs::SineWaveAudioSource(440), true);
    testMixer->setGain(0.5);
    m_preMixer->addSource(testMixer);
    if (!m_dev->isStarted())
        m_dev->start(m_playback.get());
    QTimer::singleShot(1000, [=] {
        m_preMixer->removeSource(testMixer);
        delete testMixer;
    });
}

void AudioSystem::handleDeviceHotPlug() {
    auto hotPlugMode = m_settings.value("audio/hotPlugMode", NotifyOnAnyChange).value<HotPlugMode>();
    QMessageBox msgBox;
    msgBox.setText(tr("Audio device change is detected."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.addButton(QMessageBox::Ok);
    auto openAudioSettingsButton = new QPushButton(tr("Go to audio settings"));
    msgBox.addButton(openAudioSettingsButton, QMessageBox::NoRole);
    connect(openAudioSettingsButton, &QPushButton::clicked, this, &AudioSystem::openAudioSettings);
    switch(hotPlugMode) {
        case NotifyOnAnyChange:
            msgBox.exec();
            break;
        case NotifyOnCurrentRemoval:
            if (m_dev && !m_drv->devices().contains(m_dev->name()))
                msgBox.exec();
            break;
        case None:
            break;
    }
}

QString AudioSystem::driverDisplayName(const QString &driverName) {
    return driverName;
}
