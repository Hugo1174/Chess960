#include "guidewindow.h"
#include "ui_guidewindow.h"

guidewindow::guidewindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::guidewindow)
{
    ui->setupUi(this);
    setWindowTitle("Chess960 - Гайд");

    setAttribute(Qt::WA_DeleteOnClose);
}

guidewindow::~guidewindow()
{
    delete ui;
}
