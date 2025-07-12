// promotiondialog.h
// Определяет диалог выбора фигуры для превращения пешки.

#pragma once
#include <QDialog>
#include "piece_logic.h"

class QPushButton;

/**
 * @class PromotionDialog
 * @brief Модальный диалог для выбора фигуры при превращении пешки.
 *
 * Создается программно, без .ui файла.
 */
class PromotionDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Конструктор, создающий диалог с иконками нужного цвета.
     * @param color - Цвет фигур для отображения.
     */
    PromotionDialog(PieceColor color, QWidget *parent = nullptr);

    // Публичное поле для получения результата выбора пользователя.
    PieceType selectedType;

private slots:
    // Слоты, связанные с кнопками выбора.
    void selectQueen();
    void selectRook();
    void selectBishop();
    void selectKnight();
};
