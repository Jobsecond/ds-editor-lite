#include "PrevLineAction.h"

namespace FillLyric {

    PrevLineAction *PrevLineAction::build(int row, PhonicModel *model) {
        auto *action = new PrevLineAction;
        action->m_model = model;
        action->m_newLine = row;
        return action;
    }

    void PrevLineAction::execute() {
        m_model->insertRow(m_newLine);
    }

    void PrevLineAction::undo() {
        m_model->removeRow(m_newLine);
    }
} // FillLyric