#ifndef PIECE_LOGIC_H
#define PIECE_LOGIC_H

enum PieceType { NONE, KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };
enum PieceColor { NO_COLOR, WHITE, BLACK };

struct Piece {
    PieceType type;
    PieceColor color;
};

struct Move {
    int fromRow, fromCol;
    int toRow, toCol;
};

bool isValidMove(const Piece board[8][8], Move move, PieceColor turn);
bool isSquareAttacked(Piece board[8][8], int targetRow, int targetCol, PieceColor attackerColor);
bool isKingInCheck(Piece board[8][8], PieceColor color);



#endif // PIECE_LOGIC_H
