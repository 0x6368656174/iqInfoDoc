#ifndef IQHELPVIEWERMAINWINDOW_H
#define IQHELPVIEWERMAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QRadioButton>
#include <QStack>
#include <QWidget>
#include "iqhelpviewwidget.h"

namespace Ui {
class IqHelpViewerMainWindow;
}

class IqHelpViewerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit IqHelpViewerMainWindow(QWidget *parent = 0);
    ~IqHelpViewerMainWindow();

private slots:
    void showHomePage();
    void showPage(const QUrl &url);
    void onLoadFinished(bool ok);
    void setLoadProgress(const int progress);
    void showLinkTitle(const QString & link, const QString & title, const QString & textContent);
    void onLoadStarted();
    void back();
    void forward();
    void setZoomFactor(const QString &factor);
    void zoomIn();
    void zoomOut();
    void findText();
    void showAbout();
    void showAboutQt();
    void rusificateFindString();

private:
    Ui::IqHelpViewerMainWindow *ui;
    QLineEdit *_urlLineEdit;
    QLineEdit *_findStringLineEdit;
    QLabel *_findLabel;
    QLabel *_scaleLabel;
    QComboBox *_scaleComboBox;
    QRadioButton *_latLocaleButton;
    QRadioButton *_rusLocaleButton;
    QList<IqHelpViewWidget *> _helpViewWidgets;
    IqHelpViewWidget *_currentView;
    QList<QUrl> _history;
    QWidget *_mainWidget;
    int _currentHistoryIndex;
    bool _backOrForward;

    QString rus(const QString &lat) const;
    QString lat(const QString &rus) const;
    void setCurrentView(IqHelpViewWidget * view);
    void checkBackForwardEnabled();
    void addUrlToHistory(const QUrl &url);
};

#endif // IQHELPVIEWERMAINWINDOW_H
