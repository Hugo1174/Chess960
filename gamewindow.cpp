#include "gamewindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QFrame>
#include <QGridLayout>
#include <QWidget>

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
    boardContainer->setFixedSize(720, 720); // Фиксированный размер доски
    QGridLayout *boardLayout = new QGridLayout(boardContainer);
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(0, 0, 0, 0);

    // Создание клеток доски
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QLabel *cell = new QLabel();
            cell->setFixedSize(90, 90);
            cell->setAlignment(Qt::AlignCenter);

            // Окрашивание клеток в шахматном порядке
            if ((row + col) % 2 == 0) {
                cell->setStyleSheet("background-color: #f0d9b5; border: 0px;"); // светлая клетка
            } else {
                cell->setStyleSheet("background-color: #b58863; border: 0px;"); // темная клетка
            }

            boardLayout->addWidget(cell, row, col);
        }
    }

    // Обертка доски с нумерацией
    QWidget *wrapper = new QWidget();
    QGridLayout *wrapperLayout = new QGridLayout(wrapper);
    wrapperLayout->setContentsMargins(0, 0, 0, 0);
    wrapperLayout->setSpacing(0);

    // Добавляем доску в центр
    wrapperLayout->addWidget(boardContainer, 1, 1);

    // Горизонтальная нумерация A-H сверху
    QHBoxLayout *topLabels = new QHBoxLayout();
    topLabels->setSpacing(0);
    topLabels->setContentsMargins(0, 0, 0, 0);
    QString letters = "ABCDEFGH";
    for (int i = 0; i < 8; ++i) {
        QLabel *label = new QLabel(QString(letters[i]));
        label->setFixedSize(90, 20);
        label->setAlignment(Qt::AlignCenter);
        topLabels->addWidget(label);
    }
    wrapperLayout->addLayout(topLabels, 0, 1);

    // Горизонтальная нумерация A-H снизу
    QHBoxLayout *bottomLabels = new QHBoxLayout();
    bottomLabels->setSpacing(0);
    bottomLabels->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < 8; ++i) {
        QLabel *label = new QLabel(QString(letters[i]));
        label->setFixedSize(90, 20);
        label->setAlignment(Qt::AlignCenter);
        bottomLabels->addWidget(label);
    }
    wrapperLayout->addLayout(bottomLabels, 2, 1);

    // Вертикальная нумерация 8-1 слева
    QVBoxLayout *leftLabels = new QVBoxLayout();
    leftLabels->setSpacing(0);
    leftLabels->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < 8; ++i) {
        QLabel *label = new QLabel(QString::number(8 - i));
        label->setFixedSize(20, 90);  // Уменьшена ширина для приближения к доске
        label->setAlignment(Qt::AlignCenter);
        leftLabels->addWidget(label);
    }
    wrapperLayout->addLayout(leftLabels, 1, 0);

    // Вертикальная нумерация 8-1 справа
    QVBoxLayout *rightLabels = new QVBoxLayout();
    rightLabels->setSpacing(0);
    rightLabels->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < 8; ++i) {
        QLabel *label = new QLabel(QString::number(8 - i));
        label->setFixedSize(20, 90);  // Уменьшена ширина для приближения к доске
        label->setAlignment(Qt::AlignCenter);
        rightLabels->addWidget(label);
    }
    wrapperLayout->addLayout(rightLabels, 1, 2);

    // Добавляем обертку с доской и нумерацией в основной макет
    mainLayout->addWidget(wrapper);

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
