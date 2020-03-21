#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    void SetUsername(QString _username);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Dialog *ui;
    QString username;
    QNetworkAccessManager m_accmng;
};

#endif // DIALOG_H
