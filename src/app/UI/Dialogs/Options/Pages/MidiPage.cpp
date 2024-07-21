#include "MidiPage.h"

#include <cmath>

#include <QDebug>
#include <QBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QMutex>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

#include <TalcsCore/NoteSynthesizer.h>
#include <TalcsCore/MixerAudioSource.h>
#include <TalcsCore/Decibels.h>
#include <TalcsDevice/AbstractOutputContext.h>
#include <TalcsMidi/MidiInputDevice.h>
#include <TalcsMidi/MidiMessage.h>
#include <TalcsMidi/MidiNoteSynthesizer.h>

#include <Modules/Audio/AudioSystem.h>

#include <Modules/Audio/subsystem/MidiSystem.h>
#include <Modules/Audio/subsystem/OutputSystem.h>
#include <Modules/Audio/subsystem/VSTConnectionSystem.h>

#include <UI/Controls/SeekBar.h>

namespace SVS {
    using SeekBar = ::SeekBar;
    using ExpressionSpinBox = ::QSpinBox;
    using ExpressionDoubleSpinBox = ::QDoubleSpinBox;

    class DecibelLinearizer {
    public:
        inline static double decibelToLinearValue(double decibel, double factor = -24) {
            return std::exp((decibel - factor) / - factor) - std::exp(1);
        }

        inline static double linearValueToDecibel(double linearValue, double factor = -24) {
            return -factor * std::log(linearValue + std::exp(1)) + factor;
        }
    };
}

static talcs::NoteSynthesizerDetectorMessage scores[] = {
    {0, 47, .5, true},
    {0, 75, 1, true},
    {2, 75, false},
    {2, 66, .5, true},
    {4, 66, false},
    {4, 73, .7, true},
    {6, 73, false},
    {6, 71, 1, true},
    {8, 71, false},
    {8, 66, .5, true},
    {10, 66, false},
    {10, 73, .7, true},
    {12, 47, false},
    {12, 73, false},

    {12, 49, .5, true},
    {12, 75, 1, true},
    {14, 75, false},
    {14, 66, .5, true},
    {15, 66, false},
    {15, 76, 1, true},
    {16, 76, false},
    {16, 75, .7, true},
    {18, 49, false},
    {18, 75, false},
    {18, 42, .5, true},
    {18, 73, 1, true},
    {24, 42, false},
    {24, 73, false},
    talcs::NoteSynthesizerDetectorMessage::Null,
};

static double msecToRate(int msec, double sampleRate) {
    if (msec == 0)
        return 0.005;
    return std::pow(0.005, 1000.0 / (msec * sampleRate));
}

