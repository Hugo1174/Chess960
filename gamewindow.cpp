#include "gamewindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QFrame>
#include <QGridLayout>
#include <QWidget>
#include <QPixmap>
#include <QRandomGenerator>
#include <algorithm>

// Функция генерации стартовой позиции Chess960
QString generateChess960Position()
{
    QVector<QChar> pos(8, ' ');

    // 1. Расставляем слонов на клетках разного цвета
    int bishop1 = QRandomGenerator::global()->bounded(4) * 2;     // чётные позиции: 0,2,4,6
    int bishop2 = QRandomGenerator::global()->bounded(4) * 2 + 1; // нечётные позиции: 1,3,5,7
    pos[bishop1] = 'B';
    pos[bishop2] = 'B';

    // 2. Расставляем короля между ладьями
    QVector<int> freePositions;
    for (int i = 0; i < 8; ++i) {
        if (pos[i] == ' ') freePositions.append(i);
    }

    QVector<QVector<int>> rkPositions;
    for (int i = 0; i < freePositions.size(); ++i) {
        for (int j = i+1; j < freePositions.size(); ++j) {
            for (int k = i+1; k < j; ++k) {
                rkPositions.append({freePositions[i], freePositions[k], freePositions[j]});
            }
        }
    }

    int idx = QRandomGenerator::global()->bounded(rkPositions.size());
    int r1 = rkPositions[idx][0];
    int k = rkPositions[idx][1];
    int r2 = rkPositions[idx][2];

    pos[r1] = 'R';
    pos[k] = 'K';
    pos[r2] = 'R';

    // 3. Заполняем оставшиеся 3 клетки ферзём и конями
    QVector<int> remainingPositions;
    for (int i = 0; i < 8; ++i) {
        if (pos[i] == ' ') remainingPositions.append(i);
    }

    QVector<QChar> leftovers = {'Q', 'N', 'N'};
    std::shuffle(leftovers.begin(), leftovers.end(), *QRandomGenerator::global());

    for (int i = 0; i < remainingPositions.size(); ++i) {
        pos[remainingPositions[i]] = leftovers[i];
    }

    return QString(pos.data(), pos.size());
}

