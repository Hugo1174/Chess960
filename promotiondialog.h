
#pragma once
#include <QDialog>
#include "piece_logic.h"

class QPushButton;


class PromotionDialog : public QDialog {
    Q_OBJECT

public:

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
