#include "piece_logic.h"
#include <QRandomGenerator>
#include <algorithm>
#include <vector>
#include <cmath>

// Вспомогательная функция для проверки, находится ли ход в пределах доски
bool isWithinBoard(int r, int c) {
    return r >= 0 && r < 8 && c >= 0 && c < 8;
}

PieceLogic::PieceLogic(QObject *parent) : QObject(parent) {
    setupNewGame();
}

void PieceLogic::setupNewGame() {
    for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) m_board[r][c] = {NONE, NO_COLOR};
    m_currentTurn = WHITE;
    m_gameStatus = IN_PROGRESS;
    m_whiteCaptured.clear();
    m_blackCaptured.clear();
    m_castlingRights[WHITE][0] = m_castlingRights[WHITE][1] = true;
    m_castlingRights[BLACK][0] = m_castlingRights[BLACK][1] = true;
    m_lastMove = {};
    m_enPassantTargetSquare = {-1, -1};
    generateChess960Position();
    m_history.clear();
    std::array<Piece, 64> initialBoard;
    std::copy(&m_board[0][0], &m_board[0][0] + 64, initialBoard.begin());
    m_history.push_back(initialBoard);
    resetHistoryBrowser();
    emit boardChanged();
}

void PieceLogic::generateChess960Position() {
    std::vector<PieceType> back_rank(8);
    std::vector<int> empty_squares;
    for(int i=0; i<8; ++i) empty_squares.push_back(i);
    int bishop1_idx = QRandomGenerator::global()->bounded(4) * 2;
    int bishop2_idx = QRandomGenerator::global()->bounded(4) * 2 + 1;
    back_rank[bishop1_idx] = BISHOP;
    back_rank[bishop2_idx] = BISHOP;
    empty_squares.erase(std::remove_if(empty_squares.begin(), empty_squares.end(),
                                       [=](int s){ return s == bishop1_idx || s == bishop2_idx; }), empty_squares.end());
    int king_pos_in_empty = 1 + QRandomGenerator::global()->bounded(4);
    int king_square = empty_squares[king_pos_in_empty];
    back_rank[king_square] = KING;
    int rook1_pos_in_empty = QRandomGenerator::global()->bounded(king_pos_in_empty);
    int rook1_square = empty_squares[rook1_pos_in_empty];
    back_rank[rook1_square] = ROOK;
    int rook2_pos_in_empty = king_pos_in_empty + 1 + QRandomGenerator::global()->bounded(empty_squares.size() - (king_pos_in_empty + 1));
    int rook2_square = empty_squares[rook2_pos_in_empty];
    back_rank[rook2_square] = ROOK;
    m_kingInitialCol[WHITE] = m_kingInitialCol[BLACK] = king_square;
    m_rookInitialCols[WHITE][0] = m_rookInitialCols[BLACK][0] = std::min(rook1_square, rook2_square);
    m_rookInitialCols[WHITE][1] = m_rookInitialCols[BLACK][1] = std::max(rook1_square, rook2_square);
    empty_squares.erase(std::remove_if(empty_squares.begin(), empty_squares.end(),
                                       [=](int s){ return s == king_square || s == rook1_square || s == rook2_square; }), empty_squares.end());
    back_rank[empty_squares[0]] = QUEEN;
    back_rank[empty_squares[1]] = KNIGHT;
    back_rank[empty_squares[2]] = KNIGHT;
    for (int col = 0; col < 8; ++col) {
        m_board[0][col] = {back_rank[col], BLACK};
        m_board[1][col] = {PAWN, BLACK};
        m_board[6][col] = {PAWN, WHITE};
        m_board[7][col] = {back_rank[col], WHITE};
    }
}

