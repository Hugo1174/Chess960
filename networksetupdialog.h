#ifndef NETWORKSETUPDIALOG_H
#define NETWORKSETUPDIALOG_H

#include <QDialog>
#include <QAbstractSocket>
#include "piece_logic.h"

// Предварительные объявления классов Qt
class QTcpServer;
class QTcpSocket;
class QPushButton;
class QLineEdit;
class QLabel;

/**
 * @class NetworkSetupDialog
 * @brief Диалоговое окно для настройки сетевой P2P-игры.
 *
 * Позволяет пользователю выбрать роль Хоста (создать игру)
 * или Клиента (подключиться к игре). Выполняет начальное
 * соединение и обмен данными для старта игры.
 */
class NetworkSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkSetupDialog(QWidget *parent = nullptr);
    ~NetworkSetupDialog();

    // Публичные методы для получения результата после закрытия окна
    QTcpSocket* getSocket() const;
    QString getInitialBoardLayout() const;
    PieceColor getPlayerColor() const;

private slots:
    // Слоты для кнопок
    void hostGame();
    void joinGame();

    // Слоты для обработки сетевых событий
    void onNewConnection();      // Срабатывает у Хоста при подключении Клиента
    void onConnected();          // Срабатывает у Клиента при подключении к Хосту
    void onReadyRead();          // Срабатывает при получении данных
    void onSocketStateChanged(QAbstractSocket::SocketState socketState); // Для отслеживания дисконнектов

private:
    void setupUI(); // Вспомогательная функция для создания интерфейса
    void sendInitialData(QTcpSocket* clientSocket); // Хост отправляет стартовые данные игры
    QString findMyIp() const; // Вспомогательная функция для определения локального IP

    // Сетевые объекты
    QTcpServer* m_server = nullptr;
    QTcpSocket* m_socket = nullptr;

    // Элементы интерфейса
    QPushButton* m_hostButton;
    QPushButton* m_joinButton;
    QLineEdit* m_ipLineEdit;
    QLabel* m_statusLabel;
    QLabel* m_infoLabel;

    // Данные игры для передачи
    QString m_initialBoardLayout;
    PieceColor m_playerColor;
    bool m_isHost; // Флаг, определяющий, является ли этот игрок хостом
};

#endif // NETWORKSETUPDIALOG_H```
