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
#include <QLineEdit>
#include <QLabel>

// Конструктор для локальной игры.
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

    connect(m_logic, &PieceLogic::boardChanged, this, &gamewindow::onBoardChanged);

    m_logic->setupNewGame();
    updateBoardUI();
}

// Конструктор для сетевой игры.
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
    // В сетевой игре кнопка "Новая игра" не имеет смысла, скрываем ее.
    findChild<QPushButton*>("newGameButton")->setVisible(false);

    connect(m_logic, &PieceLogic::boardChanged, this, &gamewindow::onBoardChanged);
    connect(m_networkManager, &NetworkManager::moveReceived, this, &gamewindow::onMoveReceived);
    connect(m_networkManager, &NetworkManager::opponentDisconnected, this, &gamewindow::onOpponentDisconnected);

    // Подписка на чат и обработчики отправки
    connect(m_networkManager, &NetworkManager::chatReceived, this, &gamewindow::onChatReceived);
    if (m_sendChatButton) {
        connect(m_sendChatButton, &QPushButton::clicked, this, &gamewindow::onSendChatClicked);
    }
    if (m_chatInput) {
        connect(m_chatInput, &QLineEdit::returnPressed, this, &gamewindow::onSendChatClicked);
    }

    m_logic->setBoardFromLayout(initialLayout);
}

// Создание и компоновка всех элементов интерфейса.
void gamewindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // ==== ЛЕВАЯ ПАНЕЛЬ ====
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

    // ==== ДОСКА ====
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

    // ==== ПРАВАЯ ПАНЕЛЬ (история ходов + чат (только сеть)) ====
    QWidget* rightPanel = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);

    m_moveHistory = new QTextEdit();
    m_moveHistory->setReadOnly(true);
    m_moveHistory->setStyleSheet("background-color: #3c3c3c; color: white; border: 1px solid gray;");
    m_moveHistory->setMinimumWidth(260);

    rightLayout->addWidget(new QLabel("История ходов"));
    static_cast<QLabel*>(rightLayout->itemAt(rightLayout->count()-1)->widget())->setStyleSheet("color: white; font-weight: bold;");
    rightLayout->addWidget(m_moveHistory);

    if (m_isNetworkGame) {
        QLabel* chatLabel = new QLabel("Чат");
        chatLabel->setStyleSheet("color: white; font-weight: bold;");
        rightLayout->addWidget(chatLabel);

        m_chatHistory = new QTextEdit();
        m_chatHistory->setReadOnly(true);
        m_chatHistory->setStyleSheet("background-color: #2f2f2f; color: white; border: 1px solid gray;");

        rightLayout->addWidget(m_chatHistory, /*stretch*/1);

        QHBoxLayout* chatInputLayout = new QHBoxLayout();
        m_chatInput = new QLineEdit();
        m_chatInput->setPlaceholderText("Напишите сообщение…");
        m_sendChatButton = new QPushButton("Отправить");

        QString sendBtnStyle = R"(
            QPushButton {
                background-color: #4a7bd1; color: white; border: 1px solid #3a6bbf;
                padding: 6px 10px; font-weight: bold; border-radius: 4px;
            }
            QPushButton:hover { background-color: #5a8be1; }
            QPushButton:pressed { background-color: #375ea3; }
        )";
        m_sendChatButton->setStyleSheet(sendBtnStyle);

        chatInputLayout->addWidget(m_chatInput, 1);
        chatInputLayout->addWidget(m_sendChatButton);

        rightLayout->addLayout(chatInputLayout);
    } else {
        rightLayout->addStretch(1);
    }

    // ==== Сборка в главный лэйаут ====
    mainLayout->addWidget(leftPanelWidget);
    mainLayout->addStretch(1);
    mainLayout->addWidget(boardContainer);
    mainLayout->addStretch(1);
    mainLayout->addWidget(rightPanel);
}

