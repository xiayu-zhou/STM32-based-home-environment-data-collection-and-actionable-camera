#ifndef INDEX_H
#define INDEX_H

#include <QWidget>
#include <QTcpServer>   //监听套接字
#include <QTcpSocket>   //通信套接字
#include <QCoreApplication>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QDateTime>
#include <QApplication> // 包含 QApplication 头文件
#include <QMessageBox>
#include <QTimer>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#
}
namespace Ui {
class Index;
}

class Index : public QWidget
{
    Q_OBJECT

public:
    explicit Index(QWidget *parent = nullptr);
    Index(int user_id,QString passwd);
    ~Index();
    void init();
    int videoshow();
    int initVideo();

    int initVideoPlace();

//    void receiveData();

    void handleReadyRead();

private slots:

    int getTimeAndShow();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    
    void on_open_control_clicked();

    void on_key_left_m_pressed();

    void on_key_left_m_released();

    void on_key_right_m_pressed();

    void on_key_right_m_released();

    void on_key_up_m_pressed();

    void on_key_up_m_released();

    void on_key_down_m_pressed();

    void on_key_down_m_released();

    void on_key_reset_m_clicked();

    void on_startReceiveData_btn_clicked();

private:

    Ui::Index *ui;

    int user_id;

    QString passwd;

    // 创建一个TCP套接字
    QTcpSocket tcpSocket;

    bool open_video;

    //QLabel video_region;

    bool on_open_control_flag;

    QString Up_and_Down;

    QString Left_and_right;

    QString receiveDataFlagWithHardware;

    bool pressKeyA;

    bool pressKeyD;

    bool pressKeyW;

    bool pressKeyS;

    bool receiveDataFalag;

    QTimer receiveDataTimer;


protected:
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void keyReleased();
private slots:
    void receiveData();
    signals:

};

#endif // INDEX_H
