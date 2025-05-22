#include "piece_logic.h"
#include <cmath>
#include <QString>
#include <cstring>

Piece identifyPieceFromPixmapPath(const QString &path) {
    Piece piece{NONE, NO_COLOR};

    if (path.contains("w"))
        piece.color = WHITE;
    else if (path.contains("b"))
        piece.color = BLACK;

    if (path.contains("p"))
        piece.type = PAWN;
    else if (path.contains("n"))
        piece.type = KNIGHT;
    else if (path.contains("b"))
        piece.type = BISHOP;
    else if (path.contains("r"))
        piece.type = ROOK;
    else if (path.contains("q"))
        piece.type = QUEEN;
    else if (path.contains("k"))
        piece.type = KING;

    return piece;
}

bool isPathClear(const Piece board[8][8], int fromRow, int fromCol, int toRow, int toCol) {
    int dRow = (toRow > fromRow) ? 1 : (toRow < fromRow) ? -1 : 0;
    int dCol = (toCol > fromCol) ? 1 : (toCol < fromCol) ? -1 : 0;

    int currentRow = fromRow + dRow;
    int currentCol = fromCol + dCol;

    while (currentRow != toRow || currentCol != toCol) {
        if (board[currentRow][currentCol].type != NONE) {
            return false;
        }
        currentRow += dRow;
        currentCol += dCol;
    }
    return true;
}

bool isSquareAttacked(Piece board[8][8], int targetRow, int targetCol, PieceColor attackerColor) {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece attacker = board[r][c];
            if (attacker.color != attackerColor)
                continue;

            Move move = {r, c, targetRow, targetCol};
            if (isValidMove(board, move, attackerColor))
                return true;
        }
    }
    return false;
}

bool isKingInCheck(Piece board[8][8], PieceColor color) {
    int kingRow = -1, kingCol = -1;

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if (board[r][c].type == KING && board[r][c].color == color) {
                kingRow = r;
                kingCol = c;
            }
        }
    }

    if (kingRow == -1) return false;

    return isSquareAttacked(board, kingRow, kingCol, color == WHITE ? BLACK : WHITE);
}

bool isValidMove(const Piece board[8][8], Move move, PieceColor turn) {
    Piece fromPiece = board[move.fromRow][move.fromCol];
    Piece toPiece = board[move.toRow][move.toCol];

    if (fromPiece.color != turn || fromPiece.type == NONE)
        return false;

    if (toPiece.color == turn)
        return false;

    int dRow = move.toRow - move.fromRow;
    int dCol = move.toCol - move.fromCol;

    bool valid = false;


    switch (fromPiece.type) {
    case KNIGHT:
        valid = (abs(dRow) == 2 && abs(dCol) == 1) || (abs(dRow) == 1 && abs(dCol) == 2);
        break;

    case BISHOP:
        valid = abs(dRow) == abs(dCol) && isPathClear(board, move.fromRow, move.fromCol, move.toRow, move.toCol);
        break;

    case ROOK:
        valid = (dRow == 0 || dCol == 0) && isPathClear(board, move.fromRow, move.fromCol, move.toRow, move.toCol);
        break;

    case QUEEN:
        valid = (abs(dRow) == abs(dCol) || dRow == 0 || dCol == 0) &&
                isPathClear(board, move.fromRow, move.fromCol, move.toRow, move.toCol);
        break;

    case KING:
        valid = abs(dRow) <= 1 && abs(dCol) <= 1;
        break;

    case PAWN:
        if (fromPiece.color == WHITE) {
            if (dRow == -1 && dCol == 0 && toPiece.type == NONE)
                valid = true;
            else if (move.fromRow == 6 && dRow == -2 && dCol == 0 &&
                     toPiece.type == NONE && board[move.fromRow - 1][move.fromCol].type == NONE)
                valid = true;
            else if (dRow == -1 && abs(dCol) == 1 && toPiece.color == BLACK)
                valid = true;
        } else {
            if (dRow == 1 && dCol == 0 && toPiece.type == NONE)
                valid = true;
            else if (move.fromRow == 1 && dRow == 2 && dCol == 0 &&
                     toPiece.type == NONE && board[move.fromRow + 1][move.fromCol].type == NONE)
                valid = true;
            else if (dRow == 1 && abs(dCol) == 1 && toPiece.color == WHITE)
                valid = true;
        }
        break;

    default:
        valid = false;
        break;
    }

    if (!valid) return false;

    // Проверка: не подставляется ли король под шах
    Piece tempBoard[8][8];
    memcpy(tempBoard, board, sizeof(Piece) * 64);

    tempBoard[move.toRow][move.toCol] = tempBoard[move.fromRow][move.fromCol];
    tempBoard[move.fromRow][move.fromCol] = {NONE, NO_COLOR};

    if (isKingInCheck(tempBoard, turn)) {
        return false;
    }
    return true;

}
