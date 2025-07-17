#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include "piece_logic.h" // Для структур Move и PieceType

class QTcpSocket;

/**
 * @class NetworkManager
 * @brief Управляет сетевым взаимодействием во время игры.
 *
 * Инкапсулирует логику отправки и получения игровых данных (ходы,
 * превращения пешек и т.д.) через TCP сокет.
 */
class NetworkManager : public QObject
{
    Q_OBJECT

public:
    // Мы передаем сокет в конструктор и делаем NetworkManager его владельцем
    explicit NetworkManager(QTcpSocket *socket, QObject *parent = nullptr);

    // Публичные методы для отправки данных оппоненту
    void sendMove(const Move& move);
    void sendPromotionChoice(PieceType type);
    // Добавим позже: sendResignation(), sendDrawOffer()

signals:
    // Сигналы, которые получает UI (gamewindow)
    void moveReceived(const Move& move);
    void promotionChoiceReceived(PieceType type);
    void opponentDisconnected();

private slots:
    // Слоты для обработки событий сокета
    void onReadyRead();
    void onSocketStateChanged();

private:
    QTcpSocket* m_socket; // Сокет для общения с оппонентом

    // Определяем простой протокол. Каждое сообщение начинается с типа.
    enum MessageType : quint8 {
        MsgMove,
        MsgPromotionChoice
        // MsgResignation,
        // MsgDrawOffer
    };
};

#endif // NETWORKMANAGER_H
