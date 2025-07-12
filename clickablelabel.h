#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QMouseEvent>


class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = nullptr);

    // Координаты клетки на доске (0-7)
    int row = -1, col = -1;

signals:
    void clicked(int row, int col);

protected:
    // Переопределенный обработчик события для отслеживания кликов мыши.
    void mousePressEvent(QMouseEvent *event) override;
};

#endif
