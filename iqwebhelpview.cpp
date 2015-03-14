#include "iqwebhelpview.h"
#include <QDebug>
#include <QHBoxLayout>

IqWebHelpView::IqWebHelpView(QWidget *parent) :
    IqHelpViewWidget(parent),
    m_frame(new QFrame()),
    m_webView(new IqWebView())
{
    QHBoxLayout *layout = new QHBoxLayout();
    m_frame->setLayout(layout);
    m_frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    layout->addWidget(m_webView);

    connect(m_webView, SIGNAL(linkClicked(QUrl)), this, SIGNAL(linkClicked(QUrl)));
    connect(m_webView, SIGNAL(loadProgress(int)), this, SIGNAL(loadProgress(int)));
    connect(m_webView, SIGNAL(loadStarted()), this, SIGNAL(loadStarted()));
    connect(m_webView, SIGNAL(loadFinished(bool)), this, SIGNAL(loadFinished(bool)));
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
    if (!canShowPage(url))
        return false;

    m_webView->load(url);
    return true;
}

bool IqWebHelpView::canShowPage(const QUrl &url) const
{
    if (url.toString().right(4) != ".pdf")
        return true;

    return false;
}

QString IqWebHelpView::title() const
{
    return m_webView->title();
}

QUrl IqWebHelpView::url() const
{
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
    m_webView->setZoomFactor(factor);
}


void IqWebHelpView::findText(const QString &text)
{
    m_webView->findText("", QWebPage::HighlightAllOccurrences);
    m_webView->findText(text, QWebPage::HighlightAllOccurrences);
    m_webView->findText(text, QWebPage::FindWrapsAroundDocument);
}
