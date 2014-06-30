#include "iqwebhelpview.h"
#include <QDebug>
#include <QHBoxLayout>

IqWebHelpView::IqWebHelpView(QWidget *parent) :
    IqHelpViewWidget(parent),
    _frame(new QFrame()),
    _webView(new IqWebView())
{
    QHBoxLayout *layout = new QHBoxLayout();
    _frame->setLayout(layout);
    _frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    layout->addWidget(_webView);

    connect(_webView, SIGNAL(linkClicked(QUrl)), this, SIGNAL(linkClicked(QUrl)));
    connect(_webView, SIGNAL(loadProgress(int)), this, SIGNAL(loadProgress(int)));
    connect(_webView, SIGNAL(loadStarted()), this, SIGNAL(loadStarted()));
    connect(_webView, SIGNAL(loadFinished(bool)), this, SIGNAL(loadFinished(bool)));
    connect(_webView->page(), SIGNAL(linkHovered(QString,QString,QString)), this, SIGNAL(linkHovered(QString,QString,QString)));
    _webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
}

IqWebHelpView::~IqWebHelpView()
{
}

QWidget* IqWebHelpView::widget() const
{
    return _frame;
}

bool IqWebHelpView::showPage(const QUrl &url)
{
    if (!canShowPage(url))
        return false;

    _webView->load(url);
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
    return _webView->title();
}

QUrl IqWebHelpView::url() const
{
    return _webView->url();
}

void IqWebHelpView::back()
{
    _webView->back();
}

void IqWebHelpView::forward()

{
    _webView->forward();
}

void IqWebHelpView::reload()
{
    _webView->reload();
}


void IqWebHelpView::setZoomFactor(const qreal factor)
{
    _webView->setZoomFactor(factor);
}


void IqWebHelpView::findText(const QString &text)
{
    _webView->findText("", QWebPage::HighlightAllOccurrences);
    _webView->findText(text, QWebPage::HighlightAllOccurrences);
    _webView->findText(text, QWebPage::FindWrapsAroundDocument);
}
