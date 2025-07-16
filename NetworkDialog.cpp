#include "NetworkDialog.h"
#include "ui_NetworkDialog.h"

NetworkDialog::NetworkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkDialog),
    m_selectedMode(gamewindow::LOCAL) // Инициализация по умолчанию
{
    ui->setupUi(this);
    setWindowTitle("Сетевая игра");

    // По умолчанию IP-адрес недоступен, так как выбран режим хоста
    ui->ipAddressLineEdit->setEnabled(false);
    ui->hostButton->setChecked(true); // Хост выбран по умолчанию
    m_selectedMode = gamewindow::ONLINE_HOST;
}

NetworkDialog::~NetworkDialog()
{
    delete ui;
}

gamewindow::GameMode NetworkDialog::getSelectedMode() const
{
    return m_selectedMode;
}

QString NetworkDialog::getIpAddress() const
{
    return ui->ipAddressLineEdit->text();
}

void NetworkDialog::on_hostButton_clicked()
{
    ui->ipAddressLineEdit->setEnabled(false); // Отключаем поле ввода IP
    m_selectedMode = gamewindow::ONLINE_HOST;
}

void NetworkDialog::on_clientButton_clicked()
{
    ui->ipAddressLineEdit->setEnabled(true); // Включаем поле ввода IP
    m_selectedMode = gamewindow::ONLINE_CLIENT;
}

// Этот слот будет вызван при нажатии кнопки "OK"
void NetworkDialog::on_buttonBox_accepted()
{
    // Проверки можно добавить здесь, но они уже есть в MainWindow
    accept(); // Закрывает диалог и возвращает QDialog::Accepted
}
