
// Определяет кастомный виджет QLabel, который может обрабатывать клики мыши.

#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QMouseEvent>

/**
 * @class ClickableLabel
 * @brief QLabel, который испускает сигнал при нажатии.
 *
 * Используется для создания интерактивных клеток шахматной доски.
 * Хранит свои координаты на доске для удобства обработки.
 */
class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = nullptr);

    // Координаты клетки на доске (0-7)
    int row = -1, col = -1;

signals:
    /**
     * @brief Сигнал, испускаемый при клике на метку.
     * @param row - Ряд, на котором находится метка.
     * @param col - Столбец, на котором находится метка.
     */
    void clicked(int row, int col);

protected:
    // Переопределенный обработчик события для отслеживания кликов мыши.
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // CLICKABLELABEL_H
