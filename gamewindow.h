// gamewindow.h

#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "piece_logic.h"
#include <QMainWindow>
#include <array>

class ClickableLabel;
class NetworkHandler;
class QTextEdit;
class QGridLayout;
class QPushButton;
class QLabel;

class gamewindow : public QMainWindow
{
    Q_OBJECT

public:
    enum GameMode { LOCAL, ONLINE_HOST, ONLINE_CLIENT };
    explicit gamewindow(GameMode mode, const QString& peerIp = "", QWidget *parent = nullptr);

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
    void onConnected();
    void onDisconnected();
    void onNetworkError(const QString& error);
    void onGameStartReceived(const std::array<Piece, 64>& board, PieceColor myColor);
    void onMoveReceived(const Move& move, PieceType promotionType);

private:
    ClickableLabel* m_boardCells[8][8];
    QTextEdit* m_moveHistory;
    QGridLayout* m_whiteCapturedLayout;
    QGridLayout* m_blackCapturedLayout;
    QPushButton* m_prevMoveButton;
    QPushButton* m_nextMoveButton;
    QLabel* m_statusLabel;

    PieceLogic* m_logic;
    GameMode m_gameMode;
    PieceColor m_myColor = NO_COLOR;
    int m_selectedRow = -1;
    int m_selectedCol = -1;

    NetworkHandler* m_networkHandler = nullptr;

    void setupUI();
    void setupNetwork(const QString& peerIp);
    void updateBoardUI(const Piece* boardState = nullptr);
    void updateStatusLabel();
    void highlightValidMoves();
    void clearHighlights();
    void clearLayout(QLayout* layout);
    QString getPieceImagePath(const Piece& piece);
};

#endif // GAMEWINDOW_H
