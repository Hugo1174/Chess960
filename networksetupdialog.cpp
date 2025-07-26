#include "networksetupdialog.h"
#include "piece_logic.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QDataStream>
#include <QHostAddress>

NetworkSetupDialog::NetworkSetupDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle("Настройка сетевой игры");
    setMinimumSize(400, 200);
}

NetworkSetupDialog::~NetworkSetupDialog() {}

// Инициализация интерфейса диалогового окна.
void NetworkSetupDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    m_infoLabel = new QLabel("Выберите свою роль в игре:", this);
    m_infoLabel->setStyleSheet("font-weight: bold;");
    QHBoxLayout *roleLayout = new QHBoxLayout();
    m_hostButton = new QPushButton("Создать игру (Хост)", this);
    m_joinButton = new QPushButton("Подключиться", this);
    roleLayout->addWidget(m_hostButton);
    roleLayout->addWidget(m_joinButton);
    QHBoxLayout *ipLayout = new QHBoxLayout();
    ipLayout->addWidget(new QLabel("IP Адрес Хоста:", this));
    m_ipLineEdit = new QLineEdit(this);
    m_ipLineEdit->setPlaceholderText("Введите IP...");
    ipLayout->addWidget(m_ipLineEdit);
    m_statusLabel = new QLabel("Ожидание действия...", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_infoLabel);
    mainLayout->addLayout(roleLayout);
    mainLayout->addLayout(ipLayout);
    mainLayout->addStretch(1);
    mainLayout->addWidget(m_statusLabel);
    connect(m_hostButton, &QPushButton::clicked, this, &NetworkSetupDialog::hostGame);
    connect(m_joinButton, &QPushButton::clicked, this, &NetworkSetupDialog::joinGame);
}

// Пользователь выбрал "Создать игру".
void NetworkSetupDialog::hostGame() {
    m_isHost = true;
    m_hostButton->setEnabled(false);
    m_joinButton->setEnabled(false);
    m_ipLineEdit->setEnabled(false);

    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &NetworkSetupDialog::onNewConnection);

    // Начинаем слушать порт 12345 на всех сетевых интерфейсах.
    if (!m_server->listen(QHostAddress::Any, 12345)) {
        QMessageBox::critical(this, "Ошибка сервера", "Не удалось запустить сервер: " + m_server->errorString());
        close();
        return;
    }

    QString ipAddress = findMyIp();
    m_statusLabel->setText("Ваш IP: " + (ipAddress.isEmpty() ? "Не найден" : ipAddress) + "\nОжидание подключения оппонента...");
}

// Пользователь выбрал "Подключиться".
void NetworkSetupDialog::joinGame() {
    m_isHost = false;
    m_hostButton->setEnabled(false);
    m_joinButton->setEnabled(false);
    m_ipLineEdit->setEnabled(false);

    QString ipAddress = m_ipLineEdit->text();
    if (ipAddress.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, введите IP адрес хоста.");
        m_hostButton->setEnabled(true);
        m_joinButton->setEnabled(true);
        m_ipLineEdit->setEnabled(true);
        return;
    }

    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &NetworkSetupDialog::onConnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkSetupDialog::onReadyRead);
    connect(m_socket, &QAbstractSocket::stateChanged, this, &NetworkSetupDialog::onSocketStateChanged);

    m_statusLabel->setText("Подключение к " + ipAddress + "...");
    m_socket->connectToHost(ipAddress, 12345);
}

// Слот для Хоста: Клиент подключился.
void NetworkSetupDialog::onNewConnection() {
    m_socket = m_server->nextPendingConnection();
    if (!m_socket) return;
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkSetupDialog::onReadyRead);
    connect(m_socket, &QAbstractSocket::stateChanged, this, &NetworkSetupDialog::onSocketStateChanged);
    m_server->close(); // Больше не принимаем подключения.
    m_statusLabel->setText("Оппонент подключен! Настройка игры...");

    // Хост всегда играет белыми.
    m_playerColor = WHITE;
    // Хост генерирует расстановку и отправляет ее Клиенту.
    sendInitialData(m_socket);

    // Все готово, закрываем диалог с успехом.
    accept();
}

// Хост генерирует и отправляет данные для начала игры.
void NetworkSetupDialog::sendInitialData(QTcpSocket* clientSocket) {
    // Генерируем уникальную позицию Chess960.
    PieceLogic tempLogic;
    std::array<Piece, 64> boardState;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            boardState[i * 8 + j] = tempLogic.getPieceAt(i, j);
        }
    }
    // Сериализуем доску в строку для отправки.
    QString layoutStr;
    QTextStream stream(&layoutStr);
    for(const auto& piece : boardState) {
        stream << static_cast<int>(piece.type) << "," << static_cast<int>(piece.color) << ";";
    }
    m_initialBoardLayout = layoutStr;

    // Отправляем данные клиенту: расстановку и его цвет (черный).
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << layoutStr << static_cast<quint8>(BLACK);

    clientSocket->write(block);
}

// Слот для Клиента: успешно подключились к Хосту.
void NetworkSetupDialog::onConnected() {
    m_statusLabel->setText("Соединение установлено!\nОжидание данных от хоста...");
}

// Слот для Клиента: получили данные от Хоста.
void NetworkSetupDialog::onReadyRead() {
    if (m_isHost) return;

    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_5_15);

    // Читаем расстановку и наш цвет.
    quint8 color;
    in >> m_initialBoardLayout >> color;
    m_playerColor = static_cast<PieceColor>(color);

    if (!m_initialBoardLayout.isEmpty()) {
        m_statusLabel->setText("Данные получены! Игра начинается.");
        accept(); // Все готово.
    }
}

// Обработка разрыва соединения на этапе настройки.
void NetworkSetupDialog::onSocketStateChanged(QAbstractSocket::SocketState socketState) {
    if (socketState == QAbstractSocket::UnconnectedState) {
        if (isVisible()) {
            QMessageBox::critical(this, "Ошибка соединения", "Не удалось установить соединение или оно было разорвано.");
            reject(); // Закрываем диалог с ошибкой.
        }
    }
}

// Вспомогательная функция для поиска локального IP.
QString NetworkSetupDialog::findMyIp() const {
    const QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (const QHostAddress &address : ipAddressesList) {
        if (address != QHostAddress::LocalHost && address.toIPv4Address()) {
            return address.toString();
        }
    }
    return QString();
}

// Геттеры.
QTcpSocket* NetworkSetupDialog::getSocket() const { return m_socket; }
QString NetworkSetupDialog::getInitialBoardLayout() const { return m_initialBoardLayout; }
PieceColor NetworkSetupDialog::getPlayerColor() const { return m_playerColor; }
