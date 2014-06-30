#include "iqcontentwidget.h"
#include <QTextStream>
#include <QDebug>
#include <QUrl>


IqContentWidget::IqContentWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    _webPageDocumentIcon = QIcon("://icons/document-globe.png");
    _pdfDocumentIcon = QIcon("://icons/document-pdf-text.png");
    _folderIcon = QIcon("://icons/folder.png");
    _openFolderIcon = QIcon("://icons/folder-open-document.png");
    connect(&_http, SIGNAL(requestFinished(int,bool)), this, SLOT(parseBuffer(int,bool)));
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(changeFolderIcon(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(changeFolderIcon(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(emitLinkClicked(QTreeWidgetItem*,int)));
}

void IqContentWidget::loadContent(const QString &url)
{
    clear();

    _buffer.open(QIODevice::ReadWrite);

    QUrl validUrl (url);
    if(!validUrl.isValid())
        return;

    _http.setHost(validUrl.host());
    _requestId = _http.get(validUrl.path(), &_buffer);
    _http.closeConnection();
}

void IqContentWidget::emitLinkClicked(QTreeWidgetItem *item, const int column)
{
    QUrl url (item->data(0, Qt::UserRole).toString());
    if (url.isValid())
        emit linkClicked(url);
}

void IqContentWidget::parseBuffer(const int id, const bool error)
{
    if (error)
        return;
    if (id != _requestId)
        return;

    _buffer.close();
    _buffer.open(QIODevice::ReadOnly);

    qDebug() << "Content reserved...\n" << _buffer.buffer();

    _xmlReader.setDevice(&_buffer);

    while (_xmlReader.readNextStartElement())
    {
        if (_xmlReader.name().toString() == "content")
        {

        }
        else if (_xmlReader.name().toString() == "folder")
        {
            readFolder(NULL);
        }
        else if (_xmlReader.name().toString() == "page")
        {
            readPage(NULL);
        }
        else
        {
            _xmlReader.skipCurrentElement();
        }
    }
}

void IqContentWidget::readPage(QTreeWidgetItem *item)
{
    if (!_xmlReader.isStartElement() || _xmlReader.name().toString() != "page")
        return;

    QTreeWidgetItem *page = createChildItem(item);
    page->setText(0, QObject::tr("Unknown title"));

    QString href = _xmlReader.attributes().value("href").toString().trimmed();
    page->setData(0, Qt::UserRole, href);
    page->setToolTip(0, href);
    page->setStatusTip(0, href);

    if (href.right(4) == ".pdf")
    {
        page->setIcon(0, _pdfDocumentIcon);
    }
    else
    {
        page->setIcon(0, _webPageDocumentIcon);
    }

    page->setText(0, _xmlReader.readElementText());
}

void IqContentWidget::readFolder(QTreeWidgetItem *item)
{
    if (!_xmlReader.isStartElement() || _xmlReader.name().toString() != "folder")
        return;

    QTreeWidgetItem *folder = createChildItem(item);
    bool folded = (_xmlReader.attributes().value("folded") == "yes");
    folder->setExpanded(folded);

    folder->setText(0, _xmlReader.attributes().value("title").toString().trimmed());
    if(folded)
    {
        folder->setIcon(0, _openFolderIcon);
    }
    else
    {
        folder->setIcon(0, _folderIcon);
    }

    while (_xmlReader.readNextStartElement()) {
        if (_xmlReader.name() == "folder")
        {
            readFolder(folder);
        }
        else if (_xmlReader.name() == "page")
        {
            readPage(folder);
        }
        else
            _xmlReader.skipCurrentElement();
    }
}

QTreeWidgetItem * IqContentWidget::createChildItem(QTreeWidgetItem *item)
{
    QTreeWidgetItem *childItem;
    if (item) {
        childItem = new QTreeWidgetItem(item);
    } else {
        childItem = new QTreeWidgetItem(this);
    }

    childItem->setData(0, Qt::UserRole + 1, _xmlReader.name().toString());
    return childItem;
}

void IqContentWidget::changeFolderIcon(QTreeWidgetItem *item)
{
    if (item->data(0, Qt::UserRole + 1).toString() != "folder")
        return;

    if (item->isExpanded())
    {
        item->setIcon(0, _openFolderIcon);
    }
    else
    {
        item->setIcon(0, _folderIcon);
    }
}
