
#include "promotiondialog.h"
#include <QPushButton>
#include <QHBoxLayout>

PromotionDialog::PromotionDialog(PieceColor color, QWidget *parent)
    : QDialog(parent), selectedType(QUEEN) // Ферзь - выбор по умолчанию.
{
    setWindowTitle("Выбор фигуры");
    setModal(true); // Блокируем основное окно.

    QHBoxLayout *layout = new QHBoxLayout(this);

    // Формируем путь к иконкам в зависимости от цвета.
    QString prefix = (color == WHITE) ? ":/new/prefix1/pieces600/w" : ":/new/prefix1/pieces600/b";

    // Создаем кнопки с иконками фигур.
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

// Слоты устанавливают результат и закрывают диалог с флагом "успешно".
void PromotionDialog::selectQueen()  { selectedType = QUEEN; accept(); }
void PromotionDialog::selectRook()   { selectedType = ROOK; accept(); }
void PromotionDialog::selectBishop() { selectedType = BISHOP; accept(); }
void PromotionDialog::selectKnight() { selectedType = KNIGHT; accept(); }
