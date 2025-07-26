#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QMouseEvent>

/**
 * @class ClickableLabel
 * @brief Расширение QLabel, которое умеет отлавливать клики мыши.
 *
 * Используется для создания интерактивных клеток на шахматной доске.
 * Хранит свои координаты (row, col) и издает сигнал при нажатии.
 */
class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = nullptr);

    // Координаты клетки на доске, устанавливаются извне.
    int row = -1, col = -1;

signals:
    // Сигнал, испускаемый при клике, передает координаты клетки.
    void clicked(int row, int col);

protected:
    // Переопределенный обработчик события нажатия мыши.
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // CLICKABLELABEL_H
