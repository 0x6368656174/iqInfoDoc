#ifndef IQWEBVIEW_H
#define IQWEBVIEW_H

#include <QWebView>

class IqWebView : public QWebView
{
public:
    IqWebView();

protected:
    void contextMenuEvent(QContextMenuEvent *event);
};

#endif // IQWEBVIEW_H
