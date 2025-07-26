#include "networkmanager.h"
#include <QTcpSocket>
#include <QDataStream>

NetworkManager::NetworkManager(QTcpSocket *socket, QObject *parent)
    : QObject(parent), m_socket(socket)
{
    // NetworkManager теперь управляет временем жизни сокета.
    m_socket->setParent(this);

    // Подключаем сигналы сокета к нашим обработчикам.
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkManager::onSocketStateChanged);
}

// Сериализует и отправляет ход в бинарном виде.
void NetworkManager::sendMove(const Move& move)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) return;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15); // Для совместимости.

    // Записываем тип сообщения и все поля структуры Move.
    out << static_cast<quint8>(MsgMove)
        << static_cast<quint8>(move.fromRow) << static_cast<quint8>(move.fromCol)
        << static_cast<quint8>(move.toRow)   << static_cast<quint8>(move.toCol)
        << static_cast<quint8>(move.promotion);

    m_socket->write(block);
}

// Вызывается, когда в сокет приходят данные.
void NetworkManager::onReadyRead()
{
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_5_15);

    // Цикл на случай, если пришло несколько сообщений сразу.
    while(!in.atEnd()) {
        quint8 msgTypeRaw;
        in >> msgTypeRaw;
        MessageType msgType = static_cast<MessageType>(msgTypeRaw);

        if (msgType == MsgMove) {
            quint8 fromRow, fromCol, toRow, toCol, promotionPiece;
            in >> fromRow >> fromCol >> toRow >> toCol >> promotionPiece;

            // Собираем структуру Move из полученных данных.
            Move receivedMove = {static_cast<int>(fromRow), static_cast<int>(fromCol),
                                 static_cast<int>(toRow),   static_cast<int>(toCol),
                                 static_cast<PieceType>(promotionPiece)};
            // Уведомляем остальную часть программы о полученном ходе.
            emit moveReceived(receivedMove);
        }
    }
}

// Вызывается при изменении состояния сокета.
void NetworkManager::onSocketStateChanged()
{
    if (m_socket->state() == QAbstractSocket::UnconnectedState) {
        emit opponentDisconnected();
    }
}
