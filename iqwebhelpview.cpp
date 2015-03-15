#include "iqwebhelpview.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QStandardPaths>
#include <QNetworkRequest>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QWebFrame>

IqWebHelpView::IqWebHelpView(QWidget *parent) :
    IqHelpViewWidget(parent),
    m_twiceZoomHack(true),
    m_currentZoom(1),
    m_frame(new QFrame()),
    m_webView(new IqWebView()),
    m_downloadDocumentReply(Q_NULLPTR)
{
    QHBoxLayout *layout = new QHBoxLayout();
    m_frame->setLayout(layout);
    m_frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    layout->addWidget(m_webView);

    connect(m_webView, &QWebView::linkClicked, this, &IqHelpViewWidget::linkClicked);
    connect(m_webView, &QWebView::loadProgress, this, &IqHelpViewWidget::loadProgress);
    connect(m_webView, &QWebView::loadStarted, this, &IqHelpViewWidget::loadStarted);
    connect(m_webView, &QWebView::loadFinished, this, &IqHelpViewWidget::loadFinished);
    connect(m_webView, &QWebView::loadFinished, this, &IqWebHelpView::onLoadFinished);
    connect(m_webView->page(), SIGNAL(linkHovered(QString,QString,QString)), this, SIGNAL(linkHovered(QString,QString,QString)));
    m_webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

}

IqWebHelpView::~IqWebHelpView()
{
}

QWidget* IqWebHelpView::widget() const
{
    return m_frame;
}

bool IqWebHelpView::showPage(const QUrl &url)
{
    m_documentUrl = QUrl();

    if (!canShowPage(url))
        return false;

    if (url.toString().right(4) != ".pdf") {
//        m_webView->setZoomFactor(m_currentZoom);
        m_webView->load(url);
        return true;
    }

    m_webView->setZoomFactor(1);

    m_documentUrl = url;
    if (m_downloadDocumentReply) {
        disconnect(m_downloadDocumentReply, 0, this, 0);
        m_downloadDocumentReply->abort();
        m_downloadDocumentReply->deleteLater();
        m_downloadDocumentReply = Q_NULLPTR;
    }


    QFileInfo fileInfo (QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/" + url.path());
    if (fileInfo.exists()) {
        if (fileInfo.fileName().right(4) == ".pdf")
            showPdf(url);
        return true;
    }

    QNetworkRequest request;
    request.setUrl(url);
    m_downloadDocumentReply = m_accessManager.get(request);
    connect(m_downloadDocumentReply, &QNetworkReply::downloadProgress, this, &IqWebHelpView::showDownloadProgress);
    connect(m_downloadDocumentReply, &QNetworkReply::finished, this, &IqWebHelpView::processDownloadDocumentFinished);

    return true;
}

bool IqWebHelpView::canShowPage(const QUrl &url) const
{
    Q_UNUSED(url);
    return true;
}

QString IqWebHelpView::title() const
{
    return m_webView->title();
}

QUrl IqWebHelpView::url() const
{
    if (m_documentUrl.isValid())
        return m_documentUrl;

    return m_webView->url();
}

void IqWebHelpView::back()
{
    m_webView->back();
}

void IqWebHelpView::forward()

{
    m_webView->forward();
}

void IqWebHelpView::reload()
{
    m_webView->reload();
}

void IqWebHelpView::setZoomFactor(const qreal factor)
{
    m_currentZoom = factor;
    if (url().toString().right(4) == ".pdf") {
        if (m_twiceZoomHack) {
            m_webView->page()->mainFrame()->evaluateJavaScript(QString("PDFViewerApplication[\"setScale\"](101, true);"));
            m_twiceZoomHack = false;
        }
        m_webView->page()->mainFrame()->evaluateJavaScript(QString("PDFViewerApplication[\"setScale\"](%0, true);")
                                                           .arg(factor));
        return;
    }
    m_webView->setZoomFactor(factor);
}


void IqWebHelpView::findText(const QString &text)
{
    if (url().toString().right(4) == ".pdf") {
        if (m_oldFindText != text) {
            m_webView->page()->mainFrame()->evaluateJavaScript(QString("PDFViewerApplication.findBar.open();"
                                                                       "PDFViewerApplication.findBar.findField.value = \"%0\";"
                                                                       "PDFViewerApplication.findBar.dispatchEvent('');")
                                                               .arg(text));
            m_oldFindText = text;
        } else {
            m_webView->page()->mainFrame()->evaluateJavaScript(QString("PDFViewerApplication.findBar.findNextButton.click();"));
        }
        return;
    }
    m_webView->findText("", QWebPage::HighlightAllOccurrences);
    m_webView->findText(text, QWebPage::HighlightAllOccurrences);
    m_webView->findText(text, QWebPage::FindWrapsAroundDocument);
}

void IqWebHelpView::showDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal == 0) {
        emit loadProgress(100);
        return;
    }
    emit loadProgress(bytesReceived/bytesTotal);
}

void IqWebHelpView::processDownloadDocumentFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    Q_CHECK_PTR(reply);
    m_downloadDocumentReply = Q_NULLPTR;
    reply->deleteLater();

    QDir dir;
    QFileInfo fileInfo (QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/" + reply->url().path());
    qDebug() << tr("Create document dir \"%0\".")
                .arg(fileInfo.path());

    if(!dir.mkpath(fileInfo.path())) {
        qWarning() << tr("Unable to create folder \"%0\".")
                      .arg(fileInfo.path());
        return;
    }

    qDebug() << tr("Open file \"%0\" for write.")
                .arg(fileInfo.filePath());

    QFile file (fileInfo.filePath());
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << tr("Unable to open \"%0\".")
                      .arg(fileInfo.path());
        return;
    }

    qDebug() << tr("Write document data in \"%0\".")
                .arg(fileInfo.filePath());

    file.write(reply->readAll());
    file.close();

    if (fileInfo.fileName().right(4) == ".pdf")
        showPdf(reply->url());
}

void IqWebHelpView::showPdf(const QUrl &url)
{
    m_twiceZoomHack = true;
    QFileInfo fileInfo (QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/" + url.path());
    if (!fileInfo.exists()) {
        qWarning() << tr("Not found PDF document in \"%0\".")
                      .arg(fileInfo.filePath());
        return;
    }

    QString pdfjsPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/pdfjs/web/viewer.html";
    if (!QFile::exists(pdfjsPath)) {
        qWarning() << tr("Not found pdf.js viewer in \"%0\".")
                      .arg(pdfjsPath);
        return;
    }

    QUrl pdfViewerUrl ("file://" + pdfjsPath + "?file=" + fileInfo.filePath());
    qDebug() << tr("Show pdf.js viewer on path \"%0\".")
                .arg(pdfViewerUrl.toString());

    m_webView->load(pdfViewerUrl);
}

void IqWebHelpView::onLoadFinished()
{
//    setZoomFactor(m_currentZoom);
}
