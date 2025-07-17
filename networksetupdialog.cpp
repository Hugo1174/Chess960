#include "networksetupdialog.h"
#include "piece_logic.h" // Для генерации позиции

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

NetworkSetupDialog::~NetworkSetupDialog()
{
    // Объекты с родителем (this) удалятся автоматически
}

void NetworkSetupDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    m_infoLabel = new QLabel("Выберите свою роль в игре:", this);
    m_infoLabel->setStyleSheet("font-weight: bold;");

    // Панель для кнопок "Хост" и "Клиент"
    QHBoxLayout *roleLayout = new QHBoxLayout();
    m_hostButton = new QPushButton("Создать игру (Хост)", this);
    m_joinButton = new QPushButton("Подключиться", this);
    roleLayout->addWidget(m_hostButton);
    roleLayout->addWidget(m_joinButton);

    // Панель для ввода IP
    QHBoxLayout *ipLayout = new QHBoxLayout();
    ipLayout->addWidget(new QLabel("IP Адрес Хоста:", this));
    m_ipLineEdit = new QLineEdit(this);
    m_ipLineEdit->setPlaceholderText("Введите IP...");
    ipLayout->addWidget(m_ipLineEdit);

    m_statusLabel = new QLabel("Ожидание действия...", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);

    // Сборка интерфейса
    mainLayout->addWidget(m_infoLabel);
    mainLayout->addLayout(roleLayout);
    mainLayout->addLayout(ipLayout);
    mainLayout->addStretch(1);
    mainLayout->addWidget(m_statusLabel);

    // Подключение сигналов кнопок
    connect(m_hostButton, &QPushButton::clicked, this, &NetworkSetupDialog::hostGame);
    connect(m_joinButton, &QPushButton::clicked, this, &NetworkSetupDialog::joinGame);
}


void NetworkSetupDialog::hostGame() {
    m_isHost = true;
    m_hostButton->setEnabled(false);
    m_joinButton->setEnabled(false);
    m_ipLineEdit->setEnabled(false);

    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &NetworkSetupDialog::onNewConnection);

    // Начинаем слушать на любом адресе, порт 12345
    if (!m_server->listen(QHostAddress::Any, 12345)) {
        QMessageBox::critical(this, "Ошибка сервера", "Не удалось запустить сервер: " + m_server->errorString());
        close();
        return;
    }

    QString ipAddress = findMyIp();
    m_statusLabel->setText("Ваш IP: " + (ipAddress.isEmpty() ? "Не найден" : ipAddress) + "\nОжидание подключения оппонента...");
}


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
    // Подключаем все необходимые сигналы сокета
    connect(m_socket, &QTcpSocket::connected, this, &NetworkSetupDialog::onConnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkSetupDialog::onReadyRead);
    connect(m_socket, &QAbstractSocket::stateChanged, this, &NetworkSetupDialog::onSocketStateChanged);

    m_statusLabel->setText("Подключение к " + ipAddress + "...");
    m_socket->connectToHost(ipAddress, 12345);
}


void NetworkSetupDialog::onNewConnection() {
    // Эта функция вызывается только у Хоста
    m_socket = m_server->nextPendingConnection();
    if (!m_socket) return;

    // Подключаем сигналы к новому сокету
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkSetupDialog::onReadyRead);
    connect(m_socket, &QAbstractSocket::stateChanged, this, &NetworkSetupDialog::onSocketStateChanged);

    m_server->close(); // Больше не принимаем подключения
    m_statusLabel->setText("Оппонент подключен! Настройка игры...");

    // Хост играет белыми
    m_playerColor = WHITE;

    // Генерация и отправка стартовой информации
    sendInitialData(m_socket);

    // Соединение установлено, можно закрывать диалог
    accept();
}

void NetworkSetupDialog::sendInitialData(QTcpSocket* clientSocket) {
    // Генерируем уникальную позицию Chess960
    PieceLogic tempLogic;
    std::array<Piece, 64> boardState;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            boardState[i * 8 + j] = tempLogic.getPieceAt(i, j);
        }
    }
    // Сериализуем доску в строку для отправки
    QString layoutStr;
    QTextStream stream(&layoutStr);
    for(const auto& piece : boardState) {
        stream << static_cast<int>(piece.type) << "," << static_cast<int>(piece.color) << ";";
    }
    m_initialBoardLayout = layoutStr;

    // Отправляем данные клиенту
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << layoutStr << static_cast<quint8>(BLACK); // Клиент будет играть черными

    clientSocket->write(block);
}

void NetworkSetupDialog::onConnected() {
    // Эта функция вызывается только у Клиента
    m_statusLabel->setText("Соединение установлено!\nОжидание данных от хоста...");
}

void NetworkSetupDialog::onReadyRead() {
    // Эта функция вызывается только у Клиента при получении данных от Хоста
    if (m_isHost) return; // Хост не должен получать данные на этом этапе

    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_5_15);

    // Читаем расстановку и наш цвет
    quint8 color;
    in >> m_initialBoardLayout >> color;

    m_playerColor = static_cast<PieceColor>(color);

    if (!m_initialBoardLayout.isEmpty()) {
        m_statusLabel->setText("Данные получены! Игра начинается.");
        // Все готово, можно закрывать диалог
        accept();
    }
}


void NetworkSetupDialog::onSocketStateChanged(QAbstractSocket::SocketState socketState) {
    if (socketState == QAbstractSocket::UnconnectedState) {
        // Если соединение разорвалось до начала игры
        if (isVisible()) {
            QMessageBox::critical(this, "Ошибка соединения", "Не удалось установить соединение или оно было разорвано.");
            reject(); // Закрываем диалог с отрицательным результатом
        }
    }
}

QString NetworkSetupDialog::findMyIp() const {
    // Ищем подходящий IPv4 адрес для отображения
    const QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (const QHostAddress &address : ipAddressesList) {
        if (address != QHostAddress::LocalHost && address.toIPv4Address()) {
            return address.toString();
        }
    }
    return QString();
}

// Реализация геттеров
QTcpSocket* NetworkSetupDialog::getSocket() const { return m_socket; }
QString NetworkSetupDialog::getInitialBoardLayout() const { return m_initialBoardLayout; }
PieceColor NetworkSetupDialog::getPlayerColor() const { return m_playerColor; }
