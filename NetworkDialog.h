#ifndef NETWORKDIALOG_H
#define NETWORKDIALOG_H

#include <QDialog>
#include "gamewindow.h" // Для доступа к gamewindow::GameMode

// Предварительное объявление, чтобы избежать включения ui-заголовка здесь
namespace Ui {
class NetworkDialog;
}

class NetworkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkDialog(QWidget *parent = nullptr);
    ~NetworkDialog();

    gamewindow::GameMode getSelectedMode() const;
    QString getIpAddress() const;

private slots:
    void on_hostButton_clicked();
    void on_clientButton_clicked();
    void on_buttonBox_accepted();

private:
    Ui::NetworkDialog *ui;
    gamewindow::GameMode m_selectedMode;
};

#endif // NETWORKDIALOG_H
