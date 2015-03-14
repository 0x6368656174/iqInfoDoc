#ifndef IQCONTENTWIDGET_H
#define IQCONTENTWIDGET_H

#include <QTreeWidget>
#include <QXmlStreamReader>
#include <QIcon>
#include <QBuffer>
#include <QNetworkAccessManager>

class IqContentWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit IqContentWidget(QWidget *parent = 0);

    void loadContent(const QString &url);

signals:
    void linkClicked(const QUrl &url);

private:
    void changeFolderIcon(QTreeWidgetItem * item);
    void emitLinkClicked(QTreeWidgetItem * item, const int column);
    void readFolder(QTreeWidgetItem *item);
    void readPage(QTreeWidgetItem *item);
    void processContentReply();
    QTreeWidgetItem *createChildItem(QTreeWidgetItem *item);

private:
    QNetworkAccessManager m_networkAccessManager;
    QXmlStreamReader m_xmlReader;

    QIcon m_webPageDocumentIcon;
    QIcon m_pdfDocumentIcon;
    QIcon m_folderIcon;
    QIcon m_openFolderIcon;
};

#endif // IQCONTENTWIDGET_H
