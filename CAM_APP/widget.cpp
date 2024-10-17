#include "widget.h"
#include "ui_widget.h"
#include "signup.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <string>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "index.h"
#include <QIntValidator>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    QIntValidator *intValidator = new QIntValidator(this);
    ui->lineEdit->setValidator(intValidator);
}

int UserConnectMySQL(int user_id,const char *user_passwd)
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
        return -1;
    }

    qDebug() << "Connected to the database";

    // 执行数据库查询
    QSqlQuery query;

    if (query.exec("SELECT password FROM myuser where phone = "+QString::number(user_id)+";")) {  // 请替换为您的表名

        QSqlRecord record = query.record();
        int columnCount = record.count();
        qDebug() << "Number of columns:" << columnCount;
        while (query.next()) {
            // 输出查询结果
            QVariant value = query.value(0);

            QByteArray byteArray = value.toString().toUtf8();  // 或者使用 toLatin1()
            const char *charArray = byteArray.constData();

            if(strcmp(user_passwd,charArray) == 0){
                qDebug() << "Column" << 0 << ":" << value.toString();
                db.close();
                //登录成功
                return 1;
            }else {
                db.close();
                //密码错误
                return 0;
            }
        }
    } else {
        QMessageBox::critical(nullptr, QObject::tr("Query Error"),
                              QObject::tr("Failed to execute query. Error: %1").arg(query.lastError().text()));
    }

    // 关闭数据库连接
    db.close();

    //没有此用户
    return -2;
}


Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_2_clicked()
{
    int flage;
    QString ID = ui->lineEdit->text();
    QString passwd = ui->lineEdit_2->text();
    flage = UserConnectMySQL(ID.toInt(),passwd.toUtf8().constData());
    qDebug() << "登录结果为"<<flage;
    if(flage == 1){
        this->close();
        Index *index = new Index(ID.toInt(),passwd);
        index->show();
    }
}

//去
void Widget::on_Signup_btn_s_clicked()
{
    this->close();
    Signup *sig = new Signup();
    sig->show();
}
