#ifndef IQCONTENTDOCKWIDGET_H
#define IQCONTENTDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class IqContentDockWidget;
}

class IqContentDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit IqContentDockWidget(QWidget *parent = 0);
    ~IqContentDockWidget();

private:
    Ui::IqContentDockWidget *ui;
};

#endif // IQCONTENTDOCKWIDGET_H
