#include "iqhelpviewermainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QFont>
#include <QFontDatabase>
#include <QDebug>
#include <QStandardPaths>
#include <QLibraryInfo>
#include <QDir>

static bool copyRecursively(const QString &srcFilePath,
                            const QString &tgtFilePath)
{
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir()) {
        QDir targetDir(tgtFilePath);
        targetDir.cdUp();
        if (!targetDir.mkpath(QFileInfo(tgtFilePath).fileName()))
            return false;
        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        foreach (const QString &fileName, fileNames) {
            const QString newSrcFilePath
                    = srcFilePath + QLatin1Char('/') + fileName;
            const QString newTgtFilePath
                    = tgtFilePath + QLatin1Char('/') + fileName;
            if (!copyRecursively(newSrcFilePath, newTgtFilePath))
                return false;
        }
    } else {
        if (!QFile::copy(srcFilePath, tgtFilePath))
            return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QTranslator ruTranslator;
    ruTranslator.load("://iqInfoDoc_ru");
    a.installTranslator(&ruTranslator);

    QTranslator qtRuTranslator;
    qtRuTranslator.load("qt_ru", QLibraryInfo::location (QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtRuTranslator);

    QTranslator qtBaseRuTranslator;
    qtBaseRuTranslator.load("qtbase_ru", QLibraryInfo::location (QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtBaseRuTranslator);

    int defaultFontId = QFontDatabase::addApplicationFont("://arial.ttf");
    QString family = QFontDatabase::applicationFontFamilies(defaultFontId).at(0);
    QFont defaultFont (family);
    a.setFont(defaultFont);

    QApplication::setOrganizationName("itQuasar");
    QApplication::setApplicationName("iqInfoDoc");
    QApplication::setApplicationVersion("1.0.0");

    qDebug() << QObject::tr("Remove cache from \"%0\".")
                .arg(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    QDir cacheDir (QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if (!cacheDir.removeRecursively()) {
        qCritical() << QObject::tr("Unable to remove cache dir from \"%0\".")
                       .arg(cacheDir.path());
        exit(1);
    }

    QDir pdfJsDir (QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/pdfjs");
    qDebug() << QObject::tr("Unpack pdf.js to \"%0\".")
                .arg(pdfJsDir.path());
    if (!pdfJsDir.removeRecursively()) {
        qCritical() << QObject::tr("Unable to remove pdf.js dir from \"%0\".")
                       .arg(pdfJsDir.path());
        exit(1);
    }
    if(!copyRecursively("://pdfjs/", pdfJsDir.path())) {
        qCritical() << QObject::tr("Unbale to copy pdf.js to \"%0\".")
                       .arg(pdfJsDir.path());
        exit(1);
    }

    IqHelpViewerMainWindow w;
    w.show();

    return a.exec();
}
