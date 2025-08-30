#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include "piece_logic.h"

class QTcpSocket;

/**
 * @class NetworkManager
 * @brief Инкапсулирует весь сетевой протокол игры.
 *
 * Отвечает за сериализацию, отправку, получение и десериализацию
 * игровых данных (ходов) через TCP сокет, а также обмен сообщениями чата.
 */
class NetworkManager : public QObject
{
    Q_OBJECT

public:
    // Принимает уже установленный сокет и становится его владельцем.
    explicit NetworkManager(QTcpSocket *socket, QObject *parent = nullptr);

    // Отправляет ход (включая информацию о превращении) оппоненту.
    void sendMove(const Move& move);

    // Отправляет текстовое сообщение чата оппоненту.
    void sendChatMessage(const QString &message);

signals:
    // Сигнал, испускаемый при получении хода от оппонента.
    void moveReceived(const Move& move);
    // Сигнал о разрыве соединения.
    void opponentDisconnected();
    // Сигнал при получении сообщения чата.
    void chatReceived(const QString &message);

private slots:
    // Внутренние слоты для обработки событий сокета.
    void onReadyRead();
    void onSocketStateChanged();

private:
    QTcpSocket* m_socket;

    // Простой бинарный протокол.
    enum MessageType : quint8 {
        MsgMove = 0,   // Ход.
        MsgChat = 1    // Сообщение чата (QString).
    };
};

#endif // NETWORKMANAGER_H
