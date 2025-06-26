#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "clickablelabel.h"
#include "piece_logic.h"
#include <QMainWindow>
#include <QTextEdit>
#include <QGridLayout>
class QPushButton;

class gamewindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit gamewindow(QWidget *parent = nullptr);

signals:
    void menuRequested();

private slots:
    void handleCellClick(int row, int col);

    // ИСПРАВЛЕНИЕ 2: Разделяем слоты
    void onBoardChanged(); // Новый слот-прослойка

    void handlePromotion(int row, int col, PieceColor color);
    void onNewGameClicked();
    void onBackToMenuClicked();
    void onPrevMoveClicked();
    void onNextMoveClicked();

private:
    // UI Элементы
    ClickableLabel* m_boardCells[8][8];
    QTextEdit* m_moveHistory;
    QGridLayout* m_whiteCapturedLayout;
    QGridLayout* m_blackCapturedLayout;
    QPushButton* m_prevMoveButton;
    QPushButton* m_nextMoveButton;

    // Указатель на логику игры
    PieceLogic* m_logic;

    // Внутреннее состояние UI
    int m_selectedRow = -1;
    int m_selectedCol = -1;

    // Приватные методы для UI
    void setupUI();
    void updateBoardUI(const Piece* boardState = nullptr); // Этот метод остается без изменений
    void highlightValidMoves();
    void clearHighlights();
    void clearLayout(QLayout* layout);
    QString getPieceImagePath(const Piece& piece);
};

#endif // GAMEWINDOW_H
