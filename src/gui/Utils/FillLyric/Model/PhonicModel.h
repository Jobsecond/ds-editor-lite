#ifndef DS_EDITOR_LITE_PHONICMODEL_H
#define DS_EDITOR_LITE_PHONICMODEL_H
#include <QApplication>
#include <QPainter>
#include <QMenu>
#include <QStandardItemModel>
#include <QTableView>
#include <QTextEdit>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QAbstractButton>

#include "g2pglobal.h"
#include "mandarin.h"

#include "PhonicCommon.h"
#include "../Model/PhonicNote.h"
#include "../View/PhonicDelegate.h"

namespace FillLyric {

    class PhonicModel : public QStandardItemModel {
        Q_OBJECT
    public:
        explicit PhonicModel(QTableView *tableView, bool skipSlur = false,
                             QObject *parent = nullptr)
            : QStandardItemModel(parent), m_tableView(tableView), m_skipSlur(skipSlur) {
        }

        // init
        void setSkipSlur(bool skipSlur);

        // Gui functions
        void repaintItem(QModelIndex index, const QString &text);
        int shrinkModel();
        void expandModel(int col);

        // Basic functions
        int currentLyricLength(int row);

        // RoleData functions
        static QList<int> allRoles();

        QString cellLyric(int row, int col);
        bool setLyric(int row, int col, const QString &lyric);
        QString cellSyllable(int row, int col);
        bool setSyllable(int row, int col, const QString &syllable);
        QString cellSyllableRevised(int row, int col);
        bool setSyllableRevised(int row, int col, const QString &syllableRevised);
        QStringList cellCandidates(int row, int col);
        bool setCandidates(int row, int col, const QStringList &candidate);
        int cellLyricType(int row, int col);
        bool setLyricType(int row, int col, TextType type);
        QStringList cellFermata(int row, int col);
        bool setFermata(int row, int col, const QList<QString> &fermata);
        bool cellLineFeed(int row, int col);
        bool setLineFeed(int row, int col, bool lineFeed);
        QString cellToolTip(int row, int col);

        // Cell operations
        void putData(int row, int col, const Phonic &phonic);
        Phonic takeData(int row, int col);
        void clearData(int row, int col);
        void moveData(int row, int col, int tarRow, int tarCol);

        void cellPut(const QModelIndex &index, const Phonic &phonic);
        Phonic cellTake(const QModelIndex &index);
        void cellClear(const QModelIndex &index);
        void cellMove(const QModelIndex &source, const QModelIndex &target);

        QString cellToolTip(const QModelIndex &index);

        void cellMoveLeft(const QModelIndex &index);
        void cellMoveRight(const QModelIndex &index);

        // Fermata operations
        void collapseFermata();
        void expandFermata();

        bool fermataState = false;

        int modelMaxCol = 0;

    private:
        bool m_skipSlur = false;
        QTableView *m_tableView;
    };

} // FillLyric

#endif // DS_EDITOR_LITE_PHONICMODEL_H
