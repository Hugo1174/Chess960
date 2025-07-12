#ifndef PIECE_LOGIC_H
#define PIECE_LOGIC_H

#include <QObject>
#include <vector>
#include <utility>
#include <array>

// Тип фигуры
enum PieceType { NONE, KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };
// Цвет фигуры
enum PieceColor { NO_COLOR, WHITE, BLACK };
// Текущий статус игры
enum GameStatus { IN_PROGRESS, CHECKMATE, STALEMATE };

// Структура, описывающая одну фигуру
struct Piece {
    PieceType type = NONE;
    PieceColor color = NO_COLOR;
};

// Структура, описывающая один ход
struct Move {
    int fromRow, fromCol;
    int toRow, toCol;
};


class PieceLogic : public QObject
{
    Q_OBJECT

public:
    explicit PieceLogic(QObject *parent = nullptr);

    // --- Публичный API для управления игрой ---
    void setupNewGame();
    bool tryMove(int fromRow, int fromCol, int toRow, int toCol);
    void promotePawn(int row, int col, PieceType newType);

    // --- Методы для получения состояния игры (для UI) ---
    Piece getPieceAt(int row, int col) const;
    PieceColor getCurrentTurn() const;
    GameStatus getGameStatus() const;
    const std::vector<Piece>& getCapturedPieces(PieceColor color) const;
    std::vector<Move> getValidMovesForPiece(int row, int col);
    bool isKingInCheck(PieceColor kingColor) const;

    // --- Методы для просмотра истории ---
    const Piece* browseHistory(int step); // Шаг: +1 (вперед), -1 (назад)
    void resetHistoryBrowser();
    int getHistorySize() const;
    int getCurrentHistoryIndex() const;

signals:
    // Испускается, когда пешка достигла последней горизонтали.
    void promotionRequired(int row, int col, PieceColor color);
    // Испускается после любого изменения на доске.
    void boardChanged();

private:
    // --- Внутреннее состояние игры ---
    Piece m_board[8][8];
    PieceColor m_currentTurn;
    GameStatus m_gameStatus;
    std::vector<Piece> m_whiteCaptured;
    std::vector<Piece> m_blackCaptured;
    // Права на рокировку [цвет][0 - длинная, 1 - короткая].
    bool m_castlingRights[3][2];
    // Начальные позиции короля и ладей для правил Chess960.
    int m_kingInitialCol[3];
    int m_rookInitialCols[3][2];
    // Последний совершенный ход.
    Move m_lastMove;
    // Координаты клетки, уязвимой для взятия на проходе.
    std::pair<int, int> m_enPassantTargetSquare;

    // История ходов в виде полных состояний доски.
    std::vector<std::array<Piece, 64>> m_history;
    int m_historyBrowserIndex; // Текущая позиция при просмотре истории.

    // --- Приватные вспомогательные функции ---
    void generateChess960Position();
    void switchTurn();
    void updateGameStatus();
    bool hasLegalMoves(PieceColor color);

    // --- Функции проверки правил ---
    // Главная функция проверки хода, включая проверку на шах королю.
    bool isMoveValid(const Piece board[8][8], PieceColor turn, const Move& move, bool checkKingSafety = true) const;
    // Проверяет, атакована ли клетка фигурами указанного цвета.
    bool isSquareAttacked(const Piece board[8][8], int row, int col, PieceColor attackerColor) const;
    bool isKingInCheck(const Piece board[8][8], PieceColor kingColor) const;

    // Функции проверки ходов для конкретных фигур.
    bool isPawnMoveValid(const Piece board[8][8], const Move& move) const;
    bool isKnightMoveValid(const Move& move) const;
    bool isKingMoveValid(const Piece board[8][8], PieceColor turn, const Move& move) const;
    // Обобщенная функция для скользящих фигур (ферзь, ладья, слон).
    bool isSlidingMoveValid(const Piece board[8][8], const Move& move, std::initializer_list<std::pair<int, int>> directions) const;
};

#endif
