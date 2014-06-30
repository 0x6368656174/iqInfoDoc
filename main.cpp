#include "iqhelpviewermainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QFont>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator ruTranslator;
    ruTranslator.load("iqInfoDoc_ru");
    a.installTranslator(&ruTranslator);

    QTranslator qtRuTranslator;
    qtRuTranslator.load("qt_ru");
    a.installTranslator(&qtRuTranslator);

    int defaultFontId = QFontDatabase::addApplicationFont("://arial.ttf");
    QString family = QFontDatabase::applicationFontFamilies(defaultFontId).at(0);
    QFont defaultFont (family);
    a.setFont(defaultFont);

    QApplication::setOrganizationName("itQuasar");
    QApplication::setApplicationName("iqHelpViewer");
    IqHelpViewerMainWindow w;
    w.show();

    return a.exec();
}
