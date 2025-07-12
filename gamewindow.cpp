// gamewindow.cpp

#include "gamewindow.h"
#include "promotiondialog.h"
#include "networkhandler.h"
#include "clickablelabel.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QMap>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QRandomGenerator>

gamewindow::gamewindow(GameMode mode, const QString& peerIp, QWidget *parent)
    : QMainWindow(parent)
    , m_logic(new PieceLogic(this))
    , m_gameMode(mode)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Chess960 - Игра");
    setMinimumSize(1280, 720);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(45, 45, 45));
    setAutoFillBackground(true);
    setPalette(pal);

    setupUI();

    if (m_gameMode == LOCAL) {
        connect(m_logic, &PieceLogic::boardChanged, this, &gamewindow::onBoardChanged);
        connect(m_logic, &PieceLogic::promotionRequired, this, &gamewindow::handlePromotion);
        m_logic->setupNewGame();
    } else {
        setupNetwork(peerIp);
    }
    updateStatusLabel();
}

void gamewindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    QVBoxLayout *leftPanelLayout = new QVBoxLayout();
    QWidget* leftPanelWidget = new QWidget();

    m_statusLabel = new QLabel("Инициализация...");
    m_statusLabel->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    leftPanelLayout->addWidget(m_statusLabel);
    leftPanelLayout->addSpacing(10);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QPushButton* newGameButton = new QPushButton("Новая игра");
    QPushButton* backToMenuButton = new QPushButton("В меню");
    newGameButton->setVisible(m_gameMode == LOCAL);

    QString buttonStyle = R"(
        QPushButton {
            background-color: #555555; color: white; border: 1px solid #777777;
            padding: 8px; font-weight: bold; border-radius: 4px;
        }
        QPushButton:hover { background-color: #666666; }
        QPushButton:pressed { background-color: #444444; }
    )";
    newGameButton->setStyleSheet(buttonStyle);
    backToMenuButton->setStyleSheet(buttonStyle);

    connect(newGameButton, &QPushButton::clicked, this, &gamewindow::onNewGameClicked);
    connect(backToMenuButton, &QPushButton::clicked, this, &gamewindow::onBackToMenuClicked);

    buttonsLayout->addWidget(newGameButton);
    buttonsLayout->addWidget(backToMenuButton);
    leftPanelLayout->addLayout(buttonsLayout);
    leftPanelLayout->addSpacing(20);

    QLabel* blackCapturedLabel = new QLabel("Съеденные (Черные)");
    blackCapturedLabel->setStyleSheet("color: white; font-weight: bold;");
    leftPanelLayout->addWidget(blackCapturedLabel);
    m_blackCapturedLayout = new QGridLayout();
    m_blackCapturedLayout->setSpacing(5);
    leftPanelLayout->addLayout(m_blackCapturedLayout);
    leftPanelLayout->addStretch(1);

    QLabel* whiteCapturedLabel = new QLabel("Съеденные (Белые)");
    whiteCapturedLabel->setStyleSheet("color: white; font-weight: bold;");
    leftPanelLayout->addWidget(whiteCapturedLabel);
    m_whiteCapturedLayout = new QGridLayout();
    m_whiteCapturedLayout->setSpacing(5);
    leftPanelLayout->addLayout(m_whiteCapturedLayout);
    leftPanelLayout->addStretch(1);

    QHBoxLayout* historyButtonsLayout = new QHBoxLayout();
    m_prevMoveButton = new QPushButton("<");
    m_nextMoveButton = new QPushButton(">");
    QString historyButtonStyle = buttonStyle + "font-size: 18px;";
    m_prevMoveButton->setFixedSize(40, 40);
    m_nextMoveButton->setFixedSize(40, 40);
    m_prevMoveButton->setStyleSheet(historyButtonStyle);
    m_nextMoveButton->setStyleSheet(historyButtonStyle);

    connect(m_prevMoveButton, &QPushButton::clicked, this, &gamewindow::onPrevMoveClicked);
    connect(m_nextMoveButton, &QPushButton::clicked, this, &gamewindow::onNextMoveClicked);

    historyButtonsLayout->addStretch();
    historyButtonsLayout->addWidget(m_prevMoveButton);
    historyButtonsLayout->addWidget(m_nextMoveButton);
    historyButtonsLayout->addStretch();
    leftPanelLayout->addLayout(historyButtonsLayout);
    leftPanelWidget->setLayout(leftPanelLayout);

    QGridLayout *boardLayout = new QGridLayout();
    boardLayout->setSpacing(0);
    for (int c = 0; c < 8; ++c) {
        QString letter = QChar('a' + c);
        QLabel* topLetter = new QLabel(letter); topLetter->setAlignment(Qt::AlignCenter); topLetter->setFixedSize(90, 30); topLetter->setStyleSheet("color: white; font-weight: bold;");
        QLabel* bottomLetter = new QLabel(letter); bottomLetter->setAlignment(Qt::AlignCenter); bottomLetter->setFixedSize(90, 30); bottomLetter->setStyleSheet("color: white; font-weight: bold;");
        boardLayout->addWidget(topLetter, 0, c + 1); boardLayout->addWidget(bottomLetter, 9, c + 1);
    }
    for (int r = 0; r < 8; ++r) {
        QString number = QString::number(8 - r);
        QLabel* leftNumber = new QLabel(number); leftNumber->setAlignment(Qt::AlignCenter); leftNumber->setFixedSize(30, 90); leftNumber->setStyleSheet("color: white; font-weight: bold;");
        QLabel* rightNumber = new QLabel(number); rightNumber->setAlignment(Qt::AlignCenter); rightNumber->setFixedSize(30, 90); rightNumber->setStyleSheet("color: white; font-weight: bold;");
        boardLayout->addWidget(leftNumber, r + 1, 0); boardLayout->addWidget(rightNumber, r + 1, 10);
        for (int c = 0; c < 8; ++c) {
            ClickableLabel* cell = new ClickableLabel(); cell->setFixedSize(90, 90); cell->setAlignment(Qt::AlignCenter); cell->row = r; cell->col = c;
            connect(cell, &ClickableLabel::clicked, this, &gamewindow::handleCellClick);
            boardLayout->addWidget(cell, r + 1, c + 1);
            m_boardCells[r][c] = cell;
        }
    }
    QWidget* boardContainer = new QWidget();
    boardContainer->setLayout(boardLayout);
    boardContainer->setFixedSize(boardLayout->sizeHint());

    m_moveHistory = new QTextEdit();
    m_moveHistory->setReadOnly(true);
    m_moveHistory->setStyleSheet("background-color: #3c3c3c; color: white; border: 1px solid gray;");
    m_moveHistory->setMinimumWidth(200);

    mainLayout->addWidget(leftPanelWidget);
    mainLayout->addStretch(1);
    mainLayout->addWidget(boardContainer);
    mainLayout->addStretch(1);
    mainLayout->addWidget(m_moveHistory);
}

