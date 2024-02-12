//
// Created by fluty on 2023/8/13.
//

#include "QKeyEvent"
#include <QHBoxLayout>

#include "EditLabel.h"

EditLabel::EditLabel(QWidget *parent) {
    setAttribute(Qt::WA_StyledBackground);
    setAttribute(Qt::WA_Hover, true);

    label = new QLabel;
    label->setText("Label");
    label->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    label->installEventFilter(this);
    label->setStyleSheet(QString("padding: 2px;"));

    lineEdit = new QLineEdit;
    lineEdit->installEventFilter(this);
    lineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    // lineEdit->setStyleSheet(QString("border: 1px solid #606060;"
    //                                 "border-bottom: 2px solid #9BBAFF;"
    //                                 "background-color: #202122; "
    //                                 "border-radius: 4px; color: #F0F0F0;"
    //                                 "selection-color: #000;"
    //                                 "selection-background-color: #9BBAFF;"
    //                                 "padding: 2px;"));

    this->addWidget(label);
    this->addWidget(lineEdit);
    this->setCurrentWidget(label);

    // this->setMaximumHeight(this->sizeHint().height());
    installEventFilter(this);
}

bool EditLabel::eventFilter(QObject *object, QEvent *event) {
    // if (event->type() == QEvent::HoverEnter) {
    //     setCursor(Qt::IBeamCursor);
    // } else if (event->type() == QEvent::HoverLeave) {
    //     setCursor(Qt::ArrowCursor);
    // }
    if (!isEnabled())
        return QWidget::eventFilter(object, event);

    if (object == label) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            lineEdit->setText(label->text());
            this->setCurrentWidget(lineEdit);
            m_editing = true;
        }
    } else if (object == lineEdit) {
        auto commit = [&]() {
            if (m_updateLabelWhenEditCompleted)
                setText(lineEdit->text());
            else
                setCurrentWidget(label);

            if (m_editing)
                emit editCompleted(lineEdit->text());

            m_editing = false;
        };
        if (event->type() == QEvent::KeyPress) {
            auto keyEvent = dynamic_cast<QKeyEvent *>(event);
            auto key = keyEvent->key();
            if (key == Qt::Key_Escape || key == Qt::Key_Return) // Esc or Enter pressed
                commit();
        } else if (event->type() == QEvent::FocusOut) {
            //            qDebug() << "FocusOut";
            commit();
        } else if (event->type() == QEvent::ContextMenu) {
            //            qDebug() << "ContextMenu";
        }
    }

    return QWidget::eventFilter(object, event);
}

QString EditLabel::text() const {
    return label->text();
}

void EditLabel::setText(const QString &text) {
    setCurrentWidget(label);
    label->setText(text);
    // emit valueChanged(text);
}
void EditLabel::setUpdateLabelWhenEditCompleted(bool on) {
    m_updateLabelWhenEditCompleted = on;
}