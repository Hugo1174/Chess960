#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "guidewindow.h"
#include "gamewindow.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_5_clicked();

    void on_pushButton_play2_clicked();

    void on_pushButton_play3_clicked();

    void on_pushButton_play1_clicked();

    void on_pushButton_guide_clicked();

private:
    Ui::MainWindow *ui;
    guidewindow *guide_w;
    gamewindow *game_w;

};
#endif // MAINWINDOW_H
