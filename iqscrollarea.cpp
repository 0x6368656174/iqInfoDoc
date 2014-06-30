#include "iqscrollarea.h"

IqScrollArea::IqScrollArea(QWidget *parent) :
    QScrollArea(parent)
{
}

void IqScrollArea::resizeEvent(QResizeEvent *event)
{
    emit resized();
    QScrollArea::resizeEvent(event);
}
