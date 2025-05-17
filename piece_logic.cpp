#include "piece_logic.h"

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
// Вспомогательная функция для проверки пути (нет фигур между from и to)
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


bool isValidMove(const Piece board[8][8], const Move &move, PieceColor turn) {
    Piece fromPiece = board[move.fromRow][move.fromCol];
    Piece toPiece = board[move.toRow][move.toCol];

    if (fromPiece.color != turn)
        return false;

    if (fromPiece.color == toPiece.color)
        return false;

    int dRow = move.toRow - move.fromRow;
    int dCol = move.toCol - move.fromCol;

    switch (fromPiece.type) {
    case KNIGHT:
        return (abs(dRow) == 2 && abs(dCol) == 1) || (abs(dRow) == 1 && abs(dCol) == 2);

    case BISHOP:
        if (abs(dRow) == abs(dCol)) {
            return isPathClear(board, move.fromRow, move.fromCol, move.toRow, move.toCol);
        }
        return false;

    case ROOK:
        if (dRow == 0 || dCol == 0) {
            return isPathClear(board, move.fromRow, move.fromCol, move.toRow, move.toCol);
        }
        return false;

    case QUEEN:
        if (dRow == 0 || dCol == 0 || abs(dRow) == abs(dCol)) {
            return isPathClear(board, move.fromRow, move.fromCol, move.toRow, move.toCol);
        }
        return false;

    case KING:
        return abs(dRow) <= 1 && abs(dCol) <= 1;

    case PAWN:
        // Логика пешки без изменений
        if (fromPiece.color == WHITE) {
            if (dRow == -1 && dCol == 0 && toPiece.type == NONE) return true;
            if (move.fromRow == 6 && dRow == -2 && dCol == 0 && toPiece.type == NONE && board[move.fromRow-1][move.fromCol].type == NONE) return true;
            if (dRow == -1 && abs(dCol) == 1 && toPiece.color == BLACK) return true;
        } else {
            if (dRow == 1 && dCol == 0 && toPiece.type == NONE) return true;
            if (move.fromRow == 1 && dRow == 2 && dCol == 0 && toPiece.type == NONE && board[move.fromRow+1][move.fromCol].type == NONE) return true;
            if (dRow == 1 && abs(dCol) == 1 && toPiece.color == WHITE) return true;
        }
        return false;

    default:
        return false;
    }
}
