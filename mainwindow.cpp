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

// Запускает локальную игру (на одном компьютере).
void MainWindow::on_pushButton_play1_clicked()
{
    if (!game_w) {
        hide();
        game_w = new gamewindow();
        // Соединяем сигнал от игрового окна с нашим слотом для возврата в меню.
        connect(game_w, &gamewindow::menuRequested, this, &MainWindow::handleReturnToMenu);
        game_w->showMaximized();
    }
}

// Слот, который вызывается, когда игровое окно закрывается.
void MainWindow::handleReturnToMenu()
{
    if (game_w) {
        // Окно gamewindow уничтожается само (из-за флага WA_DeleteOnClose),
        // нам нужно только сбросить указатель.
        game_w = nullptr;
    }
    this->show();
}

// Заглушка для игры с ботом.
void MainWindow::on_pushButton_play2_clicked() { QMessageBox::about(this, "ИГРА ПРОТИВ БОТА", "Игра против бота находится в разработке"); }

// Запускает процесс настройки и старта сетевой игры.
void MainWindow::on_pushButton_play3_clicked()
{
    NetworkSetupDialog dialog(this);
    // Показываем диалог настройки и ждем, пока пользователь его не закроет.
    if (dialog.exec() == QDialog::Accepted) {
        // Если настройка прошла успешно, получаем данные из диалога.
        QTcpSocket* socket = dialog.getSocket();
        QString initialLayout = dialog.getInitialBoardLayout();
        PieceColor playerColor = dialog.getPlayerColor();

        if (!socket) return;

        // Создаем NetworkManager с уже установленным сокетом.
        NetworkManager *netManager = new NetworkManager(socket);

        // Создаем игровое окно в сетевом режиме.
        hide();
        game_w = new gamewindow(netManager, initialLayout, playerColor);

        connect(game_w, &gamewindow::menuRequested, this, &MainWindow::handleReturnToMenu);
        game_w->showMaximized();

    } else {
        QMessageBox::information(this, "Отмена", "Настройка сетевой игры была отменена.");
    }
}

// Открывает окно с гайдом.
void MainWindow::on_pushButton_guide_clicked() {
    if (guide_w == nullptr) { // Если окна нет, создаем.
        guide_w = new guidewindow(this);
        // Умный указатель: сбрасываем guide_w в nullptr, когда окно будет уничтожено.
        connect(guide_w, &QWidget::destroyed, this, [this]() { guide_w = nullptr; });
        guide_w->show();
    } else { // Если окно уже есть (свернуто), просто показываем его.
        guide_w->raise();
        guide_w->activateWindow();
    }
}

// Выход из приложения.
void MainWindow::on_pushButton_exit_clicked() { qApp->quit(); }
