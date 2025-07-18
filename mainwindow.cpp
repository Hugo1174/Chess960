#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "guidewindow.h"
#include "gamewindow.h"
#include "networksetupdialog.h"
#include <QMessageBox>
#include <QTcpSocket>

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
    if (!game_w) {
        hide();
        game_w = new gamewindow();
        connect(game_w, &gamewindow::menuRequested, this, &MainWindow::handleReturnToMenu);
        game_w->showMaximized();
    }
}

void MainWindow::handleReturnToMenu()
{
    if (game_w) {
        game_w = nullptr;
    }
    this->show();
}

void MainWindow::on_pushButton_play2_clicked() { QMessageBox::about(this, "ИГРА ПРОТИВ БОТА", "Игра против бота находится в разработке"); }



void MainWindow::on_pushButton_play3_clicked()
{
    NetworkSetupDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        // Получаем сокет и данные из диалога
        QTcpSocket* socket = dialog.getSocket();
        QString initialLayout = dialog.getInitialBoardLayout();
        PieceColor playerColor = dialog.getPlayerColor();

        if (!socket) return;

        // 1. Создаем NetworkManager и передаем ему управление сокетом
        NetworkManager *netManager = new NetworkManager(socket); // Родителя не ставим, передадим gamewindow

        // 2. Создаем игровое окно в "сетевом" режиме
        // (Мы добавим этот конструктор в gamewindow на следующем шаге)
        hide();
        game_w = new gamewindow(netManager, initialLayout, playerColor);

        // 3. Подключаем сигнал возврата в меню
        connect(game_w, &gamewindow::menuRequested, this, &MainWindow::handleReturnToMenu);

        game_w->showMaximized();

    } else {
        QMessageBox::information(this, "Отмена", "Настройка сетевой игры была отменена.");
    }
}
// ... остальные методы без изменений ...
void MainWindow::on_pushButton_guide_clicked() { if (guide_w == nullptr) { guide_w = new guidewindow(this); connect(guide_w, &QWidget::destroyed, this, [this]() { guide_w = nullptr; }); guide_w->show(); } else { guide_w->raise(); guide_w->activateWindow(); } }
void MainWindow::on_pushButton_exit_clicked() { qApp->quit(); }
