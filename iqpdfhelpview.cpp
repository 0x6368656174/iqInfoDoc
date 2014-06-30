#include "iqpdfhelpview.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QPainter>
#include <QScrollBar>
#include <QApplication>
#include <poppler/qt4/poppler-qt4.h>

IqPdfHelpView::IqPdfHelpView(QWidget *parent) :
    IqHelpViewWidget(parent),
    _scrollArea(new IqScrollArea()),
    _widget(new QWidget()),
    _frame(new QFrame()),
    _pdfDocument(NULL),
    _ioBuffer(&_pdfData),
    _currentSearchPage(0),
    _currentSearchResult(-1),
    _zoomFactor(1)
{
    QVBoxLayout *frameLayout = new QVBoxLayout();
    _frame->setLayout(frameLayout);
    _frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    frameLayout->addWidget(_scrollArea);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(20);
    _widget->setLayout(layout);
    _scrollArea->setWidget(_widget);
    _scrollArea->setFrameStyle(QFrame::NoFrame);
    connect(_scrollArea, SIGNAL(resized()), this, SLOT(paintPdf()));
    connect(&_http, SIGNAL(requestFinished(int,bool)), this, SLOT(onRequestFinished(int,bool)));
}

IqPdfHelpView::~IqPdfHelpView()
{
}

QWidget* IqPdfHelpView::widget() const
{
    return _frame;
}

bool IqPdfHelpView::showPage(const QUrl &url)
{
    if (!canShowPage(url))
        return false;

    _url = url;

    emit loadStarted();

    foreach (QLabel *page, _pages)
    {
        if (page)
        {
            _widget->layout()->removeWidget(page);
            page->deleteLater();
        }
    }
    _pages.clear();

    foreach (Poppler::Page *page, _pdfPages)
    {
        delete page;
    }
    _pdfPages.clear();
    _pagesCache.clear();

    _searchText = "";
    _currentSearchPage = 0;
    _currentSearchResult = -1;
    _searchResults.clear();

    if (_pdfDocument)
    {
        delete _pdfDocument;
        _pdfDocument = NULL;
    }

    _pdfData.clear();

    _http.setHost(url.host());
    _requesId = _http.get(url.toString(), &_ioBuffer);

    return true;
}

void IqPdfHelpView::onRequestFinished(int id, bool error)
{
    if (id == _requesId  && !error)
    {
        _ioBuffer.close();
        qDebug() << tr("PDF data gets.");

        _pdfDocument = Poppler::Document::loadFromData(_pdfData);
        if (!_pdfDocument || _pdfDocument->isLocked())
        {
            qDebug() << tr("Error on parse PDF.");
            delete _pdfDocument;
            _pdfDocument = NULL;
            return;
        }

        _pdfDocument->setRenderHint(Poppler::Document::TextAntialiasing);

        for (int i = 0; i < _pdfDocument->numPages(); i++)
        {
            QLabel *label = new QLabel();
            label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
            _pages << label;
            _widget->layout()->addWidget(label);

            // Access page of the PDF file
            Poppler::Page* pdfPage = _pdfDocument->page(i);  // Document starts at page 0
            if (pdfPage == 0)
            {
                qDebug() << tr("Error on parse PDF.");
                delete _pdfDocument;
                _pdfDocument = NULL;
                return;
            }

            _pdfPages << pdfPage;
        }

        paintPdf();

        emit loadFinished(true);
    }
}

void IqPdfHelpView::paintPdf()
{
    if (_pdfDocument)
    {
        if (_pages.count() != _pdfPages.count())
            return;

        _pagesCache.clear();

        for (int i = 0; i < _pdfDocument->numPages(); i++)
        {
            qreal progress = (qreal)i/(_pdfDocument->numPages() - 1) * 100;
            emit loadProgress(progress);
            // Generate a QImage of the rendered page
            QImage image = _pdfPages[i]->renderToImage(72*_zoomFactor, 72*_zoomFactor);
            if (image.isNull())
            {
                qDebug() << tr("Error on parse PDF.");
                return;
            }

            QPixmap pixmap = QPixmap::fromImage(image);
            _pagesCache << pixmap;
            _pages[i]->setPixmap(pixmap);
            int width = image.width();
            if (width < _scrollArea->viewport()->width())
                width = _scrollArea->viewport()->width();
            _pages[i]->setMinimumSize(width, image.height());
            _pages[i]->setMaximumSize(_pages[i]->minimumSize());
            _pages[i]->resize(_pages[i]->minimumSize());
        }

        if (_pages.count() > 0)
        {
            _widget->setMinimumSize(_pages.at(0)->width(), _pages.count() * _pages.at(0)->height() + (_pages.count() - 1) * _widget->layout()->spacing());
            _widget->setMaximumSize(_widget->minimumSize());
            _widget->resize(_widget->minimumSize());
        }

        paintFindResult();
    }
}

