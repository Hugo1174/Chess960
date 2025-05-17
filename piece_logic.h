#ifndef PIECE_LOGIC_H
#define PIECE_LOGIC_H

#include <QString>
#include <QVector>
#include <QPair>

enum PieceType {
    NONE,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum PieceColor {
    NO_COLOR,
    WHITE,
    BLACK
};

struct Piece {
    PieceType type;
    PieceColor color;
};

struct Move {
    int fromRow, fromCol;
    int toRow, toCol;
};

Piece identifyPieceFromPixmapPath(const QString &path);
bool isValidMove(const Piece board[8][8], const Move &move, PieceColor turn);

#endif // PIECE_LOGIC_H
