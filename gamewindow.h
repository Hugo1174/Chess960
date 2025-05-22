#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "clickablelabel.h"
#include "piece_logic.h"

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QGridLayout>
#include <QVBoxLayout>

class gamewindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit gamewindow(QWidget *parent = nullptr);
        ~gamewindow();

    private:
        QWidget *centralWidget;
        QLabel *topCapturedPieces;
        QLabel *bottomCapturedPieces;
        QTextEdit *moveHistory;

        ClickableLabel* boardCells[8][8];
        Piece boardState[8][8];

        int selectedRow = -1, selectedCol = -1;
        PieceColor currentTurn = WHITE;

        void setupChess960Position();
        void handleCellClick(int row, int col);
        void switchTurn();
        void updateBoardUI();
        void highlightValidMoves(int row, int col);
        void clearHighlights();
        bool hasLegalMove(PieceColor color);
        bool isSquareAttacked(Piece board[8][8], int targetRow, int targetCol, PieceColor attackerColor);
        QString currentMoveLine;
        int moveCounter = 1;
        QHBoxLayout *whiteCapturedLayout;
        QHBoxLayout *blackCapturedLayout;





    signals:
        void returnToMenu();
};



#endif // GAMEWINDOW_H
