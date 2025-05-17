#include "gamewindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QFrame>
#include <QGridLayout>
#include <QPalette>
#include <QWidget>
#include <QBrush>

// Конструктор игрового окна
gamewindow::gamewindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Chess960 - Игровое Окно");
    setMinimumSize(1280, 720);

    // Центральный виджет
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Основной горизонтальный макет
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Левая панель для съеденных фигур
    QVBoxLayout *capturedPiecesPanel = new QVBoxLayout();
    capturedPiecesPanel->setSpacing(0);

    // Съеденные фигуры (Белые)
    QLabel *whiteCapturedPieces = new QLabel("Съеденные фигуры (Белые)");
    whiteCapturedPieces->setFixedHeight(100);
    whiteCapturedPieces->setMinimumWidth(200);
    whiteCapturedPieces->setMaximumWidth(250);
    capturedPiecesPanel->addWidget(whiteCapturedPieces);

    // Разделительная линия
    QFrame *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setFrameShadow(QFrame::Plain);
    divider->setLineWidth(4);
    divider->setStyleSheet("background-color: green;");
    capturedPiecesPanel->addWidget(divider);

    // Съеденные фигуры (Черные)
    QLabel *blackCapturedPieces = new QLabel("Съеденные фигуры (Черные)");
    blackCapturedPieces->setFixedHeight(100);
    blackCapturedPieces->setMinimumWidth(200);
    blackCapturedPieces->setMaximumWidth(250);
    capturedPiecesPanel->addWidget(blackCapturedPieces);

    // Добавляем панель для съеденных фигур в основной макет
    mainLayout->addLayout(capturedPiecesPanel);

    // Контейнер для шахматной доски
    QWidget *boardContainer = new QWidget();
    QGridLayout *boardContainerLayout = new QGridLayout(boardContainer);
    boardContainerLayout->setSpacing(0);
    boardContainerLayout->setContentsMargins(0, 0, 0, 0);

    // Создание клеток доски
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QLabel *cell = new QLabel();
            cell->setMinimumSize(90, 90);
            cell->setMaximumSize(90, 90);
            cell->setAlignment(Qt::AlignCenter);

            // Окрашивание клеток в шахматном порядке
            if ((row + col) % 2 == 0) {
                cell->setStyleSheet("background-color: #f0d9b5; border: 0px;"); // светлая клетка
            } else {
                cell->setStyleSheet("background-color: #b58863; border: 0px;"); // темная клетка
            }

            boardContainerLayout->addWidget(cell, row + 1, col + 1);
        }
    }

    mainLayout->addWidget(boardContainer);

    // Поле для записи ходов (уменьшенное)
    QTextEdit *moveHistory = new QTextEdit();
    moveHistory->setPlaceholderText("Ходы...");
    moveHistory->setReadOnly(true);
    moveHistory->setMinimumWidth(200);
    moveHistory->setMaximumWidth(250);
    mainLayout->addWidget(moveHistory);
}

// Деструктор игрового окна
gamewindow::~gamewindow()
{
}
