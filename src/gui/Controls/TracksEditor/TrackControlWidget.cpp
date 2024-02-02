//
// Created by fluty on 2024/1/29.
//

#include <QMenu>

#include "TrackControlWidget.h"

TrackControlWidget::TrackControlWidget(QListWidgetItem *item, QWidget *parent) {
    m_item = item;
    setAttribute(Qt::WA_StyledBackground);

    m_btnColor = new QPushButton;
    m_btnColor->setObjectName("btnColor");
    m_btnColor->setMaximumWidth(8);
    m_btnColor->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    m_lbTrackIndex = new QLabel("1");
    m_lbTrackIndex->setObjectName("lbTrackIndex");
    m_lbTrackIndex->setAlignment(Qt::AlignCenter);
    m_lbTrackIndex->setMinimumWidth(m_buttonSize);
    m_lbTrackIndex->setMaximumWidth(m_buttonSize);
    m_lbTrackIndex->setMinimumHeight(m_buttonSize);
    m_lbTrackIndex->setMaximumHeight(m_buttonSize);

    m_btnMute = new QPushButton("M");
    m_btnMute->setObjectName("btnMute");
    m_btnMute->setCheckable(true);
    m_btnMute->setChecked(false);
    m_btnMute->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_btnMute->setMinimumWidth(m_buttonSize);
    m_btnMute->setMaximumWidth(m_buttonSize);
    m_btnMute->setMinimumHeight(m_buttonSize);
    m_btnMute->setMaximumHeight(m_buttonSize);
    m_btnMute->setContentsMargins(0, 0, 0, 0);
    connect(m_btnMute, &QPushButton::clicked, this, [&] { emit propertyChanged(); });

    m_btnSolo = new QPushButton("S");
    m_btnSolo->setObjectName("btnSolo");
    m_btnSolo->setCheckable(true);
    m_btnSolo->setChecked(false);
    m_btnSolo->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_btnSolo->setMinimumWidth(m_buttonSize);
    m_btnSolo->setMaximumWidth(m_buttonSize);
    m_btnSolo->setMinimumHeight(m_buttonSize);
    m_btnSolo->setMaximumHeight(m_buttonSize);
    connect(m_btnSolo, &QPushButton::clicked, this, [&] { emit propertyChanged(); });

    m_leTrackName = new EditLabel();
    m_leTrackName->setText("Track 1");
    m_leTrackName->setObjectName("leTrackName");
    m_leTrackName->setMinimumHeight(m_buttonSize);
    m_leTrackName->setMaximumHeight(m_buttonSize);
    connect(m_leTrackName, &EditLabel::valueChanged, this, [&] { emit propertyChanged(); });

    m_muteSoloTrackNameLayout = new QHBoxLayout;
    m_muteSoloTrackNameLayout->setObjectName("muteSoloTrackNameLayout");
    m_muteSoloTrackNameLayout->addWidget(m_lbTrackIndex);
    m_muteSoloTrackNameLayout->addWidget(m_btnMute);
    m_muteSoloTrackNameLayout->addWidget(m_btnSolo);
    m_muteSoloTrackNameLayout->addWidget(m_leTrackName);
    m_muteSoloTrackNameLayout->setSpacing(4);
    m_muteSoloTrackNameLayout->setContentsMargins(4, 8, 4, 8);

    auto placeHolder = new QWidget;
    placeHolder->setMinimumWidth(m_buttonSize);
    placeHolder->setMaximumWidth(m_buttonSize);
    placeHolder->setMinimumHeight(m_buttonSize);
    placeHolder->setMaximumHeight(m_buttonSize);

    m_sbarPan = new SeekBar;
    m_sbarPan->setObjectName("m_sbarPan");
    connect(m_sbarPan, &SeekBar::valueChanged, this, &TrackControlWidget::onSeekBarValueChanged);
    //        m_panSlider->setValue(50);

    m_lePan = new EditLabel();
    m_lePan->setText("C");
    m_lePan->setObjectName("lePan");
    m_lePan->setMinimumWidth(40);
    m_lePan->setMaximumWidth(40);
    m_lePan->setMinimumHeight(m_buttonSize);
    m_lePan->setMaximumHeight(m_buttonSize);
    m_lePan->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    m_lePan->label->setAlignment(Qt::AlignCenter);
    m_lePan->lineEdit->setAlignment(Qt::AlignCenter);
    m_lePan->setEnabled(false);

    m_sbarGain = new SeekBar;
    m_sbarGain->setObjectName("m_sbarGain");
    m_sbarGain->setMax(120);
    m_sbarGain->setMin(0);
    m_sbarGain->setDefaultValue(100);
    connect(m_sbarGain, &SeekBar::valueChanged, this, &TrackControlWidget::onSeekBarValueChanged);

    m_leVolume = new EditLabel();
    m_leVolume->setText("0dB");
    m_leVolume->setObjectName("leVolume");
    m_leVolume->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_leVolume->label->setAlignment(Qt::AlignCenter);
    m_leVolume->lineEdit->setAlignment(Qt::AlignCenter);
    m_leVolume->setMinimumWidth(40);
    m_leVolume->setMaximumWidth(40);
    m_leVolume->setMinimumHeight(20);
    m_leVolume->setMaximumHeight(20);
    m_leVolume->setEnabled(false);

    m_panVolumeSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    m_panVolumeLayout = new QHBoxLayout;
    m_panVolumeLayout->addWidget(placeHolder);
    m_panVolumeLayout->addWidget(m_sbarPan);
    m_panVolumeLayout->addWidget(m_lePan);
    m_panVolumeLayout->addWidget(m_sbarGain);
    m_panVolumeLayout->addWidget(m_leVolume);
    m_panVolumeLayout->setSpacing(0);
    m_panVolumeLayout->setContentsMargins(4, 0, 4, 8);

    m_controlWidgetLayout = new QVBoxLayout;
    m_controlWidgetLayout->addLayout(m_muteSoloTrackNameLayout);
    m_controlWidgetLayout->addLayout(m_panVolumeLayout);
    m_controlWidgetLayout->addItem(m_panVolumeSpacer);

    // m_levelMeter = new LevelMeter(Qt::Vertical);
    // m_levelMeter->initBuffer(8);
    // m_levelMeter->setMaximumWidth(12);

    m_mainLayout = new QHBoxLayout;
    m_mainLayout->setObjectName("TrackControlPanel");
    m_mainLayout->addWidget(m_btnColor);
    m_mainLayout->addLayout(m_controlWidgetLayout);
    // m_mainLayout->addWidget(m_levelMeter);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins({});

    setLayout(m_mainLayout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // setFixedHeight(72);

    auto actionInsert = new QAction("Insert new track", this);
    connect(actionInsert, &QAction::triggered, this, [&] { emit insertNewTrackTriggered(); });
    auto actionRemove = new QAction("Delete", this);
    connect(actionRemove, &QAction::triggered, this, [&] { emit removeTrackTriggerd(); });
    auto actionAddAudioClip = new QAction("Add audio clip", this);
    connect(actionAddAudioClip, &QAction::triggered, this, [&] { emit addAudioClipTriggered(); });

    addAction(actionInsert);
    addAction(actionRemove);
    addAction(actionAddAudioClip);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    setStyleSheet(R"(
TrackControlWidget {
    background-color: #2A2B2C;
    border-style: none;
    border-bottom: 1px solid rgb(72, 75, 78)
}

QLabel {
    color: #F0F0F0;
}

QPushButton {
    padding: 0px
}

SeekBar {
    qproperty-trackInactiveColor: #707070;
    qproperty-trackActiveColor: #9BBAFF;
    qproperty-thumbBorderColor: #454545;
}

TrackControlWidget > QPushButton#btnColor {
    background-color: #9BBAFF;
    border-style: none;
    border-radius: 0px;
    width: 10px;
    padding-bottom: 1px;
}

TrackControlWidget > QLabel#lbTrackIndex{
    font-size: 11pt;
    color: #D0D0D0;
}

TrackControlWidget > QPushButton#btnMute{
    color: #F0F0F0;
    font-size: 9pt;
    background-color: #2A2B2C;
    border-style: none;
    border-radius: 4px;
    border: 1px solid #606060;
}