bool PieceLogic::tryMove(int fromRow, int fromCol, int toRow, int toCol) {
    if (m_gameStatus != IN_PROGRESS) return false;
    Move move = {fromRow, fromCol, toRow, toCol};
    if (!isMoveValid(m_board, m_currentTurn, move)) return false;

    std::array<Piece, 64> newBoardState;
    std::copy(&m_board[0][0], &m_board[0][0] + 64, newBoardState.begin());

    Piece movingPiece = newBoardState[fromRow * 8 + fromCol];
    Piece targetPiece = newBoardState[toRow * 8 + toCol];

    if (movingPiece.type == PAWN && toRow == m_enPassantTargetSquare.first && toCol == m_enPassantTargetSquare.second) {
        int capturedPawnRow = (m_currentTurn == WHITE) ? toRow + 1 : toRow - 1;
        targetPiece = newBoardState[capturedPawnRow * 8 + toCol];
        newBoardState[capturedPawnRow * 8 + toCol] = {NONE, NO_COLOR};
    }
    if (targetPiece.type != NONE) {
        (targetPiece.color == WHITE) ? m_blackCaptured.push_back(targetPiece) : m_whiteCaptured.push_back(targetPiece);
    }
    if (movingPiece.type == KING) {
        m_castlingRights[m_currentTurn][0] = m_castlingRights[m_currentTurn][1] = false;
    } else if (movingPiece.type == ROOK) {
        if (fromCol == m_rookInitialCols[m_currentTurn][0]) m_castlingRights[m_currentTurn][0] = false;
        if (fromCol == m_rookInitialCols[m_currentTurn][1]) m_castlingRights[m_currentTurn][1] = false;
    }

    newBoardState[toRow * 8 + toCol] = newBoardState[fromRow * 8 + fromCol];
    newBoardState[fromRow * 8 + fromCol] = {NONE, NO_COLOR};

    // Перемещение ладьи при рокировке
    if (movingPiece.type == KING && std::abs(fromCol - toCol) > 1) {
        bool isShortCastle = toCol == 6;
        int rookInitialCol = m_rookInitialCols[m_currentTurn][isShortCastle ? 1 : 0];
        int rookFinalCol = isShortCastle ? 5 : 3;
        newBoardState[fromRow * 8 + rookFinalCol] = newBoardState[fromRow * 8 + rookInitialCol];
        newBoardState[fromRow * 8 + rookInitialCol] = {NONE, NO_COLOR};
    }

    std::copy(&newBoardState[0], &newBoardState[0] + 64, &m_board[0][0]);

    m_enPassantTargetSquare = {-1, -1};
    if (movingPiece.type == PAWN && std::abs(fromRow - toRow) == 2) {
        m_enPassantTargetSquare = { (fromRow + toRow) / 2, fromCol };
    }
    m_lastMove = move;

    m_history.push_back(newBoardState);
    resetHistoryBrowser();

    if (movingPiece.type == PAWN && (toRow == 0 || toRow == 7)) {
        emit promotionRequired(toRow, toCol, movingPiece.color);
    } else {
        switchTurn();
        updateGameStatus();
    }
    emit boardChanged();
    return true;
}

void PieceLogic::promotePawn(int row, int col, PieceType newType) {
    if (m_board[row][col].type == PAWN) {
        m_board[row][col].type = newType;
        std::copy(&m_board[0][0], &m_board[0][0] + 64, m_history.back().begin());
        switchTurn();
        updateGameStatus();
        emit boardChanged();
    }
}

bool PieceLogic::isMoveValid(const Piece board[8][8], PieceColor turn, const Move& move, bool checkKingSafety) const {
    Piece movingPiece = board[move.fromRow][move.fromCol];
    Piece targetPiece = board[move.toRow][move.toCol];
    if (movingPiece.color != turn || !isWithinBoard(move.toRow, move.toCol) || targetPiece.color == turn) return false;

    bool isValid = false;
    switch (movingPiece.type) {
    case PAWN:   isValid = isPawnMoveValid(board, move);   break;
    case KNIGHT: isValid = isKnightMoveValid(move); break;
    case BISHOP: isValid = isSlidingMoveValid(board, move, {{1,1}, {-1,-1}, {1,-1}, {-1,1}}); break;
    case ROOK:   isValid = isSlidingMoveValid(board, move, {{1,0}, {-1,0}, {0,1}, {0,-1}}); break;
    case QUEEN:  isValid = isSlidingMoveValid(board, move, {{1,1}, {-1,-1}, {1,-1}, {-1,1}, {1,0}, {-1,0}, {0,1}, {0,-1}}); break;
    case KING:   isValid = isKingMoveValid(board, turn, move);   break;
    default: return false;
    }
    if (!isValid) return false;

    if (checkKingSafety) {
        Piece tempBoard[8][8];
        std::copy(&board[0][0], &board[0][0] + 64, &tempBoard[0][0]);
        // Симулируем ход для проверки на шах
        tempBoard[move.toRow][move.toCol] = tempBoard[move.fromRow][move.fromCol];
        tempBoard[move.fromRow][move.fromCol] = {NONE, NO_COLOR};
        if (isKingInCheck(tempBoard, turn)) return false;
    }
    return true;
}

