#include "piece_logic.h"
#include <QRandomGenerator>
#include <algorithm>
#include <vector>
#include <cmath>
#include <QTextStream>

bool isWithinBoard(int r, int c) {
    return r >= 0 && r < 8 && c >= 0 && c < 8;
}

PieceLogic::PieceLogic(QObject *parent) : QObject(parent) {
    setupNewGame();
}

// Полный сброс и настройка новой игры.
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

// Генерация случайной стартовой позиции по правилам Chess960.
void PieceLogic::generateChess960Position() {
    std::vector<PieceType> back_rank(8);
    std::vector<int> empty_squares;
    for(int i=0; i<8; ++i) empty_squares.push_back(i);
    // 1. Слоны на разноцветные поля.
    int bishop1_idx = QRandomGenerator::global()->bounded(4) * 2;
    int bishop2_idx = QRandomGenerator::global()->bounded(4) * 2 + 1;
    back_rank[bishop1_idx] = BISHOP;
    back_rank[bishop2_idx] = BISHOP;
    empty_squares.erase(std::remove_if(empty_squares.begin(), empty_squares.end(),
                                       [=](int s){ return s == bishop1_idx || s == bishop2_idx; }), empty_squares.end());
    // 2. Король между ладьями.
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
    // 3. Остальные фигуры на оставшиеся поля.
    empty_squares.erase(std::remove_if(empty_squares.begin(), empty_squares.end(),
                                       [=](int s){ return s == king_square || s == rook1_square || s == rook2_square; }), empty_squares.end());
    back_rank[empty_squares[0]] = QUEEN;
    back_rank[empty_squares[1]] = KNIGHT;
    back_rank[empty_squares[2]] = KNIGHT;
    // Расставляем фигуры для обоих цветов.
    for (int col = 0; col < 8; ++col) {
        m_board[0][col] = {back_rank[col], BLACK};
        m_board[1][col] = {PAWN, BLACK};
        m_board[6][col] = {PAWN, WHITE};
        m_board[7][col] = {back_rank[col], WHITE};
    }
}

