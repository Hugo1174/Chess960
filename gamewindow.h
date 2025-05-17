#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>

namespace Ui {
class gamewindow;
}

class gamewindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit gamewindow(QWidget *parent = nullptr);
    ~gamewindow();

private:
    Ui::gamewindow *ui;
};

#endif // GAMEWINDOW_H
