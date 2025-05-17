#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "guidewindow.h"
#include "gamewindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_play2_clicked()
{
    QMessageBox::about(this, "ИГРА ПРОТИВ БОТА", "Игра против бота находится в разработке");
}


void MainWindow::on_pushButton_play3_clicked()
{
    QMessageBox::about(this, "ИГРА ПО СЕТИ", "Игра по сети находится в разработке");
}


void MainWindow::on_pushButton_play1_clicked()
{
    hide();
    game_w = new gamewindow();
    game_w -> showMaximized();

}


void MainWindow::on_pushButton_guide_clicked()
{
    hide();
    guide_w = new guidewindow(this);
    guide_w -> show();
}

