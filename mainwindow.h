#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "guidewindow.h"
#include "gamewindow.h"
#include "networkmanager.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Главное окно приложения, выступает в роли меню.
 *
 * Отвечает за навигацию: запуск разных режимов игры,
 * открытие гайда и выход из приложения.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Слоты для кнопок главного меню.
    void on_pushButton_play2_clicked(); // Игра с ботом (заглушка)
    void on_pushButton_play3_clicked(); // Сетевая игра
    void on_pushButton_play1_clicked(); // Локальная игра
    void on_pushButton_guide_clicked();
    void on_pushButton_exit_clicked();

    // Слот для корректного возврата из игрового окна в меню.
    void handleReturnToMenu();

private:
    Ui::MainWindow *ui;
    // Указатели на дочерние окна.
    guidewindow* guide_w = nullptr;
    gamewindow *game_w = nullptr;
};
#endif // MAINWINDOW_H