TrackControlWidget > QPushButton#btnMute:checked {
    color: #000;
    background-color: #FF9B9D;
    border: none
}

TrackControlWidget > QPushButton#btnSolo{
    color: #F0F0F0;
    font-size: 9pt;
    background-color: #2A2B2C;
    border-style: none;
    border-radius: 4px;
    border: 1px solid #606060;
}

TrackControlWidget > QPushButton#btnSolo:checked {
    color: #000;
    background-color: #FFCD9B;
    border: none
}

)");
}
int TrackControlWidget::trackIndex() const {
    return m_lbTrackIndex->text().toInt();
}
void TrackControlWidget::setTrackIndex(int i) {
    m_lbTrackIndex->setText(QString::number(i));
}
QString TrackControlWidget::name() const {
    return m_leTrackName->text();
}
void TrackControlWidget::setName(const QString &name) {
    m_leTrackName->setText(name);
    // emit propertyChanged();
}
DsTrackControl TrackControlWidget::control() const {
    DsTrackControl control;
    control.setGain(m_sbarGain->value()); // TODO: linear to dB
    control.setPan(m_sbarPan->value());
    control.setMute(m_btnMute->isChecked());
    control.setSolo(m_btnSolo->isChecked());
    return control;
}
void TrackControlWidget::setControl(const DsTrackControl &control) {
    m_sbarGain->setValue(control.gain());
    m_sbarPan->setValue(control.pan());
    m_btnMute->setChecked(control.mute());
    m_btnSolo->setChecked(control.solo());
    // emit propertyChanged();
}
void TrackControlWidget::setNarrowMode(bool on) {
    if (on) {
        for (int i = 0; i < m_panVolumeLayout->count(); ++i) {
            QWidget *w = m_panVolumeLayout->itemAt(i)->widget();
            if (w != nullptr)
                w->setVisible(false);
            m_panVolumeLayout->setContentsMargins(0, 0, 0, 0);
        }
    } else {
        for (int i = 0; i < m_panVolumeLayout->count(); ++i) {
            QWidget *w = m_panVolumeLayout->itemAt(i)->widget();
            if (w != nullptr)
                w->setVisible(true);
            m_panVolumeLayout->setContentsMargins(4, 0, 4, 8);
        }
    }
}
void TrackControlWidget::onTrackUpdated(const DsTrack &track) {
    m_leTrackName->setText(track.name());
    auto control = track.control();
    m_sbarGain->setValue(control.gain());
    m_sbarPan->setValue(control.pan());
    m_btnMute->setChecked(control.mute());
    m_btnSolo->setChecked(control.solo());
}
// void TrackControlWidget::setHeight(int h) {
//     m_item->setSizeHint(QSize(360, h));
// }
// void TrackControlWidget::setScale(qreal sx, qreal sy) {
//     m_item->setSizeHint(QSize(360, qRound(72 * sy)));
// }
void TrackControlWidget::onSeekBarValueChanged() {
    emit propertyChanged();
}
void TrackControlWidget::contextMenuEvent(QContextMenuEvent *event) {
    // auto menu = new QMenu;
    // auto actionInsert = new QAction("Insert new track", this);
    // connect(actionInsert, &QAction::triggered, this, [&] { emit insertNewTrackTriggered();});
    // auto actionRemove = new QAction("Delete", this);
    // connect(actionRemove, &QAction::triggered, this, [&] { emit removeTrackTriggerd(); });
    //
    // menu->addAction(actionInsert);
    // menu->addAction(actionRemove);
    // menu->exec(event->globalPos());
    QWidget::contextMenuEvent(event);
}