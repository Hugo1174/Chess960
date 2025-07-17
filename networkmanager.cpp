#include "networkmanager.h"
#include <QTcpSocket>
#include <QDataStream>

NetworkManager::NetworkManager(QTcpSocket *socket, QObject *parent)
    : QObject(parent), m_socket(socket)
{
    // Устанавливаем родителя для сокета, чтобы он удалился вместе с NetworkManager
    m_socket->setParent(this);

    // Подключаем сигналы сокета к нашим обработчикам
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkManager::onSocketStateChanged);
}

void NetworkManager::sendMove(const Move& move)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) return;

    // Создаем байтовый массив для отправки
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    // Записываем тип сообщения, а затем данные хода
    out << static_cast<quint8>(MsgMove)
        << static_cast<quint8>(move.fromRow) << static_cast<quint8>(move.fromCol)
        << static_cast<quint8>(move.toRow)   << static_cast<quint8>(move.toCol);

    m_socket->write(block);
}

void NetworkManager::sendPromotionChoice(PieceType type)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) return;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    // Записываем тип сообщения и тип выбранной фигуры
    out << static_cast<quint8>(MsgPromotionChoice)
        << static_cast<quint8>(type);

    m_socket->write(block);
}

void NetworkManager::onReadyRead()
{
    // Читаем все доступные данные из сокета
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_5_15);

    // Цикл на случай, если пришло несколько сообщений сразу
    while(!in.atEnd()) {
        quint8 msgTypeRaw;
        in >> msgTypeRaw;
        MessageType msgType = static_cast<MessageType>(msgTypeRaw);

        switch (msgType) {
        case MsgMove: {
            quint8 fromRow, fromCol, toRow, toCol;
            in >> fromRow >> fromCol >> toRow >> toCol;
            Move receivedMove = {static_cast<int>(fromRow), static_cast<int>(fromCol),
                                 static_cast<int>(toRow),   static_cast<int>(toCol)};
            emit moveReceived(receivedMove);
            break;
        }
        case MsgPromotionChoice: {
            quint8 pieceTypeRaw;
            in >> pieceTypeRaw;
            emit promotionChoiceReceived(static_cast<PieceType>(pieceTypeRaw));
            break;
        }
            // default:
        }
    }
}

void NetworkManager::onSocketStateChanged()
{
    if (m_socket->state() == QAbstractSocket::UnconnectedState) {
        // Если соединение разорвано, сообщаем об этом
        emit opponentDisconnected();
    }
}
