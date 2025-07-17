#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "guidewindow.h"
#include "gamewindow.h"
#include "networksetupdialog.h"
#include <QMessageBox>
#include <QTcpSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), game_w(nullptr) // Инициализируем здесь
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

        // Подключаемся к новому, явному сигналу
        connect(game_w, &gamewindow::menuRequested, this, &MainWindow::handleReturnToMenu);

        game_w->showMaximized();
    }
}

void MainWindow::handleReturnToMenu()
{
    if (game_w) {
        // game_w будет удален автоматически благодаря WA_DeleteOnClose
        game_w = nullptr;
    }
    this->show();
}

void MainWindow::on_pushButton_play2_clicked() { QMessageBox::about(this, "ИГРА ПРОТИВ БОТА", "Игра против бота находится в разработке"); }


void MainWindow::on_pushButton_play3_clicked()
{
    // Создаем и показываем наше новое диалоговое окно
    NetworkSetupDialog dialog(this);

    // .exec() блокирует выполнение, пока диалог не будет закрыт
    if (dialog.exec() == QDialog::Accepted) {
        // Если диалог закрыт успешно (соединение установлено),
        // мы получаем из него необходимые данные для старта игры.
        QTcpSocket* socket = dialog.getSocket();
        QString initialLayout = dialog.getInitialBoardLayout();
        PieceColor color = dialog.getPlayerColor();

        //
        // --- СЛЕДУЮЩИЙ ШАГ ---
        // Здесь мы будем создавать NetworkManager и передавать ему сокет,
        // а затем создавать gamewindow в специальном "сетевом" режиме.
        // Пока просто выведем сообщение об успехе.
        //
        QMessageBox::information(this, "Успех!", "Соединение установлено. \nВы играете за: "
                                                     + QString(color == WHITE ? "Белых" : "Черных")
                                                     + "\n\nИнтеграция с игровым окном - следующий шаг.");

        // Важно! На данном этапе сокет не обрабатывается и будет утечкой памяти.
        // На следующем шаге мы передадим его в NetworkManager, который возьмет на себя управление.
        socket->disconnectFromHost(); // Временно просто разрываем соединение

    } else {
        // Пользователь закрыл диалог или произошла ошибка
        QMessageBox::information(this, "Отмена", "Настройка сетевой игры была отменена.");
    }
}

void MainWindow::on_pushButton_guide_clicked() { if (guide_w == nullptr) { guide_w = new guidewindow(this); connect(guide_w, &QWidget::destroyed, this, [this]() { guide_w = nullptr; }); guide_w->show(); } else { guide_w->raise(); guide_w->activateWindow(); } }

void MainWindow::on_pushButton_exit_clicked() { qApp->quit(); }
