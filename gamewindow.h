#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "clickablelabel.h"
#include "piece_logic.h"
#include <QMainWindow>
#include <QTextEdit>
#include <QGridLayout>

// Предварительные объявления
class QPushButton;
class NetworkManager;

class gamewindow : public QMainWindow
{
    Q_OBJECT

public:
    // Старый конструктор для локальной игры
    explicit gamewindow(QWidget *parent = nullptr);

    // НОВЫЙ конструктор для сетевой игры
    explicit gamewindow(NetworkManager *manager, const QString& initialLayout, PieceColor myColor, QWidget *parent = nullptr);


signals:
    void menuRequested();

private slots:
    void handleCellClick(int row, int col);
    void onBoardChanged();
    void handlePromotion(int row, int col, PieceColor color);
    void onNewGameClicked();
    void onBackToMenuClicked();
    void onPrevMoveClicked();
    void onNextMoveClicked();

    // НОВЫЕ слоты для обработки сетевых событий
    void onMoveReceived(const Move& move);
    void onPromotionReceived(PieceType type);
    void onOpponentDisconnected();


private:
    // UI Элементы
    ClickableLabel* m_boardCells[8][8];
    QTextEdit* m_moveHistory;
    QGridLayout* m_whiteCapturedLayout;
    QGridLayout* m_blackCapturedLayout;
    QPushButton* m_prevMoveButton;
    QPushButton* m_nextMoveButton;

    // Указатели на логику и сеть
    PieceLogic* m_logic;
    NetworkManager* m_networkManager = nullptr;

    // Состояние UI и игры
    int m_selectedRow = -1;
    int m_selectedCol = -1;
    bool m_isNetworkGame = false;
    PieceColor m_myColor;

    // Приватные методы для UI
    void setupUI();
    void updateBoardUI(const Piece* boardState = nullptr);
    void highlightValidMoves();
    void clearHighlights();
    void clearLayout(QLayout* layout);
    QString getPieceImagePath(const Piece& piece);
};

#endif // GAMEWINDOW_H
