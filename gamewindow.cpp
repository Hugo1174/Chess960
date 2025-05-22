#include "gamewindow.h"
#include "piece_logic.h"
#include "clickablelabel.h"
#include "promotiondialog.h"

#include<QMessageBox>
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
#include <QPushButton>
#include <QAbstractButton>

// Функция генерации позиции Chess960
QString generateChess960Position() {
    QVector<QChar> pos(8, ' ');

    int bishop1 = QRandomGenerator::global()->bounded(4) * 2;
    int bishop2 = QRandomGenerator::global()->bounded(4) * 2 + 1;
    pos[bishop1] = 'B';
    pos[bishop2] = 'B';

    QVector<int> freePositions;
    for (int i = 0; i < 8; ++i)
        if (pos[i] == ' ') freePositions.append(i);

    QVector<QVector<int>> rkPositions;
    for (int i = 0; i < freePositions.size(); ++i)
        for (int j = i + 1; j < freePositions.size(); ++j)
            for (int k = i + 1; k < j; ++k)
                rkPositions.append({freePositions[i], freePositions[k], freePositions[j]});

    int idx = QRandomGenerator::global()->bounded(rkPositions.size());
    int r1 = rkPositions[idx][0], k = rkPositions[idx][1], r2 = rkPositions[idx][2];

    pos[r1] = 'R'; pos[k] = 'K'; pos[r2] = 'R';

    QVector<int> remaining;
    for (int i = 0; i < 8; ++i)
        if (pos[i] == ' ') remaining.append(i);

    QVector<QChar> leftovers = {'Q', 'N', 'N'};
    std::shuffle(leftovers.begin(), leftovers.end(), *QRandomGenerator::global());

    for (int i = 0; i < remaining.size(); ++i)
        pos[remaining[i]] = leftovers[i];

    return QString(pos.data(), pos.size());
}

gamewindow::gamewindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Chess960 - Игра");
    setMinimumSize(1280, 720);
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    QVBoxLayout *capturedPiecesPanel = new QVBoxLayout();
    capturedPiecesPanel->setSpacing(0);

    topCapturedPieces = new QLabel("Съеденные фигуры (Белые)");
    topCapturedPieces->setFixedHeight(100);
    capturedPiecesPanel->addWidget(topCapturedPieces);
    whiteCapturedLayout = new QHBoxLayout();
    capturedPiecesPanel->addLayout(whiteCapturedLayout);

    QFrame *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setLineWidth(4);
    divider->setStyleSheet("background-color: green;");
    capturedPiecesPanel->addWidget(divider);

    bottomCapturedPieces = new QLabel("Съеденные фигуры (Черные)");
    bottomCapturedPieces->setFixedHeight(100);
    capturedPiecesPanel->addWidget(bottomCapturedPieces);
    blackCapturedLayout = new QHBoxLayout();
    capturedPiecesPanel->addLayout(blackCapturedLayout);

    mainLayout->addLayout(capturedPiecesPanel);

    QWidget *boardContainer = new QWidget();
    boardContainer->setFixedSize(720, 720);
    QGridLayout *boardLayout = new QGridLayout(boardContainer);
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(0, 0, 0, 0);

    QString letters = "abcdefgh";


    // Нумерация по краям
    for (int row = 0; row < 8; ++row) {
        QLabel* rowLabelLeft = new QLabel(QString::number(8 - row));
        rowLabelLeft->setAlignment(Qt::AlignCenter);
        boardLayout->addWidget(rowLabelLeft, row + 1, 0);

        QLabel* rowLabelRight = new QLabel(QString::number(8 - row));
        rowLabelRight->setAlignment(Qt::AlignCenter);
        boardLayout->addWidget(rowLabelRight, row + 1, 9);

        for (int col = 0; col < 8; ++col) {
            ClickableLabel* cell = new ClickableLabel();
            cell->setFixedSize(90, 90);
            cell->setAlignment(Qt::AlignCenter);
            cell->row = row;
            cell->col = col;
            connect(cell, &ClickableLabel::clicked, this, &gamewindow::handleCellClick);
            cell->setStyleSheet((row + col) % 2 == 0 ?
                                    "background-color: #f0d9b5;" : "background-color: #b58863;");
            boardLayout->addWidget(cell, row + 1, col + 1);
            boardCells[row][col] = cell;
        }
    }

    // Буквы сверху и снизу
    for (int col = 0; col < 8; ++col) {
        QLabel* topLabel = new QLabel(QString(letters[col]));
        topLabel->setAlignment(Qt::AlignCenter);
        boardLayout->addWidget(topLabel, 0, col + 1);

        QLabel* bottomLabel = new QLabel(QString(letters[col]));
        bottomLabel->setAlignment(Qt::AlignCenter);
        boardLayout->addWidget(bottomLabel, 9, col + 1);
    }


    mainLayout->addWidget(boardContainer);

    moveHistory = new QTextEdit();
    moveHistory->setPlaceholderText("Ходы...");
    moveHistory->setReadOnly(true);
    moveHistory->setMinimumWidth(200);
    moveHistory->setMaximumWidth(250);
    mainLayout->addWidget(moveHistory);

    setupChess960Position();
}