// Основной обработчик кликов по доске.
void gamewindow::handleCellClick(int row, int col)
{
    // Блокировка хода, если сейчас не наш черед в сетевой игре.
    if (m_isNetworkGame && m_logic->getCurrentTurn() != m_myColor) {
        return;
    }
    // Если игра завершена, клики больше не обрабатываются.
    if (m_logic->getGameStatus() != IN_PROGRESS) {
        m_selectedRow = -1;
        m_selectedCol = -1;
        updateBoardUI(m_logic->browseHistory(0));
        return;
    }
    // Первый клик: выбор фигуры.
    if (m_selectedRow == -1) {
        if (m_logic->getPieceAt(row, col).color == m_logic->getCurrentTurn()) {
            m_selectedRow = row;
            m_selectedCol = col;
            updateBoardUI();
            highlightValidMoves();
        }
    } else { // Второй клик: совершение хода.
        Move currentMove = {m_selectedRow, m_selectedCol, row, col};
        Piece movingPiece = m_logic->getPieceAt(m_selectedRow, m_selectedCol);

        // Если это ход-превращение, запрашиваем у пользователя выбор фигуры.
        int promotionRow = (movingPiece.color == WHITE) ? 0 : 7;
        if (movingPiece.type == PAWN && row == promotionRow) {
            PromotionDialog dialog(movingPiece.color, this);
            if (dialog.exec() == QDialog::Accepted) {
                currentMove.promotion = dialog.selectedType;
            } else {
                currentMove.promotion = QUEEN; // По умолчанию ферзь.
            }
        }

        // Пытаемся совершить ход в логике.
        if (m_logic->tryMove(currentMove)) {
            // Формируем и добавляем нотацию в историю.
            QString fromStr = QChar('a' + m_selectedCol) + QString::number(8 - m_selectedRow);
            QString toStr = QChar('a' + col) + QString::number(8 - row);
            QMap<PieceType, QString> pieceNames = { {PAWN, "Pawn"}, {KNIGHT, "Knight"}, {BISHOP, "Bishop"}, {ROOK, "Rook"}, {QUEEN, "Queen"}, {KING, "King"} };
            QString colorStr = (movingPiece.color == WHITE) ? "White" : "Black";
            QString notation = QString("%1-%2 (%3 %4)").arg(fromStr, toStr, colorStr, pieceNames[movingPiece.type]);
            m_moveHistory->append(notation);

            // Если игра сетевая, отправляем ход оппоненту.
            if (m_isNetworkGame) {
                m_networkManager->sendMove(currentMove);
            }
        }

        // Сбрасываем выделение.
        m_selectedRow = -1;
        m_selectedCol = -1;

        // Проверяем, не закончилась ли игра.
        checkAndDisplayGameEndStatus();
    }
}

// Слот для обработки хода, полученного от оппонента по сети.
void gamewindow::onMoveReceived(const Move& move)
{
    // Добавляем нотацию полученного хода.
    Piece movingPiece = m_logic->getPieceAt(move.fromRow, move.fromCol);
    if (movingPiece.type != NONE) {
        QString fromStr = QChar('a' + move.fromCol) + QString::number(8 - move.fromRow);
        QString toStr = QChar('a' + move.toCol) + QString::number(8 - move.toRow);
        QMap<PieceType, QString> pieceNames = { {PAWN, "Pawn"}, {KNIGHT, "Knight"}, {BISHOP, "Bishop"}, {ROOK, "Rook"}, {QUEEN, "Queen"}, {KING, "King"} };
        QString colorStr = (movingPiece.color == WHITE) ? "White" : "Black";
        m_moveHistory->append(QString("%1-%2 (%3 %4)").arg(fromStr, toStr, colorStr, pieceNames[movingPiece.type]));
    }

    // Применяем ход к нашей локальной логике.
    m_logic->tryMove(move);

    // Проверяем, не закончилась ли игра после хода оппонента.
    checkAndDisplayGameEndStatus();
}

// Слот, вызываемый при разрыве соединения.
void gamewindow::onOpponentDisconnected()
{
    // Если игра была в процессе, объявляем техническую победу.
    if (m_logic->getGameStatus() == IN_PROGRESS) {
        QMessageBox::information(this, "Игра окончена", "Техническая победа! Ваш оппонент отключился.");
        // Принудительно завершаем игру, чтобы включить просмотр истории.
        m_logic->forceEndGame();
    }
    // Если игра уже была завершена (мат/пат), ничего не делаем, чтобы избежать дублирующих сообщений.
}

// Приём входящих сообщений чата.
void gamewindow::onChatReceived(const QString &message)
{
    if (m_chatHistory) {
        m_chatHistory->append(QString("<b>Оппонент:</b> %1").arg(message.toHtmlEscaped()));
    }
}

// Отправка сообщения чата.
void gamewindow::onSendChatClicked()
{
    if (!m_isNetworkGame || !m_networkManager || !m_chatInput || !m_chatHistory) return;

    const QString text = m_chatInput->text().trimmed();
    if (text.isEmpty()) return;

    m_networkManager->sendChatMessage(text);
    m_chatHistory->append(QString("<b>Вы:</b> %1").arg(text.toHtmlEscaped()));
    m_chatInput->clear();
}

