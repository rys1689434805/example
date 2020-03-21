#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Req_login()
{
    //取出用户名密码
    QString username = ui->username->text();
    QString password = ui->password->text();

    //创建密码的签名值
    QCryptographicHash md5_hash(QCryptographicHash::Md5);
    md5_hash.addData(password.toUtf8());
    QString md5_password(md5_hash.result().toHex());

    //拼登陆请求(Json格式)
    //构建一个json对象，填入用户名和密码的签名值
    QJsonObject json;
    json["username"] = username;
    json["password"] = md5_password;

    //将json对象放到doc中
    QJsonDocument json_doc(json);

    QString output = json_doc.toJson();

    //发给登陆服务器（发http---》qt网络访问库）
    QNetworkRequest req(QUrl("http://192.168.64.148/login/"));
    req.setHeader(QNetworkRequest::ContentLengthHeader, output.size());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_reply = m_access_mng.post(req,output.toUtf8());
    //异步处理服务器的回复（connect 服务器回复和回复的处理函数）
    connect(m_reply, SIGNAL(finished()), this, SLOT(proc_login_reply()));
}

void MainWindow::Req_login(QString _opt)
{
    //取出用户名密码
    QString username = ui->username->text();
    QString password = ui->password->text();

    //创建密码的签名值
    QCryptographicHash md5_hash(QCryptographicHash::Md5);
    md5_hash.addData(password.toUtf8());
    QString md5_password(md5_hash.result().toHex());

    //拼登陆请求(Json格式)
    //构建一个json对象，填入用户名和密码的签名值
    QJsonObject json;
    json["username"] = username;
    json["password"] = md5_password;

    //将参数填入opt键
    json["opt"] = _opt;

    //将json对象放到doc中
    QJsonDocument json_doc(json);

    QString output = json_doc.toJson();

    //发给登陆服务器（发http---》qt网络访问库）
    QNetworkRequest req(QUrl("http://192.168.64.148/login/"));
    req.setHeader(QNetworkRequest::ContentLengthHeader, output.size());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_reply = m_access_mng.post(req,output.toUtf8());
    //异步处理服务器的回复（connect 服务器回复和回复的处理函数）
    connect(m_reply, SIGNAL(finished()), this, SLOT(proc_login_reply()));
}

void MainWindow::Req_login(QString _opt, QString _room_no)
{
    //取出用户名密码
    QString username = ui->username->text();
    QString password = ui->password->text();

    //创建密码的签名值
    QCryptographicHash md5_hash(QCryptographicHash::Md5);
    md5_hash.addData(password.toUtf8());
    QString md5_password(md5_hash.result().toHex());

    //拼登陆请求(Json格式)
    //构建一个json对象，填入用户名和密码的签名值
    QJsonObject json;
    json["username"] = username;
    json["password"] = md5_password;

    //将参数填入opt键
    json["opt"] = _opt;
    json["room_no"] = _room_no;

    //将json对象放到doc中
    QJsonDocument json_doc(json);

    QString output = json_doc.toJson();

    //发给登陆服务器（发http---》qt网络访问库）
    QNetworkRequest req(QUrl("http://192.168.64.148/login/"));
    req.setHeader(QNetworkRequest::ContentLengthHeader, output.size());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_reply = m_access_mng.post(req,output.toUtf8());
    //异步处理服务器的回复（connect 服务器回复和回复的处理函数）
    connect(m_reply, SIGNAL(finished()), this, SLOT(proc_login_reply()));
}

void MainWindow::on_createroom_clicked()
{
    m_cur_opt = create_room;
    Req_login("create_room");

}

void MainWindow::proc_login_reply()
{
    //判断登陆结果
    QJsonDocument json_doc = QJsonDocument::fromJson(m_reply->readAll());

    QJsonObject json = json_doc.object();

    if ("OK" == json["login_result"].toString())
    {
        if (create_room == m_cur_opt)
        {
            QProcess pro;
            QStringList argv;
            QString room_no = json["room_no"].toString();

            QMessageBox::information(this, "房间号","房间号："+room_no);

            argv.push_back("192.168.64.148");
            argv.push_back(room_no);
            pro.startDetached("client.exe", argv,".");
            qApp->exit();
        }
        else if (follow_room == m_cur_opt)
        {
            if ("OK" == json["follow_result"].toString())
            {
                QProcess pro;
                QStringList argv;
                QString room_no = ui->room_no->text();
                argv.push_back("192.168.64.148");
                argv.push_back(room_no);
                pro.startDetached("client.exe", argv,".");
                qApp->exit();
            }
            else {
                QMessageBox::information(this, "跟房失败", "房间不存在");
            }
        }
        else if (change_password == m_cur_opt) {
            d.SetUsername(ui->username->text());
            d.show();
        }
    }
    else {
        QMessageBox::information(this, "登陆失败", "用户名或密码错误");
    }
}

void MainWindow::on_register_2_clicked()
{
    //启动浏览器并访问注册界面
    QDesktopServices::openUrl(QUrl("http://192.168.64.148"));
}

void MainWindow::on_new_password_clicked()
{
    m_cur_opt = change_password;
    Req_login();
}

void MainWindow::on_follow_clicked()
{
    m_cur_opt = follow_room;
    Req_login("follow_room", ui->room_no->text());
}
