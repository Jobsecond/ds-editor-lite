//
// Created by fluty on 24-9-26.
//

#ifndef INFERINPUTNOTE_H
#define INFERINPUTNOTE_H

#include <QList>

class Note;
class InferInputNote {
public:
    explicit InferInputNote(const Note &note);

    int id = -1;
    int start = 0;
    int length = 0;
    int key = -1;
    bool isRest = false;
    bool isSlur = false;
    QStringList aheadNames;
    QStringList normalNames;

    QList<int> aheadOffsets;
    QList<int> normalOffsets;

    friend bool operator==(const InferInputNote &lhs, const InferInputNote &rhs) {
        return lhs.id == rhs.id && lhs.start == rhs.start && lhs.length == rhs.length &&
               lhs.key == rhs.key && lhs.isRest == rhs.isRest && lhs.isSlur == rhs.isSlur &&
               lhs.aheadNames == rhs.aheadNames && lhs.normalNames == rhs.normalNames &&
               lhs.aheadOffsets == rhs.aheadOffsets && lhs.normalOffsets == rhs.normalOffsets;
    }

    friend bool operator!=(const InferInputNote &lhs, const InferInputNote &rhs) {
        return !(lhs == rhs);
    }
};

#endif //INFERINPUTNOTE_H
