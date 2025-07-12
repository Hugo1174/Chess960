// gamewindow.h
// Определяет главное игровое окно.

#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "clickablelabel.h"
#include "piece_logic.h"
#include <QMainWindow>
#include <QTextEdit>
#include <QGridLayout>

class QPushButton;

/**
 * @class gamewindow
 * @brief Основное окно игры. Выступает в роли View и Controller.
 *
 * Отображает игровое поле, обрабатывает ввод пользователя (клики),
 * и взаимодействует с классом игровой логики (PieceLogic).
 */
class gamewindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit gamewindow(QWidget *parent = nullptr);

signals:
    // Сигнал для MainWindow, информирующий о желании вернуться в главное меню.
    void menuRequested();

private slots:
    // Обрабатывает клик по клетке доски.
    void handleCellClick(int row, int col);

    // Обновляет UI после изменения состояния доски в PieceLogic.
    void onBoardChanged();

    // Открывает диалог выбора фигуры при превращении пешки.
    void handlePromotion(int row, int col, PieceColor color);

    // Слоты для кнопок управления игрой.
    void onNewGameClicked();
    void onBackToMenuClicked();
    void onPrevMoveClicked();
    void onNextMoveClicked();

private:
    // --- UI Элементы ---
    // Визуальное представление доски в виде массива интерактивных меток.
    ClickableLabel* m_boardCells[8][8];
    // Поле для отображения истории ходов в нотации.
    QTextEdit* m_moveHistory;
    // Layout-ы для отображения съеденных фигур.
    QGridLayout* m_whiteCapturedLayout;
    QGridLayout* m_blackCapturedLayout;
    // Кнопки для навигации по истории ходов.
    QPushButton* m_prevMoveButton;
    QPushButton* m_nextMoveButton;

    // Указатель на ядро игровой логики (Model).
    PieceLogic* m_logic;

    // --- Внутреннее состояние UI ---
    // Хранит координаты выбранной фигуры для совершения хода.
    int m_selectedRow = -1;
    int m_selectedCol = -1;

    // --- Приватные методы для UI ---
    // Инициализирует и собирает весь пользовательский интерфейс окна.
    void setupUI();
    // Полностью перерисовывает доску, фигуры и другую информацию.
    // Может отображать как текущее, так и историческое состояние доски.
    void updateBoardUI(const Piece* boardState = nullptr);
    // Подсвечивает доступные ходы для выбранной фигуры.
    void highlightValidMoves();
    // Сбрасывает всю подсветку на доске.
    void clearHighlights();
    // Вспомогательная функция для безопасной очистки layout от виджетов.
    void clearLayout(QLayout* layout);
    // Возвращает путь к изображению фигуры в ресурсах.
    QString getPieceImagePath(const Piece& piece);
};

#endif // GAMEWINDOW_H
