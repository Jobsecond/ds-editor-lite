#ifndef DS_EDITOR_LITE_PHONICDELEGATE_H
#define DS_EDITOR_LITE_PHONICDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QDebug>
#include <QTableView>
#include <QStandardItemModel>

#include "../Utils/CleanLyric.h"

namespace FillLyric {
    using LyricType = CleanLyric::LyricType;
    class PhonicDelegate : public QStyledItemDelegate {
    public:
        explicit PhonicDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {
        }

        enum PhonicRole {
            Syllable = Qt::UserRole,
            Candidate,
            SyllableRevised,
            LyricType,
            FermataAddition
        };

        void paint(QPainter *painter, const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override {
            QStyleOptionViewItem newOption(option);
            newOption.displayAlignment = Qt::AlignBottom | Qt::AlignHCenter;
            QStyledItemDelegate::paint(painter, newOption, index);

            // 获取原文本
            QString text = index.data(Qt::DisplayRole).toString();

            // 获取注音文本
            QString syllable = index.data(PhonicRole::Syllable).toString();

            // 获取人工修订的注音文本
            QString syllableRevised = index.data(PhonicRole::SyllableRevised).toString();

            // 设置注音的字体
            QFont syllableFont = option.font;
            syllableFont.setPointSize(syllableFont.pointSize() - 2); // 缩小字体
            painter->setFont(syllableFont);

            auto lyricType = index.data(PhonicRole::LyricType).toInt();

            // 获取候选发音列表
            QStringList candidateList = index.data(PhonicRole::Candidate).toStringList();
            // 若候选发音大于1个，注音颜色为红色
            if (syllableRevised != "") {
                painter->setPen(Qt::blue);
                syllable = syllableRevised;
            } else if (text == syllable && lyricType != LyricType::Fermata) {
                painter->setPen(Qt::darkRed);
            } else if (candidateList.size() > 1) {
                painter->setPen(Qt::red);
            } else {
                painter->setPen(Qt::black);
            }
            // 文字绘制在原字体的上方
            painter->drawText(option.rect, Qt::AlignTop | Qt::AlignHCenter, syllable);
        }
    };
}


#endif // DS_EDITOR_LITE_PHONICDELEGATE_H
