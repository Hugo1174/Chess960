#include "gamewindow.h"
#include "promotiondialog.h"
#include "networkmanager.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QMessageBox>
#include <QMap>
#include <QFont>
#include <QPushButton>


gamewindow::gamewindow(QWidget *parent)
    : QMainWindow(parent), m_logic(new PieceLogic(this)), m_networkManager(nullptr), m_isNetworkGame(false), m_myColor(NO_COLOR)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Chess960 - Локальная игра");
    setMinimumSize(1280, 720);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(45, 45, 45));
    setAutoFillBackground(true);
    setPalette(pal);

    setupUI();

    // Сначала подключаем сигналы
    connect(m_logic, &PieceLogic::boardChanged, this, &gamewindow::onBoardChanged);
    connect(m_logic, &PieceLogic::promotionRequired, this, &gamewindow::handlePromotion);

    // А потом инициализируем игру
    m_logic->setupNewGame();

    updateBoardUI();
}


gamewindow::gamewindow(NetworkManager *manager, const QString& initialLayout, PieceColor myColor, QWidget *parent)
    : QMainWindow(parent), m_logic(new PieceLogic(this)), m_networkManager(manager), m_isNetworkGame(true), m_myColor(myColor)
{
    m_networkManager->setParent(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Chess960 - Сетевая игра");
    setMinimumSize(1280, 720);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(45, 45, 45));
    setAutoFillBackground(true);
    setPalette(pal);

    setupUI();
    findChild<QPushButton*>("newGameButton")->setEnabled(false);

    // 1. Сначала подключаем все сигналы
    connect(m_logic, &PieceLogic::boardChanged, this, &gamewindow::onBoardChanged);
    connect(m_logic, &PieceLogic::promotionRequired, this, &gamewindow::handlePromotion);
    connect(m_networkManager, &NetworkManager::moveReceived, this, &gamewindow::onMoveReceived);
    connect(m_networkManager, &NetworkManager::promotionChoiceReceived, this, &gamewindow::onPromotionReceived);
    connect(m_networkManager, &NetworkManager::opponentDisconnected, this, &gamewindow::onOpponentDisconnected);

    // 2. И только потом устанавливаем доску, что вызовет сигнал boardChanged()
    m_logic->setBoardFromLayout(initialLayout);
}


void gamewindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    QVBoxLayout *leftPanelLayout = new QVBoxLayout();
    QWidget* leftPanelWidget = new QWidget();

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QPushButton* newGameButton = new QPushButton("Новая игра");
    newGameButton->setObjectName("newGameButton");
    QPushButton* backToMenuButton = new QPushButton("В меню");

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

void gamewindow::handleCellClick(int row, int col)
{
    if (m_isNetworkGame && m_logic->getCurrentTurn() != m_myColor) {
        return;
    }

    if (m_logic->getGameStatus() != IN_PROGRESS) {
        m_selectedRow = -1;
        m_selectedCol = -1;
        updateBoardUI(m_logic->browseHistory(0));
        return;
    }
    if (m_selectedRow == -1) {
        if (m_logic->getPieceAt(row, col).color == m_logic->getCurrentTurn()) {
            m_selectedRow = row;
            m_selectedCol = col;
            updateBoardUI();
            highlightValidMoves();
        }
    } else {
        Move currentMove = {m_selectedRow, m_selectedCol, row, col};

        QString notation = "";
        Piece movingPiece = m_logic->getPieceAt(m_selectedRow, m_selectedCol);
        if (movingPiece.type != NONE) {
            QString fromStr = QChar('a' + m_selectedCol) + QString::number(8 - m_selectedRow);
            QString toStr = QChar('a' + col) + QString::number(8 - row);
            QMap<PieceType, QString> pieceNames = { {PAWN, "Pawn"}, {KNIGHT, "Knight"}, {BISHOP, "Bishop"}, {ROOK, "Rook"}, {QUEEN, "Queen"}, {KING, "King"} };
            QString colorStr = (movingPiece.color == WHITE) ? "White" : "Black";
            notation = QString("%1-%2 (%3 %4)").arg(fromStr, toStr, colorStr, pieceNames[movingPiece.type]);
        }

        if (m_logic->tryMove(m_selectedRow, m_selectedCol, row, col)) {
            if (!notation.isEmpty()) {
                m_moveHistory->append(notation);
            }

            if (m_isNetworkGame) {
                m_networkManager->sendMove(currentMove);
            }

            m_selectedRow = -1;
            m_selectedCol = -1;
            GameStatus status = m_logic->getGameStatus();
            if (status != IN_PROGRESS) {
                onBoardChanged();
                if (status == CHECKMATE) {
                    QMessageBox::information(this, "Игра окончена", "Мат! " + QString(m_logic->getCurrentTurn() == WHITE ? "Черные" : "Белые") + " победили.");
                } else if (status == STALEMATE) {
                    QMessageBox::information(this, "Игра окончена", "Пат! Ничья.");
                }
            }
        } else {
            m_selectedRow = -1;
            m_selectedCol = -1;
            updateBoardUI();
        }
    }
}

