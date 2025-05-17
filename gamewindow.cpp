#include "gamewindow.h"
#include "piece_logic.h"
#include "clickablelabel.h"

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

gamewindow::gamewindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Chess960 - Игра");
    setMinimumSize(1280, 720);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Панель съеденных фигур
    QVBoxLayout *capturedPiecesPanel = new QVBoxLayout();
    capturedPiecesPanel->setSpacing(0);

    topCapturedPieces = new QLabel("Съеденные фигуры (Белые)");
    topCapturedPieces->setFixedHeight(100);
    topCapturedPieces->setMinimumWidth(200);
    topCapturedPieces->setMaximumWidth(250);
    capturedPiecesPanel->addWidget(topCapturedPieces);

    QFrame *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setFrameShadow(QFrame::Plain);
    divider->setLineWidth(4);
    divider->setStyleSheet("background-color: green;");
    capturedPiecesPanel->addWidget(divider);

    bottomCapturedPieces = new QLabel("Съеденные фигуры (Черные)");
    bottomCapturedPieces->setFixedHeight(100);
    bottomCapturedPieces->setMinimumWidth(200);
    bottomCapturedPieces->setMaximumWidth(250);
    capturedPiecesPanel->addWidget(bottomCapturedPieces);

    mainLayout->addLayout(capturedPiecesPanel);

    // Контейнер для шахматной доски
    QWidget *boardContainer = new QWidget();
    boardContainer->setFixedSize(720, 720);
    QGridLayout *boardLayout = new QGridLayout(boardContainer);
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(0, 0, 0, 0);

    // Создание клеток доски
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            ClickableLabel *cell = new ClickableLabel();
            cell->setFixedSize(90, 90);
            cell->setAlignment(Qt::AlignCenter);
            cell->row = row;
            cell->col = col;
            connect(cell, &ClickableLabel::clicked, this, &gamewindow::handleCellClick);

            // Раскраска клеток
            if ((row + col) % 2 == 0) {
                cell->setStyleSheet("background-color: #f0d9b5; border: 0px;");
            } else {
                cell->setStyleSheet("background-color: #b58863; border: 0px;");
            }

            boardLayout->addWidget(cell, row, col);
            boardCells[row][col] = cell;
        }
    }

    mainLayout->addWidget(boardContainer);

    // Поле для истории ходов
    moveHistory = new QTextEdit();
    moveHistory->setPlaceholderText("Ходы...");
    moveHistory->setReadOnly(true);
    moveHistory->setMinimumWidth(200);
    moveHistory->setMaximumWidth(250);
    mainLayout->addWidget(moveHistory);

    // Устанавливаем начальную позицию
    setupChess960Position();
}

void gamewindow::setupChess960Position() {
    QString position = generateChess960Position();

    // Инициализация пустой доски
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            boardState[r][c] = {NONE, NO_COLOR};
        }
    }

    // Заполнение фигур согласно позиции
    for (int col = 0; col < 8; ++col) {
        QChar figChar = position[col];
        PieceType pt;
        switch (figChar.unicode()) {
        case 'K': pt = KING; break;
        case 'Q': pt = QUEEN; break;
        case 'R': pt = ROOK; break;
        case 'B': pt = BISHOP; break;
        case 'N': pt = KNIGHT; break;
        default: pt = NONE; break;
        }

        boardState[7][col] = {pt, WHITE};
        boardState[0][col] = {pt, BLACK};

        boardState[6][col] = {PAWN, WHITE};
        boardState[1][col] = {PAWN, BLACK};
    }

    updateBoardUI();
}
void gamewindow::clearHighlights() {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if ((r + c) % 2 == 0) {
                boardCells[r][c]->setStyleSheet("background-color: #f0d9b5; border: 0px;");
            } else {
                boardCells[r][c]->setStyleSheet("background-color: #b58863; border: 0px;");
            }
        }
    }
}

void gamewindow::highlightSelectedAndMoves(int row, int col) {
    clearHighlights();

    // Подсветка выбранной клетки (светлее)
    boardCells[row][col]->setStyleSheet("background-color: #f7f3d3; border: 2px solid yellow;");

    // Подсвечиваем возможные ходы зелёным
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Move move{row, col, r, c};
            if (isValidMove(boardState, move, currentTurn)) {
                // Подсвечиваем клетку зеленым, если она не выбранная
                if (!(r == row && c == col)) {
                    boardCells[r][c]->setStyleSheet("background-color: #a6f0a6; border: 2px solid green;");
                }
            }
        }
    }
}

void gamewindow::handleCellClick(int row, int col) {
    Piece clickedPiece = boardState[row][col];

    if (selectedRow == -1 && selectedCol == -1) {
        if (clickedPiece.color == currentTurn) {
            selectedRow = row;
            selectedCol = col;
            highlightSelectedAndMoves(row, col);
        }
        return;
    }

    if (selectedRow != -1 && selectedCol != -1) {
        if (clickedPiece.color == currentTurn) {
            selectedRow = row;
            selectedCol = col;
            highlightSelectedAndMoves(row, col);
            return;
        }

        Move move = {selectedRow, selectedCol, row, col};
        if (isValidMove(boardState, move, currentTurn)) {
            boardState[row][col] = boardState[selectedRow][selectedCol];
            boardState[selectedRow][selectedCol] = {NONE, NO_COLOR};
            switchTurn();
            updateBoardUI();
        }

        selectedRow = -1;
        selectedCol = -1;
        clearHighlights();
    }
}



void gamewindow::switchTurn() {
    currentTurn = (currentTurn == WHITE) ? BLACK : WHITE;
}

void gamewindow::updateBoardUI() {
    // Карта фигур к путям изображений
    QMap<QPair<PieceColor, PieceType>, QString> pieceImages = {
        {{WHITE, KING}, ":/new/prefix1/pieces600/wk.png"},
        {{WHITE, QUEEN}, ":/new/prefix1/pieces600/wq.png"},
        {{WHITE, ROOK}, ":/new/prefix1/pieces600/wr.png"},
        {{WHITE, BISHOP}, ":/new/prefix1/pieces600/wb.png"},
        {{WHITE, KNIGHT}, ":/new/prefix1/pieces600/wn.png"},
        {{WHITE, PAWN}, ":/new/prefix1/pieces600/wp.png"},
        {{BLACK, KING}, ":/new/prefix1/pieces600/bk.png"},
        {{BLACK, QUEEN}, ":/new/prefix1/pieces600/bq.png"},
        {{BLACK, ROOK}, ":/new/prefix1/pieces600/br.png"},
        {{BLACK, BISHOP}, ":/new/prefix1/pieces600/bb.png"},
        {{BLACK, KNIGHT}, ":/new/prefix1/pieces600/bn.png"},
        {{BLACK, PAWN}, ":/new/prefix1/pieces600/bp.png"}
    };

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece p = boardState[row][col];
            if (p.type == NONE) {
                boardCells[row][col]->clear();
            } else {
                QString imgPath = pieceImages.value({p.color, p.type});
                boardCells[row][col]->setPixmap(QPixmap(imgPath).scaled(90, 90));
            }
        }
    }
}

gamewindow::~gamewindow() {}