void gamewindow::setupChess960Position() {
    QString pos = generateChess960Position();

    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            boardState[r][c] = {NONE, NO_COLOR};

    for (int col = 0; col < 8; ++col) {
        QChar ch = pos[col];
        PieceType type;
        switch (ch.unicode()) {
        case 'K': type = KING; break;
        case 'Q': type = QUEEN; break;
        case 'R': type = ROOK; break;
        case 'B': type = BISHOP; break;
        case 'N': type = KNIGHT; break;
        default: type = NONE; break;
        }

        boardState[7][col] = {type, WHITE};
        boardState[0][col] = {type, BLACK};
        boardState[6][col] = {PAWN, WHITE};
        boardState[1][col] = {PAWN, BLACK};
    }

    updateBoardUI();
}

void gamewindow::clearHighlights() {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            QString color = (r + c) % 2 == 0 ? "#f0d9b5" : "#b58863";
            boardCells[r][c]->setStyleSheet(QString("background-color: %1; border: none;").arg(color));
        }
    }
}

void gamewindow::highlightValidMoves(int row, int col) {
    clearHighlights();

    // Подсветка выбранной фигуры
    boardCells[row][col]->setStyleSheet("background-color: #f7f3d3; border: 2px solid yellow;");

    // Получаем все возможные ходы
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if (row == r && col == c) continue;

            Move move = {row, col, r, c};
            if (isValidMove(boardState, move, currentTurn)) {
                Piece target = boardState[r][c];

                if (target.type == NONE) {
                    boardCells[r][c]->setStyleSheet(
                        (r + c) % 2 == 0 ?
                            "background-color: #f0d9b5; border: 2px solid green;" :
                            "background-color: #b58863; border: 2px solid green;"
                        );
                } else if (target.color != currentTurn) {
                    boardCells[r][c]->setStyleSheet(
                        (r + c) % 2 == 0 ?
                            "background-color: #f0d9b5; border: 2px solid yellow;" :
                            "background-color: #b58863; border: 2px solid yellow;"
                        );
                }
            }
        }
    }
}