void IqPdfHelpView::paintFindResult()
{
    if (_currentSearchResult < 0)
    {
        for (int i = 0; i < _pages.count(); i++)
        {
            QLabel *page = _pages.at(i);
            page->setPixmap(_pagesCache.at(i));
        }
        return;
    }

    for (int i = 0; i < _pages.count(); i++)
    {
        QPixmap pixmap = _pagesCache.at(i);
        QPainter painter (&pixmap);
        QColor brushColor;
        brushColor.setRgb(255, 255, 0, 100);
        painter.setBrush(QBrush(brushColor));
        for (int j = 0; j < _searchResults.at(i).count(); j++)
        {
            if (i == _currentSearchPage && j == _currentSearchResult)
                continue;
            QRectF searchResult = _searchResults.at(i).at(j);
            QRectF zoomRectF (searchResult.x() * _zoomFactor, searchResult.y() * _zoomFactor - 2 *_zoomFactor,
                              searchResult.width() * _zoomFactor, searchResult.height() * _zoomFactor);
            painter.drawRect(zoomRectF);
        }
        _pages[i]->setPixmap(pixmap);
    }

    QRectF searchResult = _searchResults.at(_currentSearchPage).at(_currentSearchResult);
    QPixmap pixmap = *_pages.at(_currentSearchPage)->pixmap();
    QPainter painter (&pixmap);
    QColor brushColor;
    brushColor.setRgb(255, 0, 255, 100);
    painter.setBrush(QBrush(brushColor));
    QRectF zoomRectF (searchResult.x() * _zoomFactor, searchResult.y() * _zoomFactor - 2 *_zoomFactor,
                      searchResult.width() * _zoomFactor, searchResult.height() * _zoomFactor);
    painter.drawRect(zoomRectF);
    _pages[_currentSearchPage]->setPixmap(pixmap);
}

bool IqPdfHelpView::canShowPage(const QUrl &url) const
{
    if (url.toString().right(4) == ".pdf")
        return true;

    return false;
}

void IqPdfHelpView::setZoomFactor(const qreal factor)
{
    if (_zoomFactor != factor)
    {
        _zoomFactor = factor;
        paintPdf();
    }
}

void IqPdfHelpView::reload()
{
    paintPdf();
}

QString IqPdfHelpView::title() const
{
    return _url.path();
}

void IqPdfHelpView::findText(const QString &text)
{
    if (text.isEmpty())
    {
        _searchText = text;
        _currentSearchPage = 0;
        _currentSearchResult = -1;
        _searchResults.clear();
        paintFindResult();
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (_searchText != text)
    {
        _searchText = text;
        _currentSearchPage = 0;
        _currentSearchResult = -1;
        _searchResults.clear();

        for (int i = 0; i < _pdfPages.count(); i++)
        {
            qreal rectLeft = 0;
            qreal rectTop = 0;
            qreal rectRight = 0;
            qreal rectBottom = 0;

            Poppler::Page *page = _pdfPages[i];

            QList<QRectF> pageResult;
            while (page->search(text, rectLeft, rectTop, rectRight, rectBottom, Poppler::Page::NextResult, Poppler::Page::CaseInsensitive))
            {
                QRectF searcResult (rectLeft, rectTop, rectRight - rectLeft, rectBottom - rectTop);
                pageResult << searcResult;
            }

            _searchResults << pageResult;
        }
    }

    bool loop = false;
    bool found = false;
    int searchPage = _currentSearchPage;
    int searchResult = _currentSearchResult;

    while ((searchPage >= _currentSearchPage && !loop) || (searchPage <= _currentSearchPage && loop))
    {
        if (_searchResults.at(searchPage).isEmpty() ||
                searchResult >= _searchResults.at(searchPage).count() - 1)
        {
            searchResult = -1;
            if (searchPage < _pdfPages.count() - 1)
            {
                searchPage++;
            }
            else
            {
                loop = true;
                searchPage = 0;
            }
            continue;
        }

        searchResult++;
        found = true;
        break;
    }

    if (found)
    {
        _currentSearchPage = searchPage;
        _currentSearchResult = searchResult;

        QRectF searchResult = _searchResults.at(_currentSearchPage).at(_currentSearchResult);
        if (_pages.at(0)->width() > _scrollArea->viewport()->width())
            _scrollArea->horizontalScrollBar()->setValue(searchResult.x() * _zoomFactor
                                                         - _scrollArea->viewport()->width() / 2
                                                         + searchResult.width() * _zoomFactor / 2);
        if (_pages.at(0)->height() > _scrollArea->viewport()->height())
            _scrollArea->verticalScrollBar()->setValue(_pages.at(0)->height() * _currentSearchPage
                                                       + 20 * (_currentSearchPage - 1)
                                                       + searchResult.y() * _zoomFactor
                                                       - _scrollArea->viewport()->height() / 2
                                                       + searchResult.height() * _zoomFactor / 2);
    }
    else
    {
        _currentSearchPage = 0;
        _currentSearchResult = -1;
    }

    paintFindResult();

    QApplication::restoreOverrideCursor();
}
