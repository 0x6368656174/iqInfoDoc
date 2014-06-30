#ifndef IQWEBHELPVIEW_H
#define IQWEBHELPVIEW_H

#include "iqwebview.h"
#include <QFrame>
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
    QFrame *_frame;
    IqWebView *_webView;
};

#endif // IQWEBHELPVIEW_H
