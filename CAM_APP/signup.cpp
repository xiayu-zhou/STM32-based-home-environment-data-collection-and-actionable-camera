#include "signup.h"
#include "ui_signup.h"
#include "widget.h"



Signup::Signup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Signup)
{
    ui->setupUi(this);
}

Signup::~Signup()
{
    delete ui;
}

//注册按钮的执行
void Signup::on_signup_btn_clicked()
{
    // 设置 MySQL 数据库连接参数
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("119.23.59.142");  // 请替换为您的 MySQL 主机名
    db.setDatabaseName("Test_1");  // 请替换为您的数据库名称
    db.setUserName("Kiana");  // 请替换为您的数据库用户名
    db.setPassword("Kiana520...");  // 请替换为您的数据库密码

    // 尝试连接到数据库
    if (!db.open()) {
        //连接失败
        return;
    }

    qDebug() << "Connected to the database";

    // 执行数据库查询
    QSqlQuery query;

    if (query.exec("INSERT INTO myuser (phone, password) VALUES ("+ui->phonet->text()+", '"+ui->passwdt->text()+"');")) {  // 请替换为您的表名

        QSqlRecord record = query.record();
        int columnCount = record.count();
        qDebug() << "Number of columns:" << columnCount;
        while (query.next()) {
            // 输出查询结果
            QVariant value = query.value(0);

            QByteArray byteArray = value.toString().toUtf8();  // 或者使用 toLatin1()
            const char *charArray = byteArray.constData();



        }
    }
    db.close();
    // 关闭数据库连接
}

//登录按钮执行
void Signup::on_login_btn_clicked()
{
    this->close();
    Widget *login = new Widget();
    login->show();
}
