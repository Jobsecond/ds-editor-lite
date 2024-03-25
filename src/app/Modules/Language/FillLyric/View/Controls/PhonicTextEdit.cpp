#include "PhonicTextEdit.h"

namespace FillLyric {

    PhonicTextEdit::PhonicTextEdit(QWidget *parent) : QTextEdit(parent) {
        QFont f = this->font();
        f.setPointSizeF(11);
        setFont(f);
    }

    void PhonicTextEdit::wheelEvent(QWheelEvent *event) {
        if (event->modifiers() & Qt::ControlModifier) {
            const auto fontSizeDelta = event->angleDelta().y() / 120.0;
            QFont font = this->font();
            const auto newSize = font.pointSizeF() + fontSizeDelta;
            if (newSize > 0) {
                font.setPointSizeF(newSize);
                this->setFont(font);
                Q_EMIT fontChanged();
            }
            event->accept();
            return;
        }
        QTextEdit::wheelEvent(event);
    }

} // FillLyric