#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QGridLayout>
#include <QVBoxLayout>

class gamewindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit gamewindow(QWidget *parent = nullptr);
    ~gamewindow();

private:
    QWidget *centralWidget;
    QLabel *topCapturedPieces;
    QLabel *bottomCapturedPieces;
    QTextEdit *moveHistory;
    QWidget *chessBoard;
};

#endif // GAMEWINDOW_H
