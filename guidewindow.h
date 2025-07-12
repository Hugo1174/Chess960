// guidewindow.h
// Окно с правилами и справкой.

#ifndef GUIDEWINDOW_H
#define GUIDEWINDOW_H

#include <QMainWindow>

namespace Ui {
class guidewindow;
}

/**
 * @class guidewindow
 * @brief Простое окно для отображения справочной информации.
 *
 * UI управляется через файл guidewindow.ui.
 */
class guidewindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit guidewindow(QWidget *parent = nullptr);
    ~guidewindow();

private:
    Ui::guidewindow *ui;
};

#endif // GUIDEWINDOW_H
