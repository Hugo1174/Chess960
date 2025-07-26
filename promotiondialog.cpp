#include "promotiondialog.h"
#include <QPushButton>
#include <QHBoxLayout>

PromotionDialog::PromotionDialog(PieceColor color, QWidget *parent) : QDialog(parent), selectedType(QUEEN) {
    setWindowTitle("Выбор фигуры");
    setModal(true); // Блокирует остальной интерфейс, пока не будет сделан выбор.

    QHBoxLayout *layout = new QHBoxLayout(this);
    // Определяем, какого цвета иконки нам нужны.
    QString prefix = (color == WHITE) ? ":/new/prefix1/pieces600/w" : ":/new/prefix1/pieces600/b";

    QPushButton *queenBtn = new QPushButton();
    queenBtn->setIcon(QIcon(prefix + "q.png"));
    queenBtn->setIconSize(QSize(64, 64));
    connect(queenBtn, &QPushButton::clicked, this, &PromotionDialog::selectQueen);

    QPushButton *rookBtn = new QPushButton();
    rookBtn->setIcon(QIcon(prefix + "r.png"));
    rookBtn->setIconSize(QSize(64, 64));
    connect(rookBtn, &QPushButton::clicked, this, &PromotionDialog::selectRook);

    QPushButton *bishopBtn = new QPushButton();
    bishopBtn->setIcon(QIcon(prefix + "b.png"));
    bishopBtn->setIconSize(QSize(64, 64));
    connect(bishopBtn, &QPushButton::clicked, this, &PromotionDialog::selectBishop);

    QPushButton *knightBtn = new QPushButton();
    knightBtn->setIcon(QIcon(prefix + "n.png"));
    knightBtn->setIconSize(QSize(64, 64));
    connect(knightBtn, &QPushButton::clicked, this, &PromotionDialog::selectKnight);

    layout->addWidget(queenBtn);
    layout->addWidget(rookBtn);
    layout->addWidget(bishopBtn);
    layout->addWidget(knightBtn);
}

// При выборе любой из фигур, сохраняем тип и закрываем диалог с успехом.
void PromotionDialog::selectQueen()  { selectedType = QUEEN; accept(); }
void PromotionDialog::selectRook()   { selectedType = ROOK; accept(); }
void PromotionDialog::selectBishop() { selectedType = BISHOP; accept(); }
void PromotionDialog::selectKnight() { selectedType = KNIGHT; accept(); }
