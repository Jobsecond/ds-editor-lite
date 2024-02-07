#ifndef DS_EDITOR_LITE_CLEANLYRIC_H
#define DS_EDITOR_LITE_CLEANLYRIC_H

#include <QList>
#include <QString>

namespace FillLyric {
    class CleanLyric {
    public:
        enum LyricType { Letter, Hanzi, Digit, Fermata, Kana, Space, Other };
        static QPair<QList<QStringList>, QList<QList<int>>>
            cleanLyric(const QString &lyric, const QString &fermata = "-");

        static bool isLetter(QChar c);

        static bool isHanzi(QChar c);

        static bool isKana(QChar c);

        static bool isSpecialKana(QChar c);
    };
}



#endif // DS_EDITOR_LITE_CLEANLYRIC_H