// Восстанавливает доску из строкового представления (для сетевой игры).
void PieceLogic::setBoardFromLayout(const QString& layout)
{
    m_history.clear();
    m_whiteCaptured.clear();
    m_blackCaptured.clear();
    m_gameStatus = IN_PROGRESS;
    m_currentTurn = WHITE;

    QStringList pairs = layout.split(';', Qt::SkipEmptyParts);
    if (pairs.size() != 64) {
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
    m_history.push_back(initialBoard);
    resetHistoryBrowser();
    emit boardChanged();
}

// Атомарно выполняет ход, включая рокировку и превращение.
bool PieceLogic::tryMove(const Move& move) {
    if (m_gameStatus != IN_PROGRESS) return false;
    if (!isMoveValid(m_board, m_currentTurn, move)) return false;

    Piece movingPiece = m_board[move.fromRow][move.fromCol];
    Piece targetPiece = m_board[move.toRow][move.toCol];

    bool isCastle = (movingPiece.type == KING && targetPiece.type == ROOK && movingPiece.color == targetPiece.color);

    if (isCastle) {
        int rookStartCol = move.toCol;
        int kingStartCol = move.fromCol;
        bool isShortCastle = rookStartCol > kingStartCol;

        int kingDestCol = isShortCastle ? 6 : 2;
        int rookDestCol = isShortCastle ? 5 : 3;

        // Безопасное выполнение: сначала убираем фигуры, потом ставим.
        m_board[move.fromRow][kingStartCol] = {NONE, NO_COLOR};
        m_board[move.fromRow][rookStartCol] = {NONE, NO_COLOR};
        m_board[move.fromRow][kingDestCol] = movingPiece;
        m_board[move.fromRow][rookDestCol] = targetPiece;

    } else { // Обычный ход
        if (movingPiece.type == PAWN && move.toRow == m_enPassantTargetSquare.first && move.toCol == m_enPassantTargetSquare.second) {
            int capturedPawnRow = (m_currentTurn == WHITE) ? move.toRow + 1 : move.toRow - 1;
            targetPiece = m_board[capturedPawnRow][move.toCol];
            m_board[capturedPawnRow][move.toCol] = {NONE, NO_COLOR};
        }

        if (targetPiece.type != NONE && targetPiece.color != movingPiece.color) {
            (targetPiece.color == WHITE) ? m_whiteCaptured.push_back(targetPiece) : m_blackCaptured.push_back(targetPiece);
        }

        m_board[move.toRow][move.toCol] = m_board[move.fromRow][move.fromCol];
        m_board[move.fromRow][move.fromCol] = {NONE, NO_COLOR};

        if (move.promotion != NONE) {
            m_board[move.toRow][move.toCol].type = move.promotion;
        }
    }

    // Обновление прав на рокировку.
    if (movingPiece.type == KING) {
        m_castlingRights[m_currentTurn][0] = m_castlingRights[m_currentTurn][1] = false;
    } else if (movingPiece.type == ROOK) {
        if (move.fromCol == m_rookInitialCols[m_currentTurn][0]) m_castlingRights[m_currentTurn][0] = false;
        if (move.fromCol == m_rookInitialCols[m_currentTurn][1]) m_castlingRights[m_currentTurn][1] = false;
    }

    // Обновление состояния игры.
    m_enPassantTargetSquare = {-1, -1};
    if (movingPiece.type == PAWN && std::abs(move.fromRow - move.toRow) == 2) {
        m_enPassantTargetSquare = { (move.fromRow + move.toRow) / 2, move.fromCol };
    }
    m_lastMove = move;

    // Сохранение в историю.
    std::array<Piece, 64> newBoardState;
    std::copy(&m_board[0][0], &m_board[0][0] + 64, newBoardState.begin());
    m_history.push_back(newBoardState);
    resetHistoryBrowser();

    switchTurn();
    updateGameStatus();

    emit boardChanged();
    return true;
}

// Принудительно завершает игру (используется при дисконнекте).
void PieceLogic::forceEndGame()
{
    m_gameStatus = CHECKMATE; // Любой статус завершения, чтобы UI включил историю.
    emit boardChanged();
}

// Главная функция проверки валидности хода.
bool PieceLogic::isMoveValid(const Piece board[8][8], PieceColor turn, const Move& move, bool checkKingSafety) const {
    Piece movingPiece = board[move.fromRow][move.fromCol];
    Piece targetPiece = board[move.toRow][move.toCol];

    // Базовые проверки
    if (movingPiece.color != turn || !isWithinBoard(move.toRow, move.toCol)) return false;

    // Проверка на рокировку (специальный случай)
    bool isCastling = (movingPiece.type == KING && targetPiece.type == ROOK &&
                       targetPiece.color == turn);

    // Для всех случаев, кроме рокировки - запрещаем взятие своих фигур
    if (!isCastling && targetPiece.color == turn) return false;

    if (movingPiece.type == KING) {
        return isKingMoveValid(board, turn, move, checkKingSafety);
    }

    bool isValid = false;
    switch (movingPiece.type) {
    case PAWN:   isValid = isPawnMoveValid(board, move);   break;
    case KNIGHT: isValid = isKnightMoveValid(move); break;
    case BISHOP: isValid = isSlidingMoveValid(board, move, {{1,1}, {-1,-1}, {1,-1}, {-1,1}}); break;
    case ROOK:   isValid = isSlidingMoveValid(board, move, {{1,0}, {-1,0}, {0,1}, {0,-1}}); break;
    case QUEEN:  isValid = isSlidingMoveValid(board, move, {{1,1}, {-1,-1}, {1,-1}, {-1,1}, {1,0}, {-1,0}, {0,1}, {0,-1}}); break;
    default: return false;
    }
    if (!isValid) return false;

    // Проверка, не останется ли король под шахом после хода.
    if (checkKingSafety) {
        Piece tempBoard[8][8];
        std::copy(&board[0][0], &board[0][0] + 64, &tempBoard[0][0]);
        tempBoard[move.toRow][move.toCol] = tempBoard[move.fromRow][move.fromCol];
        tempBoard[move.fromRow][move.fromCol] = {NONE, NO_COLOR};

        if (move.promotion != NONE) {
            tempBoard[move.toRow][move.toCol].type = move.promotion;
        }

        if (isKingInCheck(tempBoard, turn)) return false;
    }
    return true;
}

bool PieceLogic::isPawnMoveValid(const Piece board[8][8], const Move& move) const {
    Piece movingPiece = board[move.fromRow][move.fromCol];
    Piece targetPiece = board[move.toRow][move.toCol];
    int direction = (movingPiece.color == WHITE) ? -1 : 1;
    int startRow = (movingPiece.color == WHITE) ? 6 : 1;
    // Взятие.
    if (std::abs(move.fromCol - move.toCol) == 1 && move.toRow == move.fromRow + direction) {
        if (targetPiece.type != NONE && targetPiece.color != movingPiece.color) return true;
        if (move.toRow == m_enPassantTargetSquare.first && move.toCol == m_enPassantTargetSquare.second) return true;
    }
    // Тихий ход.
    if (move.fromCol == move.toCol && targetPiece.type == NONE) {
        if (move.toRow == move.fromRow + direction) return true;
        // Двойной ход.
        if (move.fromRow == startRow && move.toRow == move.fromRow + 2 * direction && board[move.fromRow + direction][move.fromCol].type == NONE) return true;
    }
    return false;
}

bool PieceLogic::isKnightMoveValid(const Move& move) const {
    return (std::abs(move.fromRow - move.toRow) == 2 && std::abs(move.fromCol - move.toCol) == 1) ||
           (std::abs(move.fromRow - move.toRow) == 1 && std::abs(move.fromCol - move.toCol) == 2);
}

// Корректная проверка рокировки для Chess960.
bool PieceLogic::isKingMoveValid(const Piece board[8][8], PieceColor turn, const Move& move, bool checkKingSafety) const {
    // Обычный ход короля.
    if (std::abs(move.fromRow - move.toRow) <= 1 && std::abs(move.fromCol - move.toCol) <= 1) {
        if (board[move.toRow][move.toCol].color == turn) return false;
        if (checkKingSafety) {
            // Проверяем, не будет ли король под шахом после хода
            Piece tempBoard[8][8];
            std::copy(&board[0][0], &board[0][0] + 64, &tempBoard[0][0]);
            tempBoard[move.toRow][move.toCol] = tempBoard[move.fromRow][move.fromCol];
            tempBoard[move.fromRow][move.fromCol] = {NONE, NO_COLOR};
            return !isKingInCheck(tempBoard, turn);
        }
        return true;
    }
    // Проверка на попытку рокировки (клик по королю, затем по своей ладье).
    if (move.fromRow == move.toRow &&
        board[move.toRow][move.toCol].type == ROOK &&
        board[move.toRow][move.toCol].color == turn) {
        if (isKingInCheck(board, turn)) return false;

        int kingStartCol = move.fromCol;
        int rookStartCol = move.toCol;
        int homeRow = move.fromRow;
        PieceColor opponent = (turn == WHITE) ? BLACK : WHITE;

        bool isShortCastle = rookStartCol > kingStartCol;
        int castlingIndex = isShortCastle ? 1 : 0;
        if (!m_castlingRights[turn][castlingIndex]) return false;

        int kingDestCol = isShortCastle ? 6 : 2;
        int rookDestCol = isShortCastle ? 5 : 3;

        // Проверяем путь короля на атаки.
        for (int c = std::min(kingStartCol, kingDestCol); c <= std::max(kingStartCol, kingDestCol); ++c) {
            if (isSquareAttacked(board, homeRow, c, opponent)) return false;
        }

        // Проверяем все клетки МЕЖДУ королем и ладьей на пустоту.
        int pathStart = std::min(kingStartCol, rookStartCol);
        int pathEnd = std::max(kingStartCol, rookStartCol);
        for (int c = pathStart + 1; c < pathEnd; ++c) {
            if (c != kingStartCol && c != rookStartCol && board[homeRow][c].type != NONE) {
                return false;
            }
        }

        // Проверка, что конечные клетки пусты (если только их не займут сами рокирующиеся фигуры).
        if (board[homeRow][kingDestCol].type != NONE && kingDestCol != rookStartCol) return false;
        if (board[homeRow][rookDestCol].type != NONE && rookDestCol != kingStartCol) return false;

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

// Проверяет, атакована ли клетка (row, col) фигурами цвета attackerColor.
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
                // Вызываем isMoveValid без проверки на шах, чтобы избежать бесконечной рекурсии.
                if (isMoveValid(board, attackerColor, {r, c, row, col}, false)) return true;
            }
        }
    return false;
}

// Проверяет, есть ли у игрока цвета color хотя бы один легальный ход.
bool PieceLogic::hasLegalMoves(PieceColor color) {
    for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) {
            if (m_board[r][c].color == color) {
                if (!getValidMovesForPiece(r, c).empty()) return true;
            }
        }
    return false;
}

// Собирает ВСЕ легальные ходы для фигуры на (row, col).
std::vector<Move> PieceLogic::getValidMovesForPiece(int row, int col) {
    std::vector<Move> validMoves;
    if (m_board[row][col].color != m_currentTurn) return validMoves;

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Move move = {row, col, r, c};
            if (isMoveValid(m_board, m_currentTurn, move)) {
                validMoves.push_back(move);
            }
        }
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
bool PieceLogic::isKingInCheck(PieceColor kingColor) const { return isKingInCheck(m_board, kingColor); }
Piece PieceLogic::getPieceAt(int row, int col) const { return m_board[row][col]; }
PieceColor PieceLogic::getCurrentTurn() const { return m_currentTurn; }
GameStatus PieceLogic::getGameStatus() const { return m_gameStatus; }
const std::vector<Piece>& PieceLogic::getCapturedPieces(PieceColor color) const { return (color == WHITE) ? m_whiteCaptured : m_blackCaptured; }
