#include "iqcontentdockwidget.h"
#include "ui_iqcontentdockwidget.h"

IqContentDockWidget::IqContentDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::IqContentDockWidget)
{
    ui->setupUi(this);
}

IqContentDockWidget::~IqContentDockWidget()
{
    delete ui;
}