void gamewindow::setupChess960Position()
{
    QString position = generateChess960Position();

    // Пути к ресурсам фигур (проверьте соответствие с вашим .qrc)
    QMap<QChar, QString> whitePieces {
        {'K', ":/new/prefix1/pieces600/wk.png"},
        {'Q', ":/new/prefix1/pieces600/wq.png"},
        {'R', ":/new/prefix1/pieces600/wr.png"},
        {'B', ":/new/prefix1/pieces600/wb.png"},
        {'N', ":/new/prefix1/pieces600/wn.png"}
    };
    QMap<QChar, QString> blackPieces {
        {'K', ":/new/prefix1/pieces600/bk.png"},
        {'Q', ":/new/prefix1/pieces600/bq.png"},
        {'R', ":/new/prefix1/pieces600/br.png"},
        {'B', ":/new/prefix1/pieces600/bb.png"},
        {'N', ":/new/prefix1/pieces600/bn.png"}
    };

    // Очищаем доску
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            boardCells[row][col]->clear();
        }
    }

    // Расставляем белые фигуры на 1-й горизонтали (индекс 7)
    int whiteRow = 7;
    for (int col = 0; col < 8; ++col) {
        QChar piece = position[col];
        if (whitePieces.contains(piece)) {
            QPixmap pixmap(whitePieces[piece]);
            boardCells[whiteRow][col]->setPixmap(pixmap.scaled(90, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    // Пешки белых на 2-й горизонтали (индекс 6)
    int whitePawnRow = 6;
    QPixmap wpix(":/new/prefix1/pieces600/wp.png");
    for (int col = 0; col < 8; ++col) {
        boardCells[whitePawnRow][col]->setPixmap(wpix.scaled(90, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // Черные фигуры зеркально на 8-й горизонтали (индекс 0)
    int blackRow = 0;
    for (int col = 0; col < 8; ++col) {
        QChar piece = position[col];
        if (blackPieces.contains(piece)) {
            QPixmap pixmap(blackPieces[piece]);
            boardCells[blackRow][col]->setPixmap(pixmap.scaled(90, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    // Пешки черных на 7-й горизонтали (индекс 1)
    int blackPawnRow = 1;
    QPixmap bpix(":/new/prefix1/pieces600/bp.png");
    for (int col = 0; col < 8; ++col) {
        boardCells[blackPawnRow][col]->setPixmap(bpix.scaled(90, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

gamewindow::gamewindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Chess960 - Игровое Окно");
    setMinimumSize(1280, 720);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    QVBoxLayout *capturedPiecesPanel = new QVBoxLayout();
    capturedPiecesPanel->setSpacing(0);

    QLabel *whiteCapturedPieces = new QLabel("Съеденные фигуры (Белые)");
    whiteCapturedPieces->setFixedHeight(100);
    whiteCapturedPieces->setMinimumWidth(200);
    whiteCapturedPieces->setMaximumWidth(250);
    capturedPiecesPanel->addWidget(whiteCapturedPieces);

    QFrame *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setFrameShadow(QFrame::Plain);
    divider->setLineWidth(4);
    divider->setStyleSheet("background-color: green;");
    capturedPiecesPanel->addWidget(divider);

    QLabel *blackCapturedPieces = new QLabel("Съеденные фигуры (Черные)");
    blackCapturedPieces->setFixedHeight(100);
    blackCapturedPieces->setMinimumWidth(200);
    blackCapturedPieces->setMaximumWidth(250);
    capturedPiecesPanel->addWidget(blackCapturedPieces);

    mainLayout->addLayout(capturedPiecesPanel);

    QWidget *boardContainer = new QWidget();
    boardContainer->setFixedSize(720, 720);
    QGridLayout *boardLayout = new QGridLayout(boardContainer);
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(0, 0, 0, 0);

    // Создание клеток доски и сохранение в член класса boardCells
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QLabel *cell = new QLabel();
            cell->setFixedSize(90, 90);
            cell->setAlignment(Qt::AlignCenter);

            if ((row + col) % 2 == 0) {
                cell->setStyleSheet("background-color: #f0d9b5; border: 0px;");
            } else {
                cell->setStyleSheet("background-color: #b58863; border: 0px;");
            }

            boardLayout->addWidget(cell, row, col);
            boardCells[row][col] = cell;
        }
    }

    QWidget *wrapper = new QWidget();
    QGridLayout *wrapperLayout = new QGridLayout(wrapper);
    wrapperLayout->setContentsMargins(0, 0, 0, 0);
    wrapperLayout->setSpacing(0);

    wrapperLayout->addWidget(boardContainer, 1, 1);

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

    QVBoxLayout *leftLabels = new QVBoxLayout();
    leftLabels->setSpacing(0);
    leftLabels->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < 8; ++i) {
        QLabel *label = new QLabel(QString::number(8 - i));
        label->setFixedSize(20, 90);
        label->setAlignment(Qt::AlignCenter);
        leftLabels->addWidget(label);
    }
    wrapperLayout->addLayout(leftLabels, 1, 0);

    QVBoxLayout *rightLabels = new QVBoxLayout();
    rightLabels->setSpacing(0);
    rightLabels->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < 8; ++i) {
        QLabel *label = new QLabel(QString::number(8 - i));
        label->setFixedSize(20, 90);
        label->setAlignment(Qt::AlignCenter);
        rightLabels->addWidget(label);
    }
    wrapperLayout->addLayout(rightLabels, 1, 2);

    mainLayout->addWidget(wrapper);

    moveHistory = new QTextEdit();
    moveHistory->setPlaceholderText("Ходы...");
    moveHistory->setReadOnly(true);
    moveHistory->setMinimumWidth(200);
    moveHistory->setMaximumWidth(250);
    mainLayout->addWidget(moveHistory);

    // Установка фигур Chess960
    setupChess960Position();
}

gamewindow::~gamewindow()
{
}
