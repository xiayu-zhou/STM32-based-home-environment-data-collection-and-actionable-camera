#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>
#include <QTextStream>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>

namespace Ui {
class Signup;
}

class Signup : public QWidget
{
    Q_OBJECT

public:
    explicit Signup(QWidget *parent = nullptr);
    ~Signup();

private slots:
    void on_signup_btn_clicked();

    void on_login_btn_clicked();

private:
    Ui::Signup *ui;
};

#endif // SIGNUP_H