void gamewindow::setupNetwork(const QString& peerIp) {
    m_networkHandler = new NetworkHandler(this);
    connect(m_networkHandler, &NetworkHandler::connectedToPeer, this, &gamewindow::onConnected);
    connect(m_networkHandler, &NetworkHandler::disconnectedFromPeer, this, &gamewindow::onDisconnected);
    connect(m_networkHandler, &NetworkHandler::errorOccurred, this, &gamewindow::onNetworkError);
    connect(m_networkHandler, &NetworkHandler::gameStartReceived, this, &gamewindow::onGameStartReceived);
    connect(m_networkHandler, &NetworkHandler::moveReceived, this, &gamewindow::onMoveReceived);
    connect(m_logic, &PieceLogic::boardChanged, this, &gamewindow::onBoardChanged);

    if (m_gameMode == ONLINE_HOST) {
        m_networkHandler->startHosting();
        m_statusLabel->setText("Ожидание подключения оппонента...");
    } else if (m_gameMode == ONLINE_CLIENT) {
        m_networkHandler->connectToHost(peerIp);
        m_statusLabel->setText("Подключение к " + peerIp + "...");
    }
}

void gamewindow::onConnected() {
    updateStatusLabel();
    if (m_gameMode == ONLINE_HOST) {
        m_logic->setupNewGame();
        m_myColor = (QRandomGenerator::global()->bounded(2) == 0) ? WHITE : BLACK;
        std::array<Piece, 64> boardState;
        std::copy(&m_logic->getPieceAt(0,0), &m_logic->getPieceAt(0,0) + 64, boardState.begin());
        m_networkHandler->sendGameStart(boardState, m_myColor);
    }
}

void gamewindow::onDisconnected() {
    QMessageBox::warning(this, "Соединение разорвано", "Оппонент отключился.");
    updateStatusLabel();
}

void gamewindow::onNetworkError(const QString& error) {
    QMessageBox::critical(this, "Сетевая ошибка", error);
    this->close();
}

void gamewindow::onGameStartReceived(const std::array<Piece, 64>& board, PieceColor myColor) {
    m_myColor = myColor;
    m_logic->loadGame(board, WHITE);
    updateStatusLabel();
}

void gamewindow::onMoveReceived(const Move& move, PieceType promotionType) {
    if (m_logic->tryMove(move.fromRow, move.fromCol, move.toRow, move.toCol)) {
        if (promotionType != NONE) {
            m_logic->promotePawn(move.toRow, move.toCol, promotionType);
        }
    }
}

