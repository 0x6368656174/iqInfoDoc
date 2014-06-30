#include "iqhelpviewwidget.h"
#include <QUrl>

IqHelpViewWidget::IqHelpViewWidget(QObject *parent) :
    QObject(parent)
{
}

bool IqHelpViewWidget::showPage(const QUrl &url)
{
    Q_UNUSED(url);
    return false;
}

bool IqHelpViewWidget::canShowPage(const QUrl &url) const
{
    Q_UNUSED(url);
    return false;
}

QString IqHelpViewWidget::title() const
{
    return "";
}

QUrl IqHelpViewWidget::url() const
{
    return QUrl();
}

void IqHelpViewWidget::back()
{
}

void IqHelpViewWidget::forward()
{
}

void IqHelpViewWidget::reload()
{
}

void IqHelpViewWidget::setZoomFactor(const qreal factor)
{
    Q_UNUSED(factor);
}

void IqHelpViewWidget::findText(const QString &text)
{
    Q_UNUSED(text);
}
