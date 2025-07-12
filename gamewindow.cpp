// gamewindow.cpp
// Реализация главного игрового окна.

#include "gamewindow.h"
#include "promotiondialog.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QMessageBox>
#include <QMap>
#include <QFont>
#include <QPushButton>

gamewindow::gamewindow(QWidget *parent)
    : QMainWindow(parent), m_logic(new PieceLogic(this))
{
    // Важный атрибут, чтобы MainWindow мог безопасно удалить окно.
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Chess960 - Игра");
    setMinimumSize(1280, 720);

    // Установка темного фона окна.
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(45, 45, 45));
    setAutoFillBackground(true);
    setPalette(pal);

    // Создание всего UI.
    setupUI();

    // Связывание сигналов от логики со слотами этого окна (ключевой момент MVC).
    connect(m_logic, &PieceLogic::boardChanged, this, &gamewindow::onBoardChanged);
    connect(m_logic, &PieceLogic::promotionRequired, this, &gamewindow::handlePromotion);

    // Первоначальная отрисовка доски.
    updateBoardUI();
}

void gamewindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // --- Левая панель (кнопки + съеденные фигуры) ---
    QVBoxLayout *leftPanelLayout = new QVBoxLayout();
    QWidget* leftPanelWidget = new QWidget();

    // Кнопки управления.
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QPushButton* newGameButton = new QPushButton("Новая игра");
    QPushButton* backToMenuButton = new QPushButton("В меню");

    // Задание единого стиля для кнопок.
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

    // Панели съеденных фигур.
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

    // Кнопки истории (становятся видимы по окончании игры).
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

    // --- Доска с нумерацией ---
    QGridLayout *boardLayout = new QGridLayout();
    boardLayout->setSpacing(0);
    // Добавление букв и цифр по краям доски.
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
        // Создание 64 интерактивных клеток.
        for (int c = 0; c < 8; ++c) {
            ClickableLabel* cell = new ClickableLabel();
            cell->setFixedSize(90, 90); cell->setAlignment(Qt::AlignCenter);
            cell->row = r; cell->col = c; // Сохраняем координаты в самой клетке.
            connect(cell, &ClickableLabel::clicked, this, &gamewindow::handleCellClick);
            boardLayout->addWidget(cell, r + 1, c + 1);
            m_boardCells[r][c] = cell;
        }
    }
    QWidget* boardContainer = new QWidget();
    boardContainer->setLayout(boardLayout);
    boardContainer->setFixedSize(boardLayout->sizeHint());

    // --- История ходов (справа) ---
    m_moveHistory = new QTextEdit();
    m_moveHistory->setReadOnly(true);
    m_moveHistory->setStyleSheet("background-color: #3c3c3c; color: white; border: 1px solid gray;");
    m_moveHistory->setMinimumWidth(200);

    // --- Финальная верстка главного окна ---
    mainLayout->addWidget(leftPanelWidget);
    mainLayout->addStretch(1);
    mainLayout->addWidget(boardContainer);
    mainLayout->addStretch(1);
    mainLayout->addWidget(m_moveHistory);
}

void gamewindow::onBoardChanged() {
    // Просто вызываем полную перерисовку UI.
    updateBoardUI(nullptr);
}

void gamewindow::onNewGameClicked() {
    m_logic->setupNewGame();
    m_logic->resetHistoryBrowser();
    m_moveHistory->clear();
    // Сброс состояния UI.
    m_selectedRow = -1;
    m_selectedCol = -1;
    // onBoardChanged будет вызван сигналом из setupNewGame.
}

void gamewindow::onBackToMenuClicked() {
    emit menuRequested(); // Сообщаем главному меню о своем закрытии.
    this->close();
}

void gamewindow::onPrevMoveClicked() {
    const Piece* historyBoard = m_logic->browseHistory(-1);
    if (historyBoard) {
        // Перерисовываем UI, используя историческое состояние доски.
        updateBoardUI(historyBoard);
    }
}

void gamewindow::onNextMoveClicked() {
    const Piece* historyBoard = m_logic->browseHistory(1);
    if (historyBoard) {
        updateBoardUI(historyBoard);
    }
}

void gamewindow::handleCellClick(int row, int col) {
    // Если игра окончена, но мы в режиме просмотра истории,
    // клик сбрасывает просмотр на последнюю позицию.
    if (m_logic->getGameStatus() != IN_PROGRESS) {
        m_selectedRow = -1;
        m_selectedCol = -1;
        updateBoardUI(m_logic->browseHistory(0));
        return;
    }

    // --- Логика двух кликов: "откуда" -> "куда" ---

    // Этап 1: Фигура еще не выбрана.
    if (m_selectedRow == -1) {
        // Пытаемся выбрать фигуру, если на клетке есть фигура текущего игрока.
        if (m_logic->getPieceAt(row, col).color == m_logic->getCurrentTurn()) {
            m_selectedRow = row;
            m_selectedCol = col;
            updateBoardUI(); // Перерисовываем, чтобы убрать старые подсветки.
            highlightValidMoves(); // Показываем возможные ходы.
        }
    }
    // Этап 2: Фигура уже выбрана, это второй клик.
    else {
        // Формирование простой алгебраической нотации для истории.
        QString notation = "";
        if (m_logic->getPieceAt(m_selectedRow, m_selectedCol).type != NONE) {
            Piece movingPiece = m_logic->getPieceAt(m_selectedRow, m_selectedCol);
            QString fromStr = QChar('a' + m_selectedCol) + QString::number(8 - m_selectedRow);
            QString toStr = QChar('a' + col) + QString::number(8 - row);
            QMap<PieceType, QString> pieceNames = { {PAWN, "Pawn"}, {KNIGHT, "Knight"}, {BISHOP, "Bishop"}, {ROOK, "Rook"}, {QUEEN, "Queen"}, {KING, "King"} };
            QString colorStr = (movingPiece.color == WHITE) ? "White" : "Black";
            notation = QString("%1-%2 (%3 %4)").arg(fromStr, toStr, colorStr, pieceNames[movingPiece.type]);
        }

        // Пытаемся совершить ход через ядро логики.
        if (m_logic->tryMove(m_selectedRow, m_selectedCol, row, col)) {
            // Ход успешен.
            if (!notation.isEmpty()) { m_moveHistory->append(notation); }
            m_selectedRow = -1; m_selectedCol = -1;

            // Проверяем, не закончилась ли игра после этого хода.
            GameStatus status = m_logic->getGameStatus();
            if (status != IN_PROGRESS) {
                onBoardChanged(); // Обновляем доску, чтобы показать финальную позицию.
                if (status == CHECKMATE) {
                    QMessageBox::information(this, "Игра окончена", "Мат! " + QString(m_logic->getCurrentTurn() == WHITE ? "Черные" : "Белые") + " победили.");
                } else if (status == STALEMATE) {
                    QMessageBox::information(this, "Игра окончена", "Пат! Ничья.");
                }
            }
        } else {
            // Ход нелегален. Просто сбрасываем выделение.
            m_selectedRow = -1;
            m_selectedCol = -1;
            updateBoardUI();
        }
    }
}

