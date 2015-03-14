#include "iqcontentwidget.h"
#include <QTextStream>
#include <QDebug>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

IqContentWidget::IqContentWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    m_webPageDocumentIcon = QIcon("://icons/document-globe.png");
    m_pdfDocumentIcon = QIcon("://icons/document-pdf-text.png");
    m_folderIcon = QIcon("://icons/folder.png");
    m_openFolderIcon = QIcon("://icons/folder-open-document.png");

    connect(this, &QTreeWidget::itemExpanded, this, &IqContentWidget::changeFolderIcon);
    connect(this, &QTreeWidget::itemCollapsed, this, &IqContentWidget::changeFolderIcon);
    connect(this, &QTreeWidget::itemClicked, this, &IqContentWidget::emitLinkClicked);
}

void IqContentWidget::loadContent(const QString &url)
{
    clear();

    QUrl validUrl (url);
    if(!validUrl.isValid())
        return;

    QNetworkRequest request;
    request.setUrl(validUrl);
    QNetworkReply *reply = m_networkAccessManager.get(request);
    connect(reply, &QNetworkReply::finished, this, &IqContentWidget::processContentReply);
}

void IqContentWidget::emitLinkClicked(QTreeWidgetItem *item, const int column)
{
    Q_UNUSED(column);
    QUrl url (item->data(0, Qt::UserRole).toString());
    if (url.isValid())
        emit linkClicked(url);
}

void IqContentWidget::readPage(QTreeWidgetItem *item)
{
    if (!m_xmlReader.isStartElement() || m_xmlReader.name().toString() != "page")
        return;

    QTreeWidgetItem *page = createChildItem(item);
    page->setText(0, QObject::tr("Unknown title"));

    QString href = m_xmlReader.attributes().value("href").toString().trimmed();
    page->setData(0, Qt::UserRole, href);
    page->setToolTip(0, href);
    page->setStatusTip(0, href);

    if (href.right(4) == ".pdf") {
        page->setIcon(0, m_pdfDocumentIcon);
    }
    else {
        page->setIcon(0, m_webPageDocumentIcon);
    }

    page->setText(0, m_xmlReader.readElementText());
}

void IqContentWidget::processContentReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    Q_CHECK_PTR(reply);

    QBuffer buffer;

    buffer.setData(reply->readAll());
    reply->deleteLater();

    qDebug() << "Content reserved...\n" << buffer.buffer();

    m_xmlReader.setDevice(&buffer);

    while (m_xmlReader.readNextStartElement()) {
        if (m_xmlReader.name().toString() == "content") {
        }
        else if (m_xmlReader.name().toString() == "folder") {
            readFolder(NULL);
        }
        else if (m_xmlReader.name().toString() == "page") {
            readPage(NULL);
        }
        else {
            m_xmlReader.skipCurrentElement();
        }
    }
}

void IqContentWidget::readFolder(QTreeWidgetItem *item)
{
    if (!m_xmlReader.isStartElement() || m_xmlReader.name().toString() != "folder")
        return;

    QTreeWidgetItem *folder = createChildItem(item);
    bool folded = (m_xmlReader.attributes().value("folded") == "yes");
    folder->setExpanded(folded);

    folder->setText(0, m_xmlReader.attributes().value("title").toString().trimmed());
    if(folded) {
        folder->setIcon(0, m_openFolderIcon);
    }
    else {
        folder->setIcon(0, m_folderIcon);
    }

    while (m_xmlReader.readNextStartElement()) {
        if (m_xmlReader.name() == "folder") {
            readFolder(folder);
        }
        else if (m_xmlReader.name() == "page") {
            readPage(folder);
        }
        else
            m_xmlReader.skipCurrentElement();
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

    childItem->setData(0, Qt::UserRole + 1, m_xmlReader.name().toString());
    return childItem;
}

void IqContentWidget::changeFolderIcon(QTreeWidgetItem *item)
{
    if (item->data(0, Qt::UserRole + 1).toString() != "folder")
        return;

    if (item->isExpanded()) {
        item->setIcon(0, m_openFolderIcon);
    }
    else {
        item->setIcon(0, m_folderIcon);
    }
}
