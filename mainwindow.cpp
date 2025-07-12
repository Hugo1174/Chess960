// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "guidewindow.h"
#include "gamewindow.h"
#include "networkdialog.h"
#include <QMessageBox>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), game_w(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("Chess960");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_play1_clicked()
{
    // Запуск локальной игры
    if (!game_w) {
        hide();
        game_w = new gamewindow(gamewindow::LOCAL); // Указываем режим
        connect(game_w, &gamewindow::menuRequested, this, &MainWindow::handleReturnToMenu);
        game_w->showMaximized();
    }
}

void MainWindow::on_pushButton_play3_clicked()
{
    // Запуск сетевой игры
    if (game_w) return; // Не запускать, если игра уже идет

    NetworkDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        gamewindow::GameMode mode = dialog.getSelectedMode();
        QString ip = dialog.getIpAddress();

        // Проверка на пустой IP для клиента
        if (mode == gamewindow::ONLINE_CLIENT && ip.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Для подключения к игре необходимо ввести IP-адрес хоста.");
            return;
        }

        hide();
        game_w = new gamewindow(mode, ip);
        connect(game_w, &gamewindow::menuRequested, this, &MainWindow::handleReturnToMenu);
        game_w->showMaximized();
    }
}

void MainWindow::handleReturnToMenu()
{
    if (game_w) {
        // game_w удалится сам благодаря атрибуту WA_DeleteOnClose
        game_w = nullptr;
    }
    this->show();
}

void MainWindow::on_pushButton_play2_clicked() { QMessageBox::about(this, "ИГРА ПРОТИВ БОТА", "Игра против бота находится в разработке"); }
void MainWindow::on_pushButton_guide_clicked() { if (guide_w == nullptr) { guide_w = new guidewindow(this); connect(guide_w, &QWidget::destroyed, this, [this]() { guide_w = nullptr; }); guide_w->show(); } else { guide_w->raise(); guide_w->activateWindow(); } }
void MainWindow::on_pushButton_exit_clicked() { qApp->quit(); }
