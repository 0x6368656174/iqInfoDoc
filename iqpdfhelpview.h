#ifndef IQPDFHELPVIEW_H
#define IQPDFHELPVIEW_H

#include "iqhelpviewwidget.h"
#include "iqscrollarea.h"
#include <QLabel>
#include <QHttp>
#include <QBuffer>
#include <QFrame>
#include <poppler/qt4/poppler-qt4.h>

class IqPdfHelpView : public IqHelpViewWidget
{
    Q_OBJECT
public:
    explicit IqPdfHelpView(QWidget *parent = 0);

    ~IqPdfHelpView();

    virtual QWidget *widget() const;

    virtual bool showPage(const QUrl &url);

    virtual bool canShowPage(const QUrl &url) const;

    virtual QString title() const;

    inline virtual QUrl url() const {return _url;}

//    virtual void back();

//    virtual void forward();

    virtual void reload();

    virtual void setZoomFactor(const qreal factor);

    virtual void findText(const QString &text);

private slots:
    void onRequestFinished(int id, bool error);
    void paintPdf();

private:
    IqScrollArea *_scrollArea;
    QWidget *_widget;
    QFrame *_frame;
    QList<QLabel *> _pages;
    QHttp _http;
    QByteArray _pdfData;
    Poppler::Document* _pdfDocument;
    QList<QPixmap> _pagesCache;
    QList<Poppler::Page *>_pdfPages;
    QList<QList<QRectF> > _searchResults;
    QBuffer _ioBuffer;
    int _requesId;
    int _currentSearchPage;
    int _currentSearchResult;
    QString _searchText;
    qreal _zoomFactor;
    QUrl _url;

    void paintFindResult();
};

#endif // IQPDFHELPVIEW_H
