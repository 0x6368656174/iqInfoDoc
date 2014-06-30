#ifndef IQSCROLLAREA_H
#define IQSCROLLAREA_H

#include <QScrollArea>

class IqScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit IqScrollArea(QWidget *parent = 0);

    virtual void resizeEvent(QResizeEvent *event);
signals:
    void resized();

};

#endif // IQSCROLLAREA_H
