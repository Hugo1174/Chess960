#ifndef NETWORKSETUPDIALOG_H
#define NETWORKSETUPDIALOG_H

#include <QDialog>
#include <QAbstractSocket>
#include "piece_logic.h"

// Предварительные объявления
class QTcpServer;
class QTcpSocket;
class QPushButton;
class QLineEdit;
class QLabel;

/**
 * @class NetworkSetupDialog
 * @brief Диалог для установки P2P-соединения.
 *
 * Предоставляет пользователю выбор: создать игру (Хост) или подключиться (Клиент).
 * Выполняет "рукопожатие": Хост генерирует расстановку и отправляет ее Клиенту.
 * После успешного завершения предоставляет готовый сокет и параметры игры.
 */
class NetworkSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkSetupDialog(QWidget *parent = nullptr);
    ~NetworkSetupDialog();

    // Геттеры для получения результата работы диалога.
    QTcpSocket* getSocket() const;
    QString getInitialBoardLayout() const;
    PieceColor getPlayerColor() const;

private slots:
    // Слоты для кнопок UI.
    void hostGame();
    void joinGame();

    // Слоты для обработки сетевых событий.
    void onNewConnection();      // Для Хоста: когда Клиент подключился.
    void onConnected();          // Для Клиента: когда он подключился к Хосту.
    void onReadyRead();          // Для Клиента: получение данных от Хоста.
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);

private:
    void setupUI();
    void sendInitialData(QTcpSocket* clientSocket); // Хост отправляет стартовые данные.
    QString findMyIp() const; // Поиск локального IP для удобства.

    // Сетевые объекты
    QTcpServer* m_server = nullptr;
    QTcpSocket* m_socket = nullptr;

    // Элементы интерфейса
    QPushButton* m_hostButton;
    QPushButton* m_joinButton;
    QLineEdit* m_ipLineEdit;
    QLabel* m_statusLabel;
    QLabel* m_infoLabel;

    // Данные игры для обмена
    QString m_initialBoardLayout; // Строка с расстановкой фигур.
    PieceColor m_playerColor;     // Цвет, которым будет играть этот игрок.
    bool m_isHost;                // Флаг роли этого игрока.
};

#endif // NETWORKSETUPDIALOG_H