void gamewindow::handleCellClick(int row, int col) {
    if (m_gameMode == LOCAL) {
        if (m_logic->getGameStatus() != IN_PROGRESS) {
            m_selectedRow = -1; m_selectedCol = -1;
            updateBoardUI(m_logic->browseHistory(0));
            return;
        }
        if (m_selectedRow == -1) {
            if (m_logic->getPieceAt(row, col).color == m_logic->getCurrentTurn()) {
                m_selectedRow = row; m_selectedCol = col;
                updateBoardUI();
                highlightValidMoves();
            }
        } else {
            if (m_logic->tryMove(m_selectedRow, m_selectedCol, row, col)) {
                // Логика нотации и сообщений о мате/пате
            }
            m_selectedRow = -1; m_selectedCol = -1;
            updateBoardUI();
        }
    } else {
        if (m_logic->getGameStatus() != IN_PROGRESS || m_logic->getCurrentTurn() != m_myColor || !m_networkHandler->isConnected()) {
            return;
        }
        if (m_selectedRow == -1) {
            if (m_logic->getPieceAt(row, col).color == m_myColor) {
                m_selectedRow = row; m_selectedCol = col;
                updateBoardUI();
                highlightValidMoves();
            }
        } else {
            Move move = {m_selectedRow, m_selectedCol, row, col};
            if (m_logic->isMoveValid(move)) {
                PieceType promotionType = NONE;
                Piece movingPiece = m_logic->getPieceAt(move.fromRow, move.fromCol);
                if (movingPiece.type == PAWN && (move.toRow == 0 || move.toRow == 7)) {
                    PromotionDialog dialog(m_myColor, this);
                    promotionType = (dialog.exec() == QDialog::Accepted) ? dialog.selectedType : QUEEN;
                }
                m_networkHandler->sendMove(move, promotionType);
                if(m_logic->tryMove(move.fromRow, move.fromCol, move.toRow, move.toCol)) {
                    if (promotionType != NONE) {
                        m_logic->promotePawn(move.toRow, move.toCol, promotionType);
                    }
                }
            }
            m_selectedRow = -1; m_selectedCol = -1;
            updateBoardUI();
        }
    }
}

void gamewindow::onBoardChanged() {
    updateBoardUI(nullptr);
    updateStatusLabel();
}

void gamewindow::updateStatusLabel() {
    if (m_gameMode != LOCAL && (!m_networkHandler || !m_networkHandler->isConnected())) {
        m_statusLabel->setText("Отключено");
        return;
    }
    GameStatus status = m_logic->getGameStatus();
    if (status == CHECKMATE) {
        QString winner = (m_logic->getCurrentTurn() == WHITE) ? "Черные" : "Белые";
        m_statusLabel->setText("Мат! " + winner + " победили!");
        return;
    }
    if (status == STALEMATE) {
        m_statusLabel->setText("Пат! Ничья.");
        return;
    }
    if (m_gameMode == LOCAL) {
        QString turn = (m_logic->getCurrentTurn() == WHITE) ? "Белых" : "Черных";
        m_statusLabel->setText("Ход " + turn);
        m_statusLabel->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    } else {
        if (m_logic->getCurrentTurn() == m_myColor) {
            m_statusLabel->setText("Ваш ход");
            m_statusLabel->setStyleSheet("color: #66ff66; font-size: 16px; font-weight: bold;");
        } else {
            m_statusLabel->setText("Ход оппонента");
            m_statusLabel->setStyleSheet("color: #ffcc66; font-size: 16px; font-weight: bold;");
        }
    }
}

// ... Оставшиеся методы (onNewGameClicked, onBackToMenuClicked, updateBoardUI, и т.д.)
// остаются такими же, как в вашей изначальной версии.
// Я привожу их здесь для полноты файла.

void gamewindow::onNewGameClicked() {
    m_logic->setupNewGame();
    m_logic->resetHistoryBrowser();
    m_moveHistory->clear();
    m_selectedRow = -1;
    m_selectedCol = -1;
}

void gamewindow::onBackToMenuClicked() {
    if (m_networkHandler) {
        m_networkHandler->disconnect(); // Разрываем соединение при выходе
    }
    emit menuRequested();
    this->close();
}

void gamewindow::onPrevMoveClicked() {
    const Piece* historyBoard = m_logic->browseHistory(-1);
    if (historyBoard) {
        updateBoardUI(historyBoard);
    }
}

void gamewindow::onNextMoveClicked() {
    const Piece* historyBoard = m_logic->browseHistory(1);
    if (historyBoard) {
        updateBoardUI(historyBoard);
    }
}