void gamewindow::updateBoardUI(const Piece* boardState) {
    clearHighlights(); // Сначала убираем всю подсветку.

    // Если boardState передан, мы в режиме просмотра истории.
    bool isBrowsingHistory = (boardState != nullptr);

    // Расстановка фигур на доске.
    for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) {
            // Берем состояние либо из истории, либо из текущей логики.
            Piece p = isBrowsingHistory ? boardState[r * 8 + c] : m_logic->getPieceAt(r, c);
            if (p.type != NONE) {
                m_boardCells[r][c]->setPixmap(QPixmap(getPieceImagePath(p)).scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } else {
                m_boardCells[r][c]->setPixmap(QPixmap()); // Очищаем клетку.
            }
        }

    // Обновляем панели со съеденными фигурами и подсветку шаха только в реальной игре.
    if (!isBrowsingHistory) {
        const int maxCols = 5; // Максимум иконок в ряду.
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

        // Подсветка короля, если он под шахом.
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

    // Управление видимостью и доступностью кнопок истории.
    bool isGameOver = m_logic->getGameStatus() != IN_PROGRESS;
    m_prevMoveButton->setVisible(isGameOver);
    m_nextMoveButton->setVisible(isGameOver);

    if (isGameOver) {
        m_prevMoveButton->setEnabled(m_logic->getCurrentHistoryIndex() > 0);
        m_nextMoveButton->setEnabled(m_logic->getCurrentHistoryIndex() < m_logic->getHistorySize() - 1);
    }
}

void gamewindow::handlePromotion(int row, int col, PieceColor color) {
    PromotionDialog dialog(color, this);
    // dialog.exec() блокирует выполнение до закрытия диалога.
    if (dialog.exec() == QDialog::Accepted) {
        // Пользователь сделал выбор.
        m_logic->promotePawn(row, col, dialog.selectedType);
    } else {
        // Пользователь закрыл диалог. Превращаем в ферзя по умолчанию.
        m_logic->promotePawn(row, col, QUEEN);
    }
}

void gamewindow::clearHighlights() {
    // Восстанавливаем стандартную "шахматную" раскраску доски.
    for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) {
            m_boardCells[r][c]->setStyleSheet((r + c) % 2 == 0 ? "background-color: #f0d9b5;" : "background-color: #b58863;");
        }
}

void gamewindow::highlightValidMoves() {
    if(m_selectedRow == -1) return;

    // Подсвечиваем выбранную фигуру.
    QString currentStyle = m_boardCells[m_selectedRow][m_selectedCol]->styleSheet();
    m_boardCells[m_selectedRow][m_selectedCol]->setStyleSheet(currentStyle + " border: 3px solid #6699ff;");

    // Получаем легальные ходы из ядра логики.
    std::vector<Move> moves = m_logic->getValidMovesForPiece(m_selectedRow, m_selectedCol);
    for (const auto& move : moves) {
        QString baseStyle = m_boardCells[move.toRow][move.toCol]->styleSheet();
        // Разные цвета для обычного хода и для взятия.
        if (m_logic->getPieceAt(move.toRow, move.toCol).type != NONE) {
            baseStyle += " border: 4px solid #cc3333;"; // Красный - взятие.
        } else {
            baseStyle += " border: 4px solid #66cc66;"; // Зеленый - ход.
        }
        m_boardCells[move.toRow][move.toCol]->setStyleSheet(baseStyle);
    }
}

QString gamewindow::getPieceImagePath(const Piece& piece) {
    if (piece.type == NONE) return "";
    // Формируем имя файла из цвета и типа фигуры, например: 'w' + 'p' -> "wp.png".
    QMap<PieceType, QChar> typeMap = { {KING, 'k'}, {QUEEN, 'q'}, {ROOK, 'r'}, {BISHOP, 'b'}, {KNIGHT, 'n'}, {PAWN, 'p'} };
    char colorChar = (piece.color == WHITE) ? 'w' : 'b';
    return QString(":/new/prefix1/pieces600/%1%2.png").arg(colorChar).arg(typeMap[piece.type]);
}

void gamewindow::clearLayout(QLayout* layout) {
    // Корректная очистка layout для предотвращения утечек памяти.
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
