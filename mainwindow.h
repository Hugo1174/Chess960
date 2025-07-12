#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "guidewindow.h"
#include "gamewindow.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Слоты для кнопок главного меню.
    void on_pushButton_play2_clicked();
    void on_pushButton_play3_clicked();
    void on_pushButton_play1_clicked();
    void on_pushButton_guide_clicked();
    void on_pushButton_exit_clicked();

    // Слот, обрабатывающий сигнал menuRequested от gamewindow.
    void handleReturnToMenu();

private:
    Ui::MainWindow *ui;
    // Указатели для управления дочерними окнами.
    guidewindow* guide_w = nullptr;
    gamewindow *game_w = nullptr;
};
#endif
