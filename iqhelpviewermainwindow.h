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

private:
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
    QString rus(const QString &lat) const;
    QString lat(const QString &rus) const;
    void setCurrentView(IqHelpViewWidget * view);
    void checkBackForwardEnabled();
    void addUrlToHistory(const QUrl &url);

private:
    Ui::IqHelpViewerMainWindow *ui;
    QLineEdit *m_urlLineEdit;
    QLineEdit *m_findStringLineEdit;
    QLabel *m_findLabel;
    QLabel *m_scaleLabel;
    QComboBox *m_scaleComboBox;
    QRadioButton *m_latLocaleButton;
    QRadioButton *m_rusLocaleButton;
    QList<IqHelpViewWidget *> m_helpViewWidgets;
    IqHelpViewWidget *m_currentView;
    QList<QUrl> m_history;
    QWidget *m_mainWidget;
    int m_currentHistoryIndex;
    bool m_backOrForward;
};

#endif // IQHELPVIEWERMAINWINDOW_H
