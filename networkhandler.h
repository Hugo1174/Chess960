
#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "piece_logic.h" // Для структур Move и PieceType

class NetworkHandler : public QObject
{
    Q_OBJECT

public:
    explicit NetworkHandler(QObject *parent = nullptr);
    ~NetworkHandler();

    // Запускает сервер и ожидает подключения клиента.
    void startHosting();
    // Подключается к хосту по указанному адресу.
    void connectToHost(const QString& ipAddress);
    // Отправляет данные о ходе по сети.
    void sendMove(const Move& move, PieceType promotionType);
    // Отправляет начальную позицию игры (только для хоста).
    void sendGameStart(const std::array<Piece, 64>& board, PieceColor hostColor);

private slots:
    // Слот для обработки нового входящего подключения (для хоста).
    void onNewConnection();
    // Слот для чтения данных, пришедших в сокет.
    void onReadyRead();
    // Слот, вызываемый при отключении.
    void onDisconnected();

signals:
    // Сигналы, информирующие UI о сетевых событиях.
    void connectedToPeer();
    void disconnectedFromPeer();
    void gameStartReceived(const std::array<Piece, 64>& board, PieceColor myColor);
    void moveReceived(const Move& move, PieceType promotionType);
    void errorOccurred(const QString& error);

private:
    void sendMessage(const QJsonObject& message);

    QTcpServer* m_server = nullptr;
    QTcpSocket* m_socket = nullptr;
    QByteArray m_buffer; // Буфер для обработки входящих данных
};

#endif // NETWORKHANDLER_H
