#ifndef PIECE_LOGIC_H
#define PIECE_LOGIC_H

#include <QObject>
#include <vector>
#include <utility>
#include <array>

// Перечисления для типов фигур, цвета и статуса игры.
enum PieceType { NONE, KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };
enum PieceColor { NO_COLOR, WHITE, BLACK };
enum GameStatus { IN_PROGRESS, CHECKMATE, STALEMATE };

// Структура, представляющая одну фигуру на доске.
struct Piece {
    PieceType type = NONE;
    PieceColor color = NO_COLOR;
};

// Структура, представляющая один ход. Включает поле для превращения пешки.
struct Move {
    int fromRow, fromCol;
    int toRow, toCol;
    PieceType promotion = NONE;
};

/**
 * @class PieceLogic
 * @brief Игровая логика (Модель). Не зависит от UI и сети.
 *
 * Управляет состоянием доски, проверяет корректность ходов по правилам
 * Chess960, определяет исход игры и хранит историю партии.
 */
class PieceLogic : public QObject
{
    Q_OBJECT

public:
    explicit PieceLogic(QObject *parent = nullptr);

    // --- Основной API для управления игрой ---
    void setupNewGame();                    // Начинает новую игру со случайной расстановкой.
    bool tryMove(const Move& move);         // Пытается выполнить ход.
    void setBoardFromLayout(const QString& layout); // Устанавливает доску из строки (для сети).
    void forceEndGame();                    // Принудительно завершает игру (для дисконнекта).

    // --- Методы для получения состояния игры ---
    Piece getPieceAt(int row, int col) const;
    PieceColor getCurrentTurn() const;
    GameStatus getGameStatus() const;
    const std::vector<Piece>& getCapturedPieces(PieceColor color) const;
    std::vector<Move> getValidMovesForPiece(int row, int col);
    bool isKingInCheck(PieceColor kingColor) const;

    // --- Методы для просмотра истории ---
    const Piece* browseHistory(int step);
    void resetHistoryBrowser();
    int getHistorySize() const;
    int getCurrentHistoryIndex() const;

signals:
    // Сигнал, который уведомляет UI об изменении состояния доски.
    void boardChanged();

private:
    // --- Внутреннее состояние игры ---
    Piece m_board[8][8];
    PieceColor m_currentTurn;
    GameStatus m_gameStatus;
    std::vector<Piece> m_whiteCaptured;
    std::vector<Piece> m_blackCaptured;
    bool m_castlingRights[3][2];            // Права на рокировку [цвет][0-длинная, 1-короткая].
    int m_kingInitialCol[3];                // Исходное положение короля.
    int m_rookInitialCols[3][2];            // Исходное положение ладей.
    Move m_lastMove;
    std::pair<int, int> m_enPassantTargetSquare; // Клетка для взятия на проходе.

    // --- История ---
    std::vector<std::array<Piece, 64>> m_history;
    int m_historyBrowserIndex;

    // --- Приватные вспомогательные функции ---
    void generateChess960Position();
    void switchTurn();
    void updateGameStatus();
    bool hasLegalMoves(PieceColor color);

    // Функции валидации ходов
    bool isMoveValid(const Piece board[8][8], PieceColor turn, const Move& move, bool checkKingSafety = true) const;
    bool isSquareAttacked(const Piece board[8][8], int row, int col, PieceColor attackerColor) const;
    bool isKingInCheck(const Piece board[8][8], PieceColor kingColor) const;
    bool isPawnMoveValid(const Piece board[8][8], const Move& move) const;
    bool isKnightMoveValid(const Move& move) const;
    bool isKingMoveValid(const Piece board[8][8], PieceColor turn, const Move& move, bool checkKingSafety = true) const;
    bool isSlidingMoveValid(const Piece board[8][8], const Move& move, std::initializer_list<std::pair<int, int>> directions) const;
};

#endif // PIECE_LOGIC_H
