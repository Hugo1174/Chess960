#ifndef PROMOTIONDIALOG_H
#define PROMOTIONDIALOG_H

#include <QDialog>
#include "piece_logic.h"

class QPushButton;

/**
 * @class PromotionDialog
 * @brief Модальный диалог для выбора фигуры при превращении пешки.
 *
 * Показывает 4 кнопки с фигурами. После выбора закрывается
 * и отдает выбранный тип фигуры через публичное поле selectedType.
 */
class PromotionDialog : public QDialog {
    Q_OBJECT

public:
    PromotionDialog(PieceColor color, QWidget *parent = nullptr);

    // Здесь хранится результат выбора пользователя.
    PieceType selectedType;

private slots:
    // Слоты для кнопок выбора.
    void selectQueen();
    void selectRook();
    void selectBishop();
    void selectKnight();
};

#endif // PROMOTIONDIALOG_H
