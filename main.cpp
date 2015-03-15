#include "iqhelpviewermainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QFont>
#include <QFontDatabase>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

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
    QApplication::setApplicationName("iqInfoDoc");

    qDebug() << QObject::tr("Remove cache from \"%0\".")
                .arg(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    QDir cacheDir (QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if (!cacheDir.removeRecursively()) {
        qCritical() << QObject::tr("Unable to remove cache dir from \"%0\".")
                       .arg(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
        exit(1);
    }

    IqHelpViewerMainWindow w;
    w.show();

    return a.exec();
}
