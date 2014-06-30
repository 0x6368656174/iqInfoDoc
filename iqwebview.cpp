#include "iqwebview.h"
#include <QMenu>
#include <QDebug>

IqWebView::IqWebView()
{
}

void IqWebView::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);
    if (!page())
        return;
    QWebPage *webPage = page();
    QMenu *standartMenu = page()->createStandardContextMenu();
    for (int i = 0; i < standartMenu->actions().count(); i++)
    {
        QAction *standartAction = standartMenu->actions().at(i);

        if (standartAction->text() == webPage->action(QWebPage::DownloadImageToDisk)->text() ||
                standartAction->text() == webPage->action(QWebPage::OpenImageInNewWindow)->text() ||
                standartAction->text() == webPage->action(QWebPage::CopyImageToClipboard)->text() ||
                standartAction->text() == webPage->action(QWebPage::OpenLinkInNewWindow)->text() ||
                standartAction->text() == webPage->action(QWebPage::OpenFrameInNewWindow)->text() ||
                standartAction->text() == webPage->action(QWebPage::DownloadLinkToDisk)->text())
        {
            standartAction->setVisible(false);
        }
    }

    connect(standartMenu, SIGNAL(aboutToHide()), standartMenu, SLOT(deleteLater()));
    standartMenu->exec(QCursor::pos());
}
