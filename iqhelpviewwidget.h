#ifndef IQHELPVIEWWIDGET_H
#define IQHELPVIEWWIDGET_H

#include <QObject>
#include <QUrl>

class IqHelpViewWidget : public QObject
{
    Q_OBJECT
public:
    explicit IqHelpViewWidget(QObject *parent);

    virtual QWidget *widget() const = 0;

    virtual bool showPage(const QUrl &url);

    virtual bool canShowPage(const QUrl &url) const;

    virtual QUrl url() const;

    virtual QString title() const;

    virtual void setZoomFactor(const qreal factor);

    virtual void back();

    virtual void forward();

    virtual void reload();

    virtual void findText(const QString &text);

signals:
    void externalLinkActivated(const QUrl &url);
    void linkClicked(const QUrl &url);
    void loadStarted();
    void loadFinished(bool ok);
    void loadProgress(int progress);
    void linkHovered(const QString & link, const QString & title, const QString & textContent);

};

#endif // IQHELPVIEWWIDGET_H
