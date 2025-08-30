#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "clickablelabel.h"
#include "piece_logic.h"
#include <QMainWindow>
#include <QTextEdit>
#include <QGridLayout>

// Предварительные объявления для уменьшения зависимостей в заголовках.
class QPushButton;
class QLineEdit;
class NetworkManager;

/**
 * @class gamewindow
 * @brief Основное игровое окно. Выступает в роли Контроллера и Представления.
 *
 * Управляет отображением доски, обработкой ввода пользователя,
 * и взаимодействием между игровой логикой (PieceLogic) и сетью (NetworkManager).
 */
class gamewindow : public QMainWindow
{
    Q_OBJECT

public:
    // Конструктор для локальной игры (на одном компьютере).
    explicit gamewindow(QWidget *parent = nullptr);

    // Конструктор для сетевой игры.
    explicit gamewindow(NetworkManager *manager, const QString& initialLayout, PieceColor myColor, QWidget *parent = nullptr);

signals:
    // Сигнал для возврата в главное меню.
    void menuRequested();

private slots:
    // Обработка действий пользователя
    void handleCellClick(int row, int col);
    void onNewGameClicked();
    void onBackToMenuClicked();
    void onPrevMoveClicked();
    void onNextMoveClicked();

    // Реакция на изменения в логике
    void onBoardChanged();

    // Реакция на сетевые события
    void onMoveReceived(const Move& move);
    void onOpponentDisconnected();

    // Чат: приём и отправка
    void onChatReceived(const QString &message);
    void onSendChatClicked();

private:
    // UI Элементы
    ClickableLabel* m_boardCells[8][8];       // Сетка интерактивных клеток доски.
    QTextEdit* m_moveHistory;                 // Панель для отображения истории ходов.
    QGridLayout* m_whiteCapturedLayout;       // Layout для съеденных белых фигур.
    QGridLayout* m_blackCapturedLayout;       // Layout для съеденных черных фигур.
    QPushButton* m_prevMoveButton;            // Кнопка "<" для истории.
    QPushButton* m_nextMoveButton;            // Кнопка ">" для истории.

    // Элементы чата (ТОЛЬКО для сетевой игры; в локальном режиме не используются/скрыты)
    QTextEdit* m_chatHistory = nullptr;       // История переписки (read-only).
    QLineEdit* m_chatInput = nullptr;         // Поле ввода.
    QPushButton* m_sendChatButton = nullptr;  // Кнопка "Отправить".

    // Указатели на другие модули
    PieceLogic* m_logic;                      // Указатель на игровую логику (Модель).
    NetworkManager* m_networkManager = nullptr; // Указатель на сетевой модуль.

    // Состояние игры и UI
    int m_selectedRow = -1;                   // Координаты выбранной фигуры.
    int m_selectedCol = -1;
    bool m_isNetworkGame = false;             // Флаг, определяющий режим игры.
    PieceColor m_myColor;                     // Цвет фигур этого игрока в сетевой игре.

    // Приватные методы для настройки и обновления UI
    void setupUI();
    void updateBoardUI(const Piece* boardState = nullptr);
    void highlightValidMoves();
    void clearHighlights();
    void clearLayout(QLayout* layout);
    QString getPieceImagePath(const Piece& piece);
    void checkAndDisplayGameEndStatus();
};

#endif // GAMEWINDOW_H
