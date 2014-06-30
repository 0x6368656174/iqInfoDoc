#ifndef IQCONTENTWIDGET_H
#define IQCONTENTWIDGET_H

#include <QTreeWidget>
#include <QXmlStreamReader>
#include <QIcon>
#include <QBuffer>
#include <QHttp>

class IqContentWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit IqContentWidget(QWidget *parent = 0);

    void loadContent(const QString &url);

signals:
    void linkClicked(const QUrl &url);

private slots:
    void changeFolderIcon(QTreeWidgetItem * item);
    void parseBuffer(const int id, const bool result);
    void emitLinkClicked(QTreeWidgetItem * item, const int column);

private:
    QBuffer _buffer;
    QHttp _http;
    int _requestId;
    QXmlStreamReader _xmlReader;
    QTreeWidgetItem *createChildItem(QTreeWidgetItem *item);
    void readFolder(QTreeWidgetItem *item);
    void readPage(QTreeWidgetItem *item);

    QIcon _webPageDocumentIcon;
    QIcon _pdfDocumentIcon;
    QIcon _folderIcon;
    QIcon _openFolderIcon;
};

#endif // IQCONTENTWIDGET_H
