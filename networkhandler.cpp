
#include "networkhandler.h"
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

NetworkHandler::NetworkHandler(QObject *parent) : QObject(parent) {}

NetworkHandler::~NetworkHandler() {
    if (m_socket) m_socket->disconnectFromHost();
    if (m_server) m_server->close();
}

// --- РЕЖИМ ХОСТА ---
void NetworkHandler::startHosting() {
    if (m_server) return;
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &NetworkHandler::onNewConnection);

    if (!m_server->listen(QHostAddress::Any, 12345)) { // Порт 12345
        emit errorOccurred("Не удалось запустить сервер: " + m_server->errorString());
        return;
    }
    // Здесь UI должен показать IP-адрес хоста для передачи клиенту.
}

void NetworkHandler::onNewConnection() {
    if (m_socket) { // Уже есть подключение, отклоняем новое.
        m_server->nextPendingConnection()->close();
        return;
    }
    m_socket = m_server->nextPendingConnection();
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkHandler::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkHandler::onDisconnected);
    m_server->close(); // Больше не принимаем подключения.
    emit connectedToPeer();
}

// --- РЕЖИМ КЛИЕНТА ---
void NetworkHandler::connectToHost(const QString &ipAddress) {
    if (m_socket) return;
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkHandler::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkHandler::onDisconnected);
    // Для отслеживания успешного подключения, можно использовать сигнал connected().
    connect(m_socket, &QTcpSocket::connected, this, &NetworkHandler::connectedToPeer);

    m_socket->connectToHost(ipAddress, 12345);
}

// --- ОБЩАЯ ЛОГИКА ---
void NetworkHandler::onDisconnected() {
    m_socket->deleteLater();
    m_socket = nullptr;
    emit disconnectedFromPeer();
}

void NetworkHandler::sendMessage(const QJsonObject &message) {
    if (!m_socket || !m_socket->isOpen()) return;
    // Мы добавляем специальный разделитель, чтобы знать, где заканчивается одно JSON-сообщение.
    m_socket->write(QJsonDocument(message).toJson(QJsonDocument::Compact) + "||END||");
}

void NetworkHandler::sendMove(const Move& move, PieceType promotionType) {
    QJsonObject msg;
    msg["type"] = "move";
    msg["fromRow"] = move.fromRow;
    msg["fromCol"] = move.fromCol;
    msg["toRow"] = move.toRow;
    msg["toCol"] = move.toCol;
    msg["promotion"] = promotionType; // PieceType - это int, JSON это поддерживает
    sendMessage(msg);
}

void NetworkHandler::sendGameStart(const std::array<Piece, 64>& board, PieceColor hostColor) {
    QJsonObject msg;
    msg["type"] = "game_start";
    msg["host_color"] = hostColor;

    QJsonArray boardArray;
    for(const auto& piece : board) {
        QJsonObject p;
        p["type"] = piece.type;
        p["color"] = piece.color;
        boardArray.append(p);
    }
    msg["board"] = boardArray;
    sendMessage(msg);
}

void NetworkHandler::onReadyRead() {
    // Данные могут приходить частями. Накапливаем их в буфере.
    m_buffer.append(m_socket->readAll());

    // Обрабатываем все полные сообщения в буфере.
    while (m_buffer.contains("||END||")) {
        int endIndex = m_buffer.indexOf("||END||");
        QByteArray jsonData = m_buffer.left(endIndex);
        m_buffer.remove(0, endIndex + 7); // 7 - длина разделителя "||END||"

        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (doc.isNull() || !doc.isObject()) continue;

        QJsonObject obj = doc.object();
        QString type = obj["type"].toString();

        if (type == "move") {
            Move m = {obj["fromRow"].toInt(), obj["fromCol"].toInt(), obj["toRow"].toInt(), obj["toCol"].toInt()};
            PieceType pt = static_cast<PieceType>(obj["promotion"].toInt());
            emit moveReceived(m, pt);
        } else if (type == "game_start") {
            std::array<Piece, 64> board;
            QJsonArray boardArray = obj["board"].toArray();
            for(int i = 0; i < 64; ++i) {
                QJsonObject p = boardArray[i].toObject();
                board[i] = {static_cast<PieceType>(p["type"].toInt()), static_cast<PieceColor>(p["color"].toInt())};
            }
            // Если хост играет белыми, мы (клиент) играем черными.
            PieceColor myColor = (static_cast<PieceColor>(obj["host_color"].toInt()) == WHITE) ? BLACK : WHITE;
            emit gameStartReceived(board, myColor);
        }
        // ... обработка других типов сообщений (resign, draw_offer)
    }
}