bool PieceLogic::isPawnMoveValid(const Piece board[8][8], const Move& move) const {
    Piece movingPiece = board[move.fromRow][move.fromCol];
    Piece targetPiece = board[move.toRow][move.toCol];
    int direction = (movingPiece.color == WHITE) ? -1 : 1;
    int startRow = (movingPiece.color == WHITE) ? 6 : 1;
    if (std::abs(move.fromCol - move.toCol) == 1 && move.toRow == move.fromRow + direction) {
        if (targetPiece.type != NONE) return true;
        if (move.toRow == m_enPassantTargetSquare.first && move.toCol == m_enPassantTargetSquare.second) return true;
    }
    if (move.fromCol == move.toCol && targetPiece.type == NONE) {
        if (move.toRow == move.fromRow + direction) return true;
        if (move.fromRow == startRow && move.toRow == move.fromRow + 2 * direction && board[move.fromRow + direction][move.fromCol].type == NONE) return true;
    }
    return false;
}

bool PieceLogic::isKnightMoveValid(const Move& move) const {
    return (std::abs(move.fromRow - move.toRow) == 2 && std::abs(move.fromCol - move.toCol) == 1) ||
           (std::abs(move.fromRow - move.toRow) == 1 && std::abs(move.fromCol - move.toCol) == 2);
}

// ИСПРАВЛЕНИЕ 2: Полная реализация правил рокировки для Chess960
bool PieceLogic::isKingMoveValid(const Piece board[8][8], PieceColor turn, const Move& move) const {
    int dr = std::abs(move.fromRow - move.toRow);
    int dc = std::abs(move.fromCol - move.toCol);
    if (dr <= 1 && dc <= 1) return true;

    // Проверка, является ли ход попыткой рокировки
    if (dr == 0 && (move.toCol == 2 || move.toCol == 6)) {
        int homeRow = (turn == WHITE) ? 7 : 0;
        // 1. Король должен быть на своей начальной клетке (в Chess960 это не так, он может двигаться по горизонтали и рокироваться)
        // Но он не должен был ходить до этого! Мы это проверяем через m_castlingRights.
        if (isKingInCheck(board, turn)) return false; // 2. Король не должен быть под шахом

        bool isShortCastle = move.toCol == 6;
        int castlingIndex = isShortCastle ? 1 : 0;

        if (!m_castlingRights[turn][castlingIndex]) return false; // 3. Право на рокировку не утеряно

        int rookInitialCol = m_rookInitialCols[turn][castlingIndex];
        int kingInitialCol = m_kingInitialCol[turn];

        // 4. Все клетки между начальной позицией короля и его конечной позицией (c1/g1) должны быть пустыми
        for (int c = std::min(kingInitialCol, move.toCol) + 1; c < std::max(kingInitialCol, move.toCol); ++c) {
            if (board[homeRow][c].type != NONE) return false;
        }

        // 5. Все клетки между начальной позицией ладьи и ее конечной позицией (d1/f1) должны быть пустыми
        int rookFinalCol = isShortCastle ? 5 : 3;
        for (int c = std::min(rookInitialCol, rookFinalCol) + 1; c < std::max(rookInitialCol, rookFinalCol); ++c) {
            if (board[homeRow][c].type != NONE) return false;
        }

        // 6. Король не должен проходить через битое поле
        PieceColor opponent = (turn == WHITE) ? BLACK : WHITE;
        for (int c = std::min(move.fromCol, move.toCol); c <= std::max(move.fromCol, move.toCol); ++c) {
            if (isSquareAttacked(board, homeRow, c, opponent)) return false;
        }

        return true;
    }
    return false;
}

bool PieceLogic::isSlidingMoveValid(const Piece board[8][8], const Move& move, std::initializer_list<std::pair<int, int>> directions) const {
    int dr = move.toRow - move.fromRow;
    int dc = move.toCol - move.fromCol;
    for (auto dir : directions) {
        int step_r = dir.first; int step_c = dir.second;
        if (dr * step_c == dc * step_r && (dr != 0 || dc != 0)) {
            if ( (dr > 0 && step_r < 0) || (dr < 0 && step_r > 0) || (dc > 0 && step_c < 0) || (dc < 0 && step_c > 0) ) continue;
            int r = move.fromRow + step_r; int c = move.fromCol + step_c;
            while (r != move.toRow || c != move.toCol) {
                if (!isWithinBoard(r, c) || board[r][c].type != NONE) return false;
                r += step_r; c += step_c;
            }
            return true;
        }
    }
    return false;
}

