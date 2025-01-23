//
// Created by fluty on 24-2-11.
//
#include <QApplication>

#include "TestAnchorEditor.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    auto font = QApplication::font();
    font.setHintingPreference(QFont::PreferNoHinting);
    QApplication::setFont(font);

    TestAnchorEditor window;
    window.show();

    return QApplication::exec();
}