void gamewindow::handlePromotion(int row, int col, PieceColor color)
{
    if (!m_isNetworkGame || color == m_myColor) {
        PromotionDialog dialog(color, this);
        if (dialog.exec() == QDialog::Accepted) {
            PieceType selected = dialog.selectedType;
            m_logic->promotePawn(row, col, selected);
            if (m_isNetworkGame) {
                m_networkManager->sendPromotionChoice(selected);
            }
        } else {
            m_logic->promotePawn(row, col, QUEEN);
            if (m_isNetworkGame) {
                m_networkManager->sendPromotionChoice(QUEEN);
            }
        }
    }
}


void gamewindow::onMoveReceived(const Move& move)
{
    // Генерируем нотацию для хода оппонента
    QString notation = "";
    Piece movingPiece = m_logic->getPieceAt(move.fromRow, move.fromCol);
    if (movingPiece.type != NONE) {
        QString fromStr = QChar('a' + move.fromCol) + QString::number(8 - move.fromRow);
        QString toStr = QChar('a' + move.toCol) + QString::number(8 - move.toRow);
        QMap<PieceType, QString> pieceNames = { {PAWN, "Pawn"}, {KNIGHT, "Knight"}, {BISHOP, "Bishop"}, {ROOK, "Rook"}, {QUEEN, "Queen"}, {KING, "King"} };
        QString colorStr = (movingPiece.color == WHITE) ? "White" : "Black";
        notation = QString("%1-%2 (%3 %4)").arg(fromStr, toStr, colorStr, pieceNames[movingPiece.type]);
    }
    // Добавляем ее в историю
    if (!notation.isEmpty()) {
        m_moveHistory->append(notation);
    }

    // Применяем ход к нашей логике
    m_logic->tryMove(move.fromRow, move.fromCol, move.toRow, move.toCol);
}


void gamewindow::onPromotionReceived(PieceType type)
{

    PieceColor promoterColor = m_logic->getCurrentTurn();
    int promotionRow = (promoterColor == WHITE) ? 0 : 7;

    // Ищем пешку оппонента на линии превращения, чтобы передать ее координаты в promotePawn
    for (int col = 0; col < 8; ++col) {
        Piece p = m_logic->getPieceAt(promotionRow, col);
        // Мы ищем пешку цвета игрока, чей сейчас ход (т.е. оппонента)
        if (p.type == PAWN && p.color == promoterColor) {
            m_logic->promotePawn(promotionRow, col, type);
            break; // Нашли и обработали, выходим из цикла
        }
    }
}

void gamewindow::onOpponentDisconnected()
{
    QMessageBox::warning(this, "Соединение разорвано", "Ваш оппонент отключился. Игра окончена.");
    centralWidget()->setEnabled(false);
}

void gamewindow::onBoardChanged() {
    updateBoardUI(nullptr);
}

void gamewindow::onNewGameClicked() {
    m_logic->setupNewGame();
    m_logic->resetHistoryBrowser();
    m_moveHistory->clear();
    m_selectedRow = -1;
    m_selectedCol = -1;
}

void gamewindow::onBackToMenuClicked() {
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
    m_prevMoveButton->setVisible(isGameOver);
    m_nextMoveButton->setVisible(isGameOver);

    if (isGameOver) {
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
    QLayoutItem *item;
    while((item = layout->takeAt(0)) != nullptr){
        if (item->widget()){
            delete item->widget();
        } else if (item->layout()) {
            clearLayout(item->layout());
        }
        delete item;
    }
}