void gamewindow::handlePromotion(int row, int col, PieceColor color) {
    PromotionDialog dialog(color, this);
    if (dialog.exec() == QDialog::Accepted) {
        m_logic->promotePawn(row, col, dialog.selectedType);
    } else {
        m_logic->promotePawn(row, col, QUEEN);
    }
}

void gamewindow::updateBoardUI(const Piece* boardState) {
    clearHighlights();
    bool isBrowsingHistory = (boardState != nullptr);
    for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) {
            Piece p = isBrowsingHistory ? boardState[r * 8 + c] : m_logic->getPieceAt(r, c);
            m_boardCells[r][c]->setPixmap(p.type != NONE ? QPixmap(getPieceImagePath(p)).scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation) : QPixmap());
        }
    if (!isBrowsingHistory) {
        const int maxCols = 5;
        clearLayout(m_whiteCapturedLayout);
        const auto& whiteCaptured = m_logic->getCapturedPieces(WHITE);
        for (size_t i = 0; i < whiteCaptured.size(); ++i) {
            QLabel* capturedLabel = new QLabel();
            capturedLabel->setFixedSize(35, 35);
            capturedLabel->setPixmap(QPixmap(getPieceImagePath(whiteCaptured[i])).scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            m_whiteCapturedLayout->addWidget(capturedLabel, i / maxCols, i % maxCols);
        }
        clearLayout(m_blackCapturedLayout);
        const auto& blackCaptured = m_logic->getCapturedPieces(BLACK);
        for (size_t i = 0; i < blackCaptured.size(); ++i) {
            QLabel* capturedLabel = new QLabel();
            capturedLabel->setFixedSize(35, 35);
            capturedLabel->setPixmap(QPixmap(getPieceImagePath(blackCaptured[i])).scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            m_blackCapturedLayout->addWidget(capturedLabel, i / maxCols, i % maxCols);
        }
        if (m_logic->isKingInCheck(m_logic->getCurrentTurn())) {
            for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) {
                    Piece p = m_logic->getPieceAt(r, c);
                    if (p.type == KING && p.color == m_logic->getCurrentTurn()) {
                        m_boardCells[r][c]->setStyleSheet("background-color: #ff6666;");
                        break;
                    }
                }
        }
    }
    bool isGameOver = m_logic->getGameStatus() != IN_PROGRESS;
    bool showHistoryButtons = isGameOver && (m_gameMode == LOCAL);
    m_prevMoveButton->setVisible(showHistoryButtons);
    m_nextMoveButton->setVisible(showHistoryButtons);
    if (showHistoryButtons) {
        m_prevMoveButton->setEnabled(m_logic->getCurrentHistoryIndex() > 0);
        m_nextMoveButton->setEnabled(m_logic->getCurrentHistoryIndex() < m_logic->getHistorySize() - 1);
    }
}

void gamewindow::clearHighlights() {
    for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) {
            m_boardCells[r][c]->setStyleSheet((r + c) % 2 == 0 ? "background-color: #f0d9b5;" : "background-color: #b58863;");
        }
}

void gamewindow::highlightValidMoves() {
    if(m_selectedRow == -1) return;
    QString currentStyle = m_boardCells[m_selectedRow][m_selectedCol]->styleSheet();
    m_boardCells[m_selectedRow][m_selectedCol]->setStyleSheet(currentStyle + " border: 3px solid #6699ff;");
    std::vector<Move> moves = m_logic->getValidMovesForPiece(m_selectedRow, m_selectedCol);
    for (const auto& move : moves) {
        QString baseStyle = m_boardCells[move.toRow][move.toCol]->styleSheet();
        if (m_logic->getPieceAt(move.toRow, move.toCol).type != NONE) {
            baseStyle += " border: 4px solid #cc3333;";
        } else {
            baseStyle += " border: 4px solid #66cc66;";
        }
        m_boardCells[move.toRow][move.toCol]->setStyleSheet(baseStyle);
    }
}

QString gamewindow::getPieceImagePath(const Piece& piece) {
    if (piece.type == NONE) return "";
    QMap<PieceType, QChar> typeMap = { {KING, 'k'}, {QUEEN, 'q'}, {ROOK, 'r'}, {BISHOP, 'b'}, {KNIGHT, 'n'}, {PAWN, 'p'} };
    char colorChar = (piece.color == WHITE) ? 'w' : 'b';
    return QString(":/new/prefix1/pieces600/%1%2.png").arg(colorChar).arg(typeMap[piece.type]);
}

void gamewindow::clearLayout(QLayout* layout) {
    if (!layout) return;
    QLayoutItem *item;
    while((item = layout->takeAt(0)) != nullptr){
        if (item->widget()){
            delete item->widget();
        } else if (item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
        }
        delete item;
    }
}
