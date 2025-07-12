

#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent) : QLabel(parent), row(-1), col(-1) {}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{
    // Реагируем только на нажатие левой кнопки мыши.
    if (event->button() == Qt::LeftButton) {
        // Испускаем сигнал, передавая свои координаты.
        emit clicked(row, col);
    }
}
