#ifndef DS_EDITOR_LITE_WRAPCELLCLEARACTION_H
#define DS_EDITOR_LITE_WRAPCELLCLEARACTION_H

#include <QObject>
#include <QModelIndex>

#include "../../History/MAction.h"
#include "../../Model/PhonicModel.h"

namespace FillLyric {

    class WrapCellClearAction final : public MAction {
    public:
        static WrapCellClearAction *build(const QModelIndex &index, PhonicModel *model);
        void execute() override;
        void undo() override;

    private:
        PhonicModel *m_model = nullptr;
        QModelIndex m_index;

        int m_cellIndex;

        Phonic m_phonic;
    };

} // FillLyric

#endif // DS_EDITOR_LITE_WRAPCELLCLEARACTION_H
