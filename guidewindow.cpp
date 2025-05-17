#include "guidewindow.h"
#include "ui_guidewindow.h"

guidewindow::guidewindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::guidewindow)
{
    ui->setupUi(this);
}

guidewindow::~guidewindow()
{
    delete ui;
}