bool gamewindow::hasLegalMove(PieceColor color) {
    for (int r1 = 0; r1 < 8; ++r1) {
        for (int c1 = 0; c1 < 8; ++c1) {
            if (boardState[r1][c1].color != color)
                continue;

            for (int r2 = 0; r2 < 8; ++r2) {
                for (int c2 = 0; c2 < 8; ++c2) {
                    Move move = {r1, c1, r2, c2};
                    if (isValidMove(boardState, move, color)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}


void gamewindow::handleCellClick(int row, int col) {
    Piece clickedPiece = boardState[row][col];


    if (selectedRow == -1 && selectedCol == -1) {
        if (clickedPiece.color == currentTurn) {
            selectedRow = row;
            selectedCol = col;
            highlightValidMoves(row, col);
        }
        return;
    }


    if (clickedPiece.color == currentTurn) {
        selectedRow = row;
        selectedCol = col;
        highlightValidMoves(row, col);
        return;
    }

    Move move = {selectedRow, selectedCol, row, col};
    if (isValidMove(boardState, move, currentTurn)) {
        // Сохраняем информацию о фигурах до выполнения хода
        Piece movingPiece = boardState[selectedRow][selectedCol];
        Piece targetPiece = boardState[row][col];

        // ---- ЗАПИСЬ ХОДА ----
        QString pieceName;
        switch (movingPiece.type) {
        case KING: pieceName = "king"; break;
        case QUEEN: pieceName = "queen"; break;
        case ROOK: pieceName = "rook"; break;
        case BISHOP: pieceName = "bishop"; break;
        case KNIGHT: pieceName = "knight"; break;
        case PAWN: pieceName = "pawn"; break;
        default: pieceName = "unknown"; break;
        }

        QString from = QString(QChar('a' + selectedCol)) + QString::number(8 - selectedRow);
        QString to = QString(QChar('a' + col)) + QString::number(8 - row);
        QString colorStr = (movingPiece.color == WHITE) ? "white" : "black";

        QString notation = from + "-" + to + " (" + colorStr + " " + pieceName + ")";
        moveHistory->append(notation);

        // ---- СЪЕДЕННАЯ ФИГУРА ----
        if (targetPiece.type != NONE && targetPiece.color != NO_COLOR) {
            QString imgPath = ":/new/prefix1/pieces600/" +
                              QString(targetPiece.color == WHITE ? "w" : "b") +
                              QString(targetPiece.type == KING ? "k" :
                                          targetPiece.type == QUEEN ? "q" :
                                          targetPiece.type == ROOK ? "r" :
                                          targetPiece.type == BISHOP ? "b" :
                                          targetPiece.type == KNIGHT ? "n" :
                                          targetPiece.type == PAWN ? "p" : "") +
                              ".png";

            QLabel *capturedLabel = new QLabel();
            capturedLabel->setPixmap(QPixmap(imgPath).scaled(30, 30));

            if (targetPiece.color == WHITE) {
                whiteCapturedLayout->addWidget(capturedLabel);
            } else {
                blackCapturedLayout->addWidget(capturedLabel);
            }
        }

        // ---- ВЫПОЛНЕНИЕ ХОДА ----
        boardState[row][col] = movingPiece;
        boardState[selectedRow][selectedCol] = {NONE, NO_COLOR};

        selectedRow = -1;
        selectedCol = -1;
        clearHighlights();

        // ---- ПРОВЕРКА НА МАТ / ПАТ ----
        PieceColor opponent = (currentTurn == WHITE) ? BLACK : WHITE;

        if (isKingInCheck(boardState, opponent)) {
            if (!hasLegalMove(opponent)) {
                QMessageBox::information(this, "Мат", (opponent == WHITE ? "Белым" : "Чёрным") + QString(" мат!"));
                return;
            }
        } else {
            if (!hasLegalMove(opponent)) {
                QMessageBox msgBox;
                msgBox.setWindowTitle("Игра окончена");
                msgBox.setText("Пат! Ничья.");
                QPushButton *menuButton = msgBox.addButton("В меню", QMessageBox::AcceptRole);
                msgBox.exec();

                if (msgBox.clickedButton() == menuButton) {
                    this->close();
                    emit returnToMenu();
                }
            }
        }

        // ---- ПРОМОЦИЯ ПЕШКИ ----
        if (boardState[row][col].type == PAWN &&
            ((boardState[row][col].color == WHITE && row == 0) ||
             (boardState[row][col].color == BLACK && row == 7))) {

            PromotionDialog dialog(boardState[row][col].color, this);
            if (dialog.exec() == QDialog::Accepted) {
                boardState[row][col].type = dialog.selectedType;
            }
        }

        switchTurn();
        updateBoardUI();
    }


    if (boardState[row][col].type == PAWN &&
        ((boardState[row][col].color == WHITE && row == 0) ||
         (boardState[row][col].color == BLACK && row == 7))) {

        PromotionDialog dialog(boardState[row][col].color, this);
        if (dialog.exec() == QDialog::Accepted) {
            boardState[row][col].type = dialog.selectedType;
        }
    }

    updateBoardUI();  // Обновляем доску перед проверками

}

void gamewindow::switchTurn() {
    currentTurn = (currentTurn == WHITE) ? BLACK : WHITE;
}

void gamewindow::updateBoardUI() {
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

    for (int r = 0; r < 8; ++r){
        for (int c = 0; c < 8; ++c) {
            Piece p = boardState[r][c];
            if (p.type == NONE) boardCells[r][c]->clear();
            else boardCells[r][c]->setPixmap(QPixmap(pieceImages.value({p.color, p.type})).scaled(90, 90));
        }
    }
    // Подсветка короля под шахом
    if (isKingInCheck(boardState, currentTurn)) {
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                if (boardState[r][c].type == KING && boardState[r][c].color == currentTurn) {
                    boardCells[r][c]->setStyleSheet("border: 3px solid red;");
                    return; // нашли короля, выходим
                }
            }
        }
    }


}


gamewindow::~gamewindow() {}
