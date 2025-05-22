#pragma once
#include <QDialog>
#include "piece_logic.h"

class QPushButton;

class PromotionDialog : public QDialog {
    Q_OBJECT

public:
    PromotionDialog(PieceColor color, QWidget *parent = nullptr);
    PieceType selectedType;

private slots:
    void selectQueen();
    void selectRook();
    void selectBishop();
    void selectKnight();
};
