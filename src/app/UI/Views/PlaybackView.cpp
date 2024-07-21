//
// Created by fluty on 2024/2/4.
//


#include "PlaybackView.h"

#include "Controller/PlaybackController.h"
#include "Model/AppModel.h"
#include "UI/Controls/ComboBox.h"
#include "UI/Controls/EditLabel.h"
#include "UI/Controls/LineEdit.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

PlaybackView::PlaybackView(QWidget *parent) : QWidget(parent) {
    m_elTempo = new EditLabel;
    m_elTempo->setObjectName("elTempo");
    m_elTempo->setText(QString::number(m_tempo));
    auto doubleValidator = new QDoubleValidator(m_elTempo->lineEdit);
    m_elTempo->lineEdit->setValidator(doubleValidator);
    m_elTempo->setUpdateLabelWhenEditCompleted(false);
    m_elTempo->lineEdit->setAlignment(Qt::AlignCenter);
    m_elTempo->label->setAlignment(Qt::AlignCenter);
    m_elTempo->setFixedSize(64, m_contentHeight);

    m_elTimeSignature = new EditLabel;
    m_elTimeSignature->setObjectName("elTimeSignature");
    m_elTimeSignature->setUpdateLabelWhenEditCompleted(false);
    m_elTimeSignature->lineEdit->setAlignment(Qt::AlignCenter);
    m_elTimeSignature->label->setAlignment(Qt::AlignCenter);
    m_elTimeSignature->setText(QString::number(m_numerator) + "/" + QString::number(m_denominator));
    m_elTimeSignature->setFixedSize(52, m_contentHeight);

    m_btnStop = new QPushButton;
    m_btnStop->setObjectName("btnStop");
    m_btnStop->setFixedSize(m_contentHeight, m_contentHeight);
    // m_btnStop->setText("Stop");
    m_btnStop->setIcon(icoStopWhite);

    m_btnPlay = new QPushButton;
    m_btnPlay->setObjectName("btnPlay");
    m_btnPlay->setFixedSize(m_contentHeight, m_contentHeight);
    m_btnPlay->setIcon(icoPlayWhite);
    // m_btnPlay->setText("Play");
    m_btnPlay->setCheckable(true);

    m_btnPlayPause = new QPushButton(this);
    connect(m_btnPlayPause, &QPushButton::clicked, this, [=] {
        if (m_status == Paused || m_status == Stopped)
            playTriggered();
        else if (m_status == Playing)
            pauseTriggered();
    });
    m_btnPlayPause->setShortcut(Qt::Key_Space);
    m_btnPlayPause->setFixedSize(0, 0);

    m_btnPause = new QPushButton;
    m_btnPause->setObjectName("btnPause");
    m_btnPause->setFixedSize(m_contentHeight, m_contentHeight);
    m_btnPause->setIcon(icoPauseWhite);
    // m_btnPause->setText("Pause");
    m_btnPause->setCheckable(true);

    m_elTime = new EditLabel;
    m_elTime->setObjectName("elTime");
    m_elTime->setUpdateLabelWhenEditCompleted(false);
    m_elTime->lineEdit->setAlignment(Qt::AlignCenter);
    m_elTime->label->setAlignment(Qt::AlignCenter);
    m_elTime->setText(toFormattedTickTime(m_tick));
    m_elTime->setFixedSize(88, m_contentHeight);

    connect(m_elTempo, &EditLabel::editCompleted, this, [=](const QString &value) {
        auto tempo = value.toDouble();
        if (m_tempo != tempo) {
            m_tempo = tempo;
            emit setTempoTriggered(tempo);
        }
    });

    connect(m_elTimeSignature, &EditLabel::editCompleted, this, [=](const QString &value) {
        if (!value.contains('/'))
            return;

        auto splitStr = value.split('/');

        if (splitStr.size() != 2) {
            return;
        }

        bool isValidNumerator = false;
        bool isValidDenominator = false;

        auto numerator = splitStr.first().toInt(&isValidNumerator);
        auto denominator = splitStr.last().toInt(&isValidDenominator);

        // If the numerator or denominator are not positive integers, ignore the change.
        if (!(isValidNumerator && isValidDenominator && (numerator > 0) && (denominator > 0))) {
            return;
        }

        if (m_numerator != numerator || m_denominator != denominator) {
            m_numerator = numerator;
            m_denominator = denominator;
            emit setTimeSignatureTriggered(numerator, denominator);
        }
    });
    connect(m_elTime, &EditLabel::editCompleted, this, [=](const QString &value) {
        if (!value.contains(':'))
            return;

        auto splitStr = value.split(':');
        if (splitStr.size() != 3)
            return;

        auto tick = fromTickTimeString(splitStr);
        if (tick != m_tick) {
            m_tick = tick;
            emit setPositionTriggered(tick);
        }
    });
    connect(m_btnPlay, &QPushButton::clicked, this, [=] {
        emit playTriggered();
        updatePlaybackControlView();
    });
    connect(m_btnPause, &QPushButton::clicked, this, [=] {
        emit pauseTriggered();
        updatePlaybackControlView();
    });
    connect(m_btnStop, &QPushButton::clicked, this, [=] {
        emit stopTriggered();
        updatePlaybackControlView();
    });

    m_cbQuantize = new ComboBox();
    m_cbQuantize->addItems(quantizeStrings);
    m_cbQuantize->setCurrentIndex(3);
    m_cbQuantize->setFixedSize(80, m_contentHeight);

    connect(m_cbQuantize, &QComboBox::currentIndexChanged, this, [=](int index) {
        auto value = quantizeValues.at(index);
        emit setQuantizeTriggered(value);
    });

    auto mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_cbQuantize);
    mainLayout->addWidget(m_btnStop);
    mainLayout->addWidget(m_btnPlay);
    mainLayout->addWidget(m_btnPause);
    mainLayout->addWidget(m_elTime);
    mainLayout->addWidget(m_elTempo);
    mainLayout->addWidget(m_elTimeSignature);
    mainLayout->setContentsMargins(6, 6, 0, 6);
    mainLayout->setSpacing(6);
    setLayout(mainLayout);
    setContentsMargins({});
    // setMaximumHeight(44);
    // setMaximumWidth(480);

    setStyleSheet(
        "QPushButton {padding: 0px; border: none; background: none; border-radius: 6px}"
        "QPushButton:hover { background: #1AFFFFFF; }"
        "QPushButton:pressed { background: #10FFFFFF; }"
        "QPushButton#btnPlay:checked { background-color: #9BBAFF;} "
        "QPushButton#btnPause:checked { background-color: #FFCD9B;} "
        "QLabel { color: #F0F0F0; background: none; border-radius: 6px; }"
        "QLabel:hover { background: #1AFFFFFF; }"
        "QLineEdit { border-radius: 6px; }"
        "QComboBox { color: #F0F0F0; border: none; background: transparent; border-radius: 6px; }"
        "QComboBox:hover { background: #1AFFFFFF; }"
        "QComboBox:pressed { background: #10FFFFFF; }");
}
void PlaybackView::updateView() {
    m_tempo = appModel->tempo();
    m_numerator = appModel->timeSignature().numerator;
    m_denominator = appModel->timeSignature().denominator;
    m_tick = static_cast<int>(playbackController->position());
    m_status = playbackController->playbackStatus();

    updateTempoView();
    updateTimeSignatureView();
    updateTimeView();
    updatePlaybackControlView();
}
void PlaybackView::onTempoChanged(double tempo) {
    m_tempo = tempo;
    updateTempoView();
}
void PlaybackView::onTimeSignatureChanged(int numerator, int denominator) {
    m_numerator = numerator;
    m_denominator = denominator;
    updateTimeSignatureView();
    updateTimeView();
}
void PlaybackView::onPositionChanged(double tick) {
    m_tick = static_cast<int>(tick);
    updateTimeView();
}
void PlaybackView::onPlaybackStatusChanged(PlaybackStatus status) {
    m_status = status;
    updatePlaybackControlView();
}
QString PlaybackView::toFormattedTickTime(int ticks) const {
    int barTicks = 1920 * m_numerator / m_denominator;
    int beatTicks = 1920 / m_denominator;
    auto bar = ticks / barTicks + 1;
    auto beat = ticks % barTicks / beatTicks + 1;
    auto tick = ticks % barTicks % beatTicks;
    auto str = QString::asprintf("%03d", bar) + ":" + QString::asprintf("%02d", beat) + ":" +
               QString::asprintf("%03d", tick);
    return str;
}
int PlaybackView::fromTickTimeString(const QStringList &splitStr) const {
    auto bar = splitStr.at(0).toInt();
    auto beat = splitStr.at(1).toInt();
    auto tick = splitStr.at(2).toInt();
    return (bar - 1) * 1920 * m_numerator / m_denominator + (beat - 1) * 1920 / m_denominator +
           tick;
}
void PlaybackView::updateTempoView() {
    m_elTempo->setText(QString::number(m_tempo));
}
void PlaybackView::updateTimeSignatureView() {
    m_elTimeSignature->setText(QString::number(m_numerator) + "/" + QString::number(m_denominator));
}
void PlaybackView::updateTimeView() {
    m_elTime->setText(toFormattedTickTime(m_tick));
}
void PlaybackView::updatePlaybackControlView() {
    if (m_status == Playing) {
        m_btnPlay->setChecked(true);
        m_btnPlay->setIcon(icoPlayBlack);

        m_btnPause->setChecked(false);
        m_btnPause->setIcon(icoPauseWhite);
    } else if (m_status == Paused) {
        m_btnPlay->setChecked(false);
        m_btnPlay->setIcon(icoPlayWhite);

        m_btnPause->setChecked(true);
        m_btnPause->setIcon(icoPauseBlack);
    } else {
        m_btnPlay->setChecked(false);
        m_btnPlay->setIcon(icoPlayWhite);

        m_btnPause->setChecked(false);
        m_btnPause->setIcon(icoPauseWhite);
    }
}