bool PieceLogic::isKingInCheck(const Piece board[8][8], PieceColor kingColor) const {
    int kingRow = -1, kingCol = -1;
    for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) {
            if (board[r][c].type == KING && board[r][c].color == kingColor) { kingRow = r; kingCol = c; break; }
        }
    return (kingRow != -1) && isSquareAttacked(board, kingRow, kingCol, (kingColor == WHITE) ? BLACK : WHITE);
}

bool PieceLogic::isSquareAttacked(const Piece board[8][8], int row, int col, PieceColor attackerColor) const {
    for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) {
            if (board[r][c].color == attackerColor) {
                if (isMoveValid(board, attackerColor, {r, c, row, col}, false)) return true;
            }
        }
    return false;
}

bool PieceLogic::hasLegalMoves(PieceColor color) {
    for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) {
            if (m_board[r][c].color == color) {
                if (!getValidMovesForPiece(r, c).empty()) return true;
            }
        }
    return false;
}

std::vector<Move> PieceLogic::getValidMovesForPiece(int row, int col) {
    std::vector<Move> validMoves;
    if (m_board[row][col].color != m_currentTurn) return validMoves;
    for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) {
            Move move = {row, col, r, c};
            if (isMoveValid(m_board, m_currentTurn, move)) validMoves.push_back(move);
        }
    return validMoves;
}

const Piece* PieceLogic::browseHistory(int step) {
    int newIndex = m_historyBrowserIndex + step;
    if (newIndex >= 0 && static_cast<size_t>(newIndex) < m_history.size()) {
        m_historyBrowserIndex = newIndex;
        return m_history[m_historyBrowserIndex].data();
    }
    return nullptr;
}

void PieceLogic::resetHistoryBrowser() {
    m_historyBrowserIndex = m_history.empty() ? -1 : m_history.size() - 1;
}

int PieceLogic::getHistorySize() const { return m_history.size(); }
int PieceLogic::getCurrentHistoryIndex() const { return m_historyBrowserIndex; }

void PieceLogic::switchTurn() { m_currentTurn = (m_currentTurn == WHITE) ? BLACK : WHITE; }
void PieceLogic::updateGameStatus() {
    if (!hasLegalMoves(m_currentTurn)) {
        m_gameStatus = isKingInCheck(m_board, m_currentTurn) ? CHECKMATE : STALEMATE;
    } else {
        m_gameStatus = IN_PROGRESS;
    }
}
void PieceLogic::setBoardFromLayout(const QString& layout)
{
    // Очищаем старую историю и состояние
    m_history.clear();
    m_whiteCaptured.clear();
    m_blackCaptured.clear();
    m_gameStatus = IN_PROGRESS;
    m_currentTurn = WHITE; // Игра всегда начинается с хода белых

    // Разбираем строку, полученную по сети
    QStringList pairs = layout.split(';', Qt::SkipEmptyParts);
    if (pairs.size() != 64) {
        // Ошибка в полученных данных, лучше вернуться к стандартной позиции
        setupNewGame();
        return;
    }

    std::array<Piece, 64> initialBoard;
    int index = 0;
    for (const QString& pair : pairs) {
        QStringList parts = pair.split(',');
        if (parts.size() == 2) {
            Piece p;
            p.type = static_cast<PieceType>(parts[0].toInt());
            p.color = static_cast<PieceColor>(parts[1].toInt());
            m_board[index / 8][index % 8] = p;
            initialBoard[index] = p;
        }
        index++;
    }

    // Сохраняем начальную позицию в истории
    m_history.push_back(initialBoard);
    resetHistoryBrowser();

    // Сообщаем UI, что доска изменилась
    emit boardChanged();
}

bool PieceLogic::isKingInCheck(PieceColor kingColor) const { return isKingInCheck(m_board, kingColor); }
Piece PieceLogic::getPieceAt(int row, int col) const { return m_board[row][col]; }
PieceColor PieceLogic::getCurrentTurn() const { return m_currentTurn; }
GameStatus PieceLogic::getGameStatus() const { return m_gameStatus; }
const std::vector<Piece>& PieceLogic::getCapturedPieces(PieceColor color) const { return (color == WHITE) ? m_whiteCaptured : m_blackCaptured; }
