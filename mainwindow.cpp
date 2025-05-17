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

    setWindowTitle("Chess960");
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
    if (guide_w == nullptr) {
        guide_w = new guidewindow(this);

        // Подключаем сигнал закрытия окна, чтобы очистить указатель
        connect(guide_w, &QWidget::destroyed, this, [this]() {
            guide_w = nullptr;
        });

        guide_w->show();
    } else {
        guide_w->raise();      // Поднять окно на передний план
        guide_w->activateWindow(); // Активировать окно
    }
}



void MainWindow::on_pushButton_exit_clicked()
{
    qApp->quit();
}