// Централизованная проверка и отображение окончания игры.
void gamewindow::checkAndDisplayGameEndStatus()
{
    GameStatus status = m_logic->getGameStatus();
    if (status != IN_PROGRESS) {
        QString message;
        if (status == CHECKMATE) {
            QString winner = (m_logic->getCurrentTurn() == WHITE) ? "Черные" : "Белые";
            message = "Мат! " + winner + " победили.";
        } else if (status == STALEMATE) {
            message = "Пат! Ничья.";
        }
        QMessageBox::information(this, "Игра окончена", message);
    }
}

// Слот, вызываемый сигналом boardChanged от логики.
void gamewindow::onBoardChanged() {
    updateBoardUI(nullptr);
}

// Начинает новую партию в локальном режиме.
void gamewindow::onNewGameClicked() {
    m_logic->setupNewGame();
    m_logic->resetHistoryBrowser();
    m_moveHistory->clear();
    m_selectedRow = -1;
    m_selectedCol = -1;
}

// Закрывает игровое окно и возвращает в главное меню.
void gamewindow::onBackToMenuClicked() {
    emit menuRequested();
    this->close();
}

// Навигация по истории ходов.
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

// Полностью перерисовывает доску и элементы интерфейса.
void gamewindow::updateBoardUI(const Piece* boardState) {
    clearHighlights();

    bool isBrowsingHistory = (boardState != nullptr);

    // Расставляем фигуры
    for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) {
            Piece p = isBrowsingHistory ? boardState[r * 8 + c] : m_logic->getPieceAt(r, c);
            m_boardCells[r][c]->setPixmap(p.type != NONE ? QPixmap(getPieceImagePath(p)).scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation) : QPixmap());
        }

    if (!isBrowsingHistory) {
        // Обновляем панели съеденных фигур
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
        // Подсвечиваем короля под шахом
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

    // Показываем/скрываем кнопки навигации по истории
    bool isGameOver = m_logic->getGameStatus() != IN_PROGRESS;
    m_prevMoveButton->setVisible(isGameOver);
    m_nextMoveButton->setVisible(isGameOver);

    if (isGameOver) {
        m_prevMoveButton->setEnabled(m_logic->getCurrentHistoryIndex() > 0);
        m_nextMoveButton->setEnabled(m_logic->getCurrentHistoryIndex() < m_logic->getHistorySize() - 1);
    }
}

// Сбрасывает все подсветки на доске.
void gamewindow::clearHighlights() {
    for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) {
            m_boardCells[r][c]->setStyleSheet((r + c) % 2 == 0 ? "background-color: #f0d9b5;" : "background-color: #b58863;");
        }
}

// Подсвечивает доступные ходы для выбранной фигуры.
void gamewindow::highlightValidMoves() {
    if(m_selectedRow == -1) return;
    // Подсвечиваем саму выбранную фигуру
    QString currentStyle = m_boardCells[m_selectedRow][m_selectedCol]->styleSheet();
    m_boardCells[m_selectedRow][m_selectedCol]->setStyleSheet(currentStyle + " border: 3px solid #6699ff;");

    Piece movingPiece = m_logic->getPieceAt(m_selectedRow, m_selectedCol);
    std::vector<Move> moves = m_logic->getValidMovesForPiece(m_selectedRow, m_selectedCol);

    for (const auto& move : moves) {
        QString baseStyle = m_boardCells[move.toRow][move.toCol]->styleSheet();
        Piece targetPiece = m_logic->getPieceAt(move.toRow, move.toCol);

        // Проверяем, является ли ход рокировкой.
        bool isCastleMove = (movingPiece.type == KING && targetPiece.type == ROOK && movingPiece.color == targetPiece.color);

        if (isCastleMove) {
            baseStyle += " border: 4px solid #ffcc00;"; // Желтый для рокировки.
        } else if (targetPiece.type != NONE) {
            baseStyle += " border: 4px solid #cc3333;"; // Красный для взятия.
        } else {
            baseStyle += " border: 4px solid #66cc66;"; // Зеленый для обычного хода.
        }
        m_boardCells[move.toRow][move.toCol]->setStyleSheet(baseStyle);
    }
}

// Возвращает путь к изображению фигуры.
QString gamewindow::getPieceImagePath(const Piece& piece) {
    if (piece.type == NONE) return "";
    QMap<PieceType, QChar> typeMap = { {KING, 'k'}, {QUEEN, 'q'}, {ROOK, 'r'}, {BISHOP, 'b'}, {KNIGHT, 'n'}, {PAWN, 'p'} };
    char colorChar = (piece.color == WHITE) ? 'w' : 'b';
    return QString(":/new/prefix1/pieces600/%1%2.png").arg(colorChar).arg(typeMap[piece.type]);
}

// Рекурсивно очищает все виджеты из layout.
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
