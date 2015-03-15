#ifndef IQWEBHELPVIEW_H
#define IQWEBHELPVIEW_H

#include "iqwebview.h"
#include <QFrame>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "iqhelpviewwidget.h"

class IqWebHelpView : public IqHelpViewWidget
{
    Q_OBJECT
public:
    explicit IqWebHelpView(QWidget *parent = 0);

    ~IqWebHelpView();

    virtual QWidget *widget() const;

    virtual bool showPage(const QUrl &url);

    virtual bool canShowPage(const QUrl &url) const;

    virtual QString title() const;

    virtual QUrl url() const;

    virtual void back();

    virtual void forward();

    virtual void reload();

    virtual void setZoomFactor(const qreal factor);

    virtual void findText(const QString &text);

private:
    void showDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void processDownloadDocumentFinished();
    void showPdf(const QUrl &url);
    void onLoadFinished();

private:
    bool m_twiceZoomHack;
    qreal m_currentZoom;
    QString m_oldFindText;
    QFrame *m_frame;
    IqWebView *m_webView;
    QNetworkAccessManager m_accessManager;
    QNetworkReply *m_downloadDocumentReply;
    QUrl m_documentUrl;
};

#endif // IQWEBHELPVIEW_H
