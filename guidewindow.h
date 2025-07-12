
#ifndef GUIDEWINDOW_H
#define GUIDEWINDOW_H

#include <QMainWindow>

namespace Ui {
class guidewindow;
}


class guidewindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit guidewindow(QWidget *parent = nullptr);
    ~guidewindow();

private:
    Ui::guidewindow *ui;
};

#endif