class MIDIPageWidget : public QWidget, public talcs::NoteSynthesizerDetector {
    Q_OBJECT
public:
    explicit MIDIPageWidget(QWidget *parent = nullptr) : QWidget(parent) {
        auto mainLayout = new QVBoxLayout;

        auto inputGroupBox = new QGroupBox(tr("MIDI Input"));
        auto inputLayout = new QFormLayout;
        auto deviceComboBox = new QComboBox;
        inputLayout->addRow("&Device", deviceComboBox);
        inputGroupBox->setLayout(inputLayout);
        mainLayout->addWidget(inputGroupBox);

        auto synthesizerGroupBox = new QGroupBox(tr("Synthesizer"));
        auto synthesizerLayout = new QFormLayout;
        auto generatorComboBox = new QComboBox;
        generatorComboBox->addItems({tr("Sine wave"), tr("Square wave"), tr("Triangle Wave"), tr("Sawtooth wave")});
        synthesizerLayout->addRow("&Generator", generatorComboBox);

        auto amplitudeLayout = new QHBoxLayout;
        auto amplitudeSlider = new SVS::SeekBar;
        amplitudeSlider->setDefaultValue(SVS::DecibelLinearizer::decibelToLinearValue(-3));
        amplitudeSlider->setRange(SVS::DecibelLinearizer::decibelToLinearValue(-96), SVS::DecibelLinearizer::decibelToLinearValue(0));
        amplitudeLayout->addWidget(amplitudeSlider);
        auto amplitudeSpinBox = new SVS::ExpressionDoubleSpinBox;
        amplitudeSpinBox->setDecimals(1);
        amplitudeSpinBox->setRange(-96, 0);
        amplitudeSpinBox->setSpecialValueText("-INF");
        amplitudeLayout->addWidget(amplitudeSpinBox);
        auto amplitudeLabel = new QLabel(tr("&Amplitude (dB)"));
        amplitudeLabel->setBuddy(amplitudeSpinBox);
        synthesizerLayout->addRow(amplitudeLabel, amplitudeLayout);

        auto attackLayout = new QHBoxLayout;
        auto attackSlider = new SVS::SeekBar;
        attackSlider->setDefaultValue(10);
        attackSlider->setRange(0, 100);
        attackLayout->addWidget(attackSlider);
        auto attackSpinBox = new SVS::ExpressionSpinBox;
        attackSpinBox->setRange(0, 100);
        attackLayout->addWidget(attackSpinBox);
        auto attackLabel = new QLabel(tr("A&ttack (ms)"));
        attackLabel->setBuddy(attackSpinBox);
        synthesizerLayout->addRow(attackLabel, attackLayout);

        auto releaseLayout = new QHBoxLayout;
        auto releaseSlider = new SVS::SeekBar;
        releaseSlider->setDefaultValue(50);
        releaseSlider->setRange(0, 100);
        releaseLayout->addWidget(releaseSlider);
        auto releaseSpinBox = new SVS::ExpressionSpinBox;
        releaseSpinBox->setRange(0, 100);
        releaseLayout->addWidget(releaseSpinBox);
        auto releaseLabel = new QLabel(tr("&Release (ms)"));
        releaseLabel->setBuddy(releaseSpinBox);
        synthesizerLayout->addRow(releaseLabel, releaseLayout);

        auto frequencyOfALayout = new QVBoxLayout;
        auto frequencyOfASpinBox = new SVS::ExpressionDoubleSpinBox;
        frequencyOfASpinBox->setRange(440.0 * std::pow(2, -1.0/24.0), 440.0 * std::pow(2, 1.0/24.0));
        frequencyOfALayout->addWidget(frequencyOfASpinBox);
        auto adjustByProjectCheckBox = new QCheckBox(tr("Ad&just by the cent shift of the active project window"));
        frequencyOfALayout->addWidget(adjustByProjectCheckBox);
        auto frequencyOfALabel = new QLabel(tr("&Frequency of A"));
        frequencyOfALabel->setBuddy(frequencyOfASpinBox);
        synthesizerLayout->addRow(frequencyOfALabel, frequencyOfALayout);

        auto synthesizerButtonLayout = new QHBoxLayout;
        auto synthesizerTestButton = new QPushButton(tr("&Preview"));
        synthesizerTestButton->setCheckable(true);
        synthesizerButtonLayout->addWidget(synthesizerTestButton);
        auto flushButton = new QPushButton(tr("&Interrupt All Notes"));
        flushButton->setToolTip(tr("Interrupt all notes that are currently played by the synthesizer"));
        synthesizerButtonLayout->addWidget(flushButton);
        synthesizerButtonLayout->addStretch();
        synthesizerLayout->addRow(synthesizerButtonLayout);

        synthesizerGroupBox->setLayout(synthesizerLayout);
        mainLayout->addWidget(synthesizerGroupBox);
        setLayout(mainLayout);

        auto controlGroupBox = new QGroupBox(tr("Control"));

        mainLayout->addWidget(controlGroupBox, 1);

        auto ms = AudioSystem::midiSystem();

        auto deviceList = talcs::MidiInputDevice::devices();
        if (!ms->device()) {
            deviceComboBox->addItem(tr("(Not working)"), -1);
        }
        for (int i = 0; i < deviceList.size(); i++) {
            deviceComboBox->addItem(deviceList.at(i), i);
            if (ms->device() && i == ms->device()->deviceIndex())
                deviceComboBox->setCurrentIndex(i);
        }
        connect(deviceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
            auto i = deviceComboBox->itemData(index).toInt();
            if (i == -1)
                return;
            if (!ms->setDevice(i)) {
                QMessageBox::critical(this, {}, tr("Cannot open MIDI device %1").arg(deviceComboBox->itemText(index)));
                QSignalBlocker o(deviceComboBox);
                deviceComboBox->setCurrentIndex(deviceComboBox->findData(ms->device() ? ms->device()->deviceIndex() : -1));
            } else {
                QSignalBlocker o(deviceComboBox);
                if (auto notWorkingIndex = deviceComboBox->findData(-1); notWorkingIndex != -1)
                    deviceComboBox->removeItem(notWorkingIndex);
            }
        });
        connect(generatorComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
            m_cachedGenerator = index;
            m_testSynthesizer.setGenerator(static_cast<talcs::NoteSynthesizer::Generator>(index));
        });
        connect(amplitudeSlider, &SVS::SeekBar::valueChanged, this, [=](double value) {
            amplitudeSpinBox->setValue(SVS::DecibelLinearizer::linearValueToDecibel(value));
        });
        connect(amplitudeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double decibel) {
            QSignalBlocker o(amplitudeSlider);
            m_cachedAmplitude = decibel;
            amplitudeSlider->setValue(SVS::DecibelLinearizer::decibelToLinearValue(decibel));
            m_testMixer.setGain(talcs::Decibels::decibelsToGain(decibel));
        });
        connect(attackSlider, &SVS::SeekBar::valueChanged, attackSpinBox, &QSpinBox::setValue);
        connect(attackSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value) {
            QSignalBlocker o(attackSlider);
            m_cachedAttackMsec = value;
            attackSlider->setValue(value);
            m_testSynthesizer.setAttackRate(msecToRate(value, m_testSynthesizer.sampleRate()));
        });
        connect(releaseSlider, &SVS::SeekBar::valueChanged, releaseSpinBox, &QSpinBox::setValue);
        connect(releaseSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value) {
            m_cachedReleaseMsec = value;
            releaseSlider->setValue(value);
            m_testSynthesizer.setReleaseRate(msecToRate(value, m_testSynthesizer.sampleRate()));
        });
        connect(AudioSystem::outputSystem()->context(), &talcs::AbstractOutputContext::sampleRateChanged, this, [=](double sampleRate) {
            m_testSynthesizer.setAttackRate(msecToRate(attackSpinBox->value(), sampleRate));
            m_testSynthesizer.setReleaseRate(msecToRate(releaseSpinBox->value(), sampleRate));
        });
        connect(frequencyOfASpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double value) {
            m_mutex.lock();
            m_cachedFrequencyOfA = value;
            m_mutex.unlock();
            m_testSynthesizer.flush();
        });
        connect(adjustByProjectCheckBox, &QAbstractButton::clicked, this, [=](bool checked) {
            m_mutex.lock();
            if (checked) {
                m_cachedFrequencyOfA = 0;
                frequencyOfASpinBox->setDisabled(true);
            } else {
                m_cachedFrequencyOfA = frequencyOfASpinBox->value();
                frequencyOfASpinBox->setDisabled(false);
            }
            m_mutex.unlock();
            m_testSynthesizer.flush();
        });

        generatorComboBox->setCurrentIndex(m_cachedGenerator = ms->generator());
        amplitudeSpinBox->setValue(m_cachedAmplitude = ms->amplitudeDecibel());
        amplitudeSlider->setValue(SVS::DecibelLinearizer::decibelToLinearValue(amplitudeSpinBox->value()));
        attackSpinBox->setValue(m_cachedAttackMsec = ms->attackMsec());
        releaseSpinBox->setValue(m_cachedReleaseMsec = ms->releaseMsec());
        m_cachedFrequencyOfA = ms->frequencyOfA();
        if (qFuzzyIsNull(m_cachedFrequencyOfA)) {
            frequencyOfASpinBox->setValue(440.0);
            adjustByProjectCheckBox->setChecked(true);
            frequencyOfASpinBox->setDisabled(true);
        } else {
            frequencyOfASpinBox->setValue(m_cachedFrequencyOfA);
        }

        m_testSynthesizer.setDetector(this);
        m_testMixer.addSource(&m_testSynthesizer);
        AudioSystem::outputSystem()->context()->preMixer()->addSource(&m_testMixer);
        m_testSynthesizer.setGenerator(static_cast<talcs::NoteSynthesizer::Generator>(ms->generator()));
        m_testMixer.setGain(talcs::Decibels::decibelsToGain(ms->amplitudeDecibel()));
        m_testSynthesizer.setAttackRate(msecToRate(ms->attackMsec(), m_testSynthesizer.sampleRate()));
        m_testSynthesizer.setReleaseRate(msecToRate(ms->releaseMsec(), m_testSynthesizer.sampleRate()));

        connect(synthesizerTestButton, &QAbstractButton::clicked, this, [=](bool checked) {
            QMutexLocker locker(&m_mutex);
            if (checked) {
                if (!AudioSystem::outputSystem()->isReady()) {
                    synthesizerTestButton->setChecked(false);
                    return;
                }
                isTestFinished = false;
                m_currentPosition = 0;
                m_currentScoreIndex = 0;
            } else {
                m_currentScoreIndex = -2;
            }
        });
        connect(this, &MIDIPageWidget::testFinished, this, [=] {
                synthesizerTestButton->setChecked(false);
            }, Qt::QueuedConnection);
        connect(flushButton, &QAbstractButton::clicked, this, [=] {
            AudioSystem::midiSystem()->synthesizer()->noteSynthesizer()->flush();
            AudioSystem::vstConnectionSystem()->synthesizer()->noteSynthesizer()->flush();
        });
    }

    ~MIDIPageWidget() override {
        m_testMixer.removeAllSources();
        AudioSystem::outputSystem()->context()->preMixer()->removeSource(&m_testMixer);
    }

    void detectInterval(qint64 intervalLength) override {
        QMutexLocker locker(&m_mutex);
        if (m_currentScoreIndex == -2) {
            m_currentScoreIndex = -3;
            return;
        }
        if (m_currentScoreIndex == -1 && !isTestFinished) {
            isTestFinished = true;
            emit testFinished();
            return;
        }
        m_currentInterval = 97.0 / 60.0 * 4.0 * static_cast<double>(intervalLength) / m_testMixer.sampleRate();
        m_currentPosition += m_currentInterval;
    }
    talcs::NoteSynthesizerDetectorMessage nextMessage() override {
        QMutexLocker locker(&m_mutex);
        if (m_currentScoreIndex == -3) {
            m_currentScoreIndex = -1;
            return {0, talcs::NoteSynthesizerDetectorMessage::AllNotesOff};
        } else if (m_currentScoreIndex < 0) {
            return talcs::NoteSynthesizerDetectorMessage::Null;
        }
        auto message = scores[m_currentScoreIndex];
        if (message.position == -1) {
            m_currentScoreIndex = -1;
            return talcs::NoteSynthesizerDetectorMessage::Null;
        }
        if (static_cast<double>(message.position) > m_currentPosition) {
            return talcs::NoteSynthesizerDetectorMessage::Null;
        }
        message.position = static_cast<qint64>(std::round((static_cast<double>(message.position)  - (m_currentPosition - m_currentInterval)) * m_testMixer.sampleRate() / (97.0 / 60.0 * 4.0)));
        message.frequency = talcs::MidiMessage::getMidiNoteInHertz(static_cast<int>(message.frequency), qFuzzyIsNull(m_cachedFrequencyOfA) ? 440.0 : m_cachedFrequencyOfA);
        m_currentScoreIndex++;
        return message;
    }

    void accept() const {
        auto ms = AudioSystem::midiSystem();
        ms->setGenerator(m_cachedGenerator);
        ms->setAmplitudeDecibel(m_cachedAmplitude);
        ms->setAttackMsec(m_cachedAttackMsec);
        ms->setReleaseMsec(m_cachedReleaseMsec);
        ms->setFrequencyOfA(m_cachedFrequencyOfA);
        AudioSystem::vstConnectionSystem()->syncSynthesizerPreference();
    }

    talcs::MixerAudioSource m_testMixer;
    talcs::NoteSynthesizer m_testSynthesizer;

    QMutex m_mutex;
    double m_currentPosition = 0;
    double m_currentInterval = 0;
    int m_currentScoreIndex = -1;
    QAtomicInteger<bool> isTestFinished = true;

    int m_cachedGenerator;
    double m_cachedAmplitude;
    int m_cachedAttackMsec;
    int m_cachedReleaseMsec;
    double m_cachedFrequencyOfA;

signals:
    void testFinished();

};

MidiPage::MidiPage(QWidget *parent) : IOptionPage(parent) {
    auto mainLayout = new QVBoxLayout;

    m_widget = new MIDIPageWidget;

    mainLayout->addWidget(m_widget);
    setLayout(mainLayout);
}

void MidiPage::modifyOption() {
    m_widget->accept();
}

#include "MidiPage.moc"
