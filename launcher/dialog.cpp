#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::SetUsername(QString _username)
{
    username = _username;
}

void Dialog::on_buttonBox_accepted()
{
    QJsonObject json;
    QString new_password;
    new_password = ui->lineEdit->text();
    QCryptographicHash md5_hash(QCryptographicHash::Md5);
    md5_hash.addData(new_password.toUtf8());
    QString md5_password(md5_hash.result().toHex());
    json["password"] = md5_password;
    json["username"] = username;
    QJsonDocument json_doc(json);

    QString output = json_doc.toJson();

    QNetworkRequest req(QUrl("http://192.168.64.148/new_password/"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setHeader(QNetworkRequest::ContentLengthHeader, output.size());
    m_accmng.post(req, output.toUtf8());
}
