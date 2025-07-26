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
 * Содержимое полностью определяется в файле guidewindow.ui.
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
