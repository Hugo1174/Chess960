#ifndef PIECE_LOGIC_H
#define PIECE_LOGIC_H

#include <QObject>
#include <vector>
#include <utility>
#include <array>

// --- Основные структуры данных для игры ---

enum PieceType { NONE, KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };
enum PieceColor { NO_COLOR, WHITE, BLACK };
enum GameStatus { IN_PROGRESS, CHECKMATE, STALEMATE };

struct Piece {
    PieceType type = NONE;
    PieceColor color = NO_COLOR;
};

struct Move {
    int fromRow, fromCol;
    int toRow, toCol;
};

/**
 * @class PieceLogic
 * @brief Класс, инкапсулирующий всю логику шахматной игры (Модель).
 *
 * Этот класс ничего не знает об интерфейсе. Он управляет состоянием доски,
 * проверяет корректность ходов, определяет исход игры и хранит историю партии.
 * Взаимодействует с UI через сигналы и публичные методы.
 */
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

    // --- Новые методы для просмотра истории ---
    const Piece* browseHistory(int step);
    void resetHistoryBrowser();
    int getHistorySize() const;
    int getCurrentHistoryIndex() const;

signals:
    /**
     * @brief Сигнал, испускаемый, когда пешка достигла последней горизонтали и требует превращения.
     */
    void promotionRequired(int row, int col, PieceColor color);

    /**
     * @brief Сигнал, испускаемый после любого успешного изменения состояния доски.
     */
    void boardChanged();

private:
    // --- Внутреннее состояние игры ---
    Piece m_board[8][8];
    PieceColor m_currentTurn;
    GameStatus m_gameStatus;
    std::vector<Piece> m_whiteCaptured;
    std::vector<Piece> m_blackCaptured;
    bool m_castlingRights[3][2]; // [цвет][0-длинная, 1-короткая]
    int m_kingInitialCol[3];
    int m_rookInitialCols[3][2];
    Move m_lastMove;
    std::pair<int, int> m_enPassantTargetSquare;

    // --- Хранение и просмотр истории ---
    std::vector<std::array<Piece, 64>> m_history;
    int m_historyBrowserIndex;

    // --- Приватные вспомогательные функции ---
    void generateChess960Position();
    void switchTurn();
    void updateGameStatus();
    bool hasLegalMoves(PieceColor color);

    // Функции проверки правил, работающие с любой доской (для симуляции)
    bool isMoveValid(const Piece board[8][8], PieceColor turn, const Move& move, bool checkKingSafety = true) const;
    bool isSquareAttacked(const Piece board[8][8], int row, int col, PieceColor attackerColor) const;
    bool isKingInCheck(const Piece board[8][8], PieceColor kingColor) const;

    // Функции проверки ходов для конкретных фигур
    bool isPawnMoveValid(const Piece board[8][8], const Move& move) const;
    bool isKnightMoveValid(const Move& move) const;
    bool isKingMoveValid(const Piece board[8][8], PieceColor turn, const Move& move) const;
    bool isSlidingMoveValid(const Piece board[8][8], const Move& move, std::initializer_list<std::pair<int, int>> directions) const;
};

#endif // PIECE_LOGIC_H
