#include "index.h"
#include "ui_index.h"
#include <QDebug>
#include <QCoreApplication>
#include <QTimer>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QProcess>
#include <QByteArray>
#include <QApplication>
#include <QThread>

#include <QString>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QKeyEvent>
#include <QPalette>
#include <QStringList>
#include <QPalette>






Index::Index(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Index)
{
    ui->setupUi(this);
}

Index::Index(int user_id, QString passwd) : Index()
{
    this->user_id = user_id;
    this->passwd = passwd;
    this->open_video = false;
    this->Up_and_Down = "0";
    this->Left_and_right = "0";
    this->pressKeyA = false;
    this->pressKeyD = false;
    this->pressKeyW = false;
    this->pressKeyS = false;
    this->receiveDataFalag = false;
    this->receiveDataFlagWithHardware = "0";
    this->receiveDataTimer.setInterval(3000);
    initVideo();
    initVideoPlace();
    init();
    getTimeAndShow();
}

Index::~Index()
{
    delete ui;
}

void Index::init()
{
    //设置主窗口的背景颜色
    QColor main_back_Color(0, 0, 51);
    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window,main_back_Color);
    this->setPalette(palette);

    //右边控制台背景颜色
    QColor right_control_back_Color(128, 128, 128);
    ui->right_control->setAutoFillBackground(true);
    palette.setColor(QPalette::Window,right_control_back_Color);
    ui->right_control->setPalette(palette);


    //右边控制台背景颜色
    QColor show_datalabel_back_Color(64, 64, 64);
    ui->show_data_label->setAutoFillBackground(true);
    palette.setColor(QPalette::Window,show_datalabel_back_Color);
    ui->show_data_label->setPalette(palette);
    ui->video_show_la->setAutoFillBackground(true);
    palette.setColor(QPalette::Window,show_datalabel_back_Color);
    ui->video_show_la->setPalette(palette);
    ui->con_lan_la->setAutoFillBackground(true);
    palette.setColor(QPalette::Window,show_datalabel_back_Color);
    ui->con_lan_la->setPalette(palette);
    ui->bottom_la->setAutoFillBackground(true);
    palette.setColor(QPalette::Window,show_datalabel_back_Color);
    ui->bottom_la->setPalette(palette);

    //视频数据显示条
    ui->data_bar_label->setStyleSheet("background-color: rgba(255, 255, 255, 126);");
    ui->video_show_la->setStyleSheet("background-color: rgba(255, 255, 255, 126);");
    ui->con_lan_la->setStyleSheet("background-color: rgba(255, 255, 255, 126);");

    ui->MQ_135->setStyleSheet("background-color: rgba(0, 0, 0, 0);");
    ui->MQ2_ppm->setStyleSheet("background-color: rgba(0, 0, 0, 0);");
    ui->Time_my->setStyleSheet("background-color: rgba(0, 0, 0, 0);");
    //ui->DHT11_btn->setStyleSheet("background-color: rgba(0, 0, 0, 0);");
    ui->DHT11_btn_H->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

    //设置槽函数
    // 连接定时器的超时信号到自定义槽函数
    QObject::connect(&receiveDataTimer, &QTimer::timeout,this,&Index::receiveData);
}

QImage ConvertImage(const AVFrame* frame) {
    // 创建
    QImage img (frame->width, frame->height, QImage::Format_RGB888);

    // 将 YUV 数据转换为 RGB 并复制到 QImage
    for (int y = 0; y < frame->height; ++y) {
        for (int x = 0; x < frame->width; ++x) {
            int index = y * frame->linesize[0] + x;

            int Y = frame->data[0][index];
            int U = frame->data[1][index / 4];
            int V = frame->data[2][index / 4];

            // Convert YUV to RGB
            int C = Y - 16;
            int D = U - 128;
            int E = V - 128;
            int R = qBound(0, (298 * C + 409 * E + 128) >> 8, 255);

        }
    }
    return img;
}


QString tcpClientConnet(const QString& serverAddress, int port, const QString& message,QTcpSocket &tcpSocket)
{
    QString reslt = "NO";
    // 连接到服务器
    tcpSocket.connectToHost(serverAddress, port);

    if (tcpSocket.waitForConnected()) {
        qDebug() << "Connected to server.";

        // 发送消息到服务器
        tcpSocket.write(message.toUtf8());

        // 等待数据从服务器返回
        if (tcpSocket.waitForReadyRead()) {
            QByteArray responseData = tcpSocket.readAll();
            qDebug() << "Received response from server:" << responseData;
            reslt = responseData;
        } else {
            qDebug() << "No response from server.";
        }

        // 断开连接
        //tcpSocket.disconnectFromHost();
    } else {
        qDebug() << "Failed to connect to server:" << tcpSocket.errorString();
    }
    return reslt;
}


void Index::on_pushButton_clicked()
{
    qDebug()<<"user_id = " << this->user_id<<" , passwd = " << this->passwd;

    //tcpClient(serverAddress, port, message);
}

void Index::on_pushButton_2_clicked()
{
    videoshow();
}

int Index::videoshow()
{
    QFuture<int> future = QtConcurrent::run([=]() {
        // 在后台线程中使用 formatContext

        this->open_video = true;

        // 初始化FFmpeg
        av_register_all();
        avcodec_register_all();

        // 打开视频流
        AVFormatContext *formatContext = avformat_alloc_context();
        //http://192.168.31.222/mjpeg/1
        //D://202211272102.mp4d

        if (avformat_open_input(&formatContext, "http://192.168.31.223/mjpeg/1", nullptr, nullptr) != 0) {
            qDebug() << "Could not open input stream";
            return -1;
        }

        if (avformat_find_stream_info(formatContext, nullptr) < 0) {
            qDebug() << "Could not find stream information";
            return -1;
        }

        AVCodec *codec = nullptr;
        int videoStreamIndex = -1;
        qDebug()<<"媒体文件中的流量为："<<formatContext->nb_streams;
        for (unsigned int i = 0; i < formatContext->nb_streams; i++) {

            //streams 指向流数组的指针
            //codecpar 是一个指向 AVCodecParameters 结构体的指针，它包含了与该流相关的编解码参数信息。
            //codec_type 是 AVCodecParameters 结构体中的一个字段，表示流的类型，可能的取值包括
            //  AVMEDIA_TYPE_VIDEO（视频流）、AVMEDIA_TYPE_AUDIO（音频流）、AVMEDIA_TYPE_SUBTITLE（字幕流）等。
            if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoStreamIndex = i;

                //寻找到解码器
                codec = avcodec_find_decoder(formatContext->streams[i]->codecpar->codec_id);


                qDebug()<< "解码器为："<<formatContext->streams[i]->codecpar->codec_id;
                break;
            }
        }

        if (videoStreamIndex == -1 || codec == nullptr) {
            qDebug() << "Could not find video stream or codec";
            return -1;
        }

        AVCodecContext *codecContext = avcodec_alloc_context3(codec);
        if (avcodec_parameters_to_context(codecContext, formatContext->streams[videoStreamIndex]->codecpar) < 0) {
            qDebug() << "Could not copy codec parameters to context";
            return -1;
        }

        if (avcodec_open2(codecContext, codec, nullptr) < 0) {
            qDebug() << "Could not open codec";
            return -1;
        }

        // 打印解码器信息
        qDebug() << "Decoder Information:";
        qDebug() << "Name:" << codec->name;
        qDebug() << "Long Name:" << codec->long_name;
        qDebug() << "Type:" << av_get_media_type_string(codec->type);

        // 打印解码器上下文信息
        qDebug() << "Decoder Context Information:";
        qDebug() << "Codec ID:" << codecContext->codec_id;
        qDebug() << "Codec Type:" << av_get_media_type_string(codecContext->codec_type);

        AVPacket packet;
        av_init_packet(&packet);
        packet.data = nullptr;
        packet.size = 0;

        AVFrame *frame = av_frame_alloc();
        if (frame == nullptr) {
            qDebug() << "Could not allocate video frame";
            return -1;
        }

        struct SwsContext *swsContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
            codecContext->width, codecContext->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);

        if (swsContext == nullptr) {
            qDebug() << "Could not initialize SwsContext";
            return -1;
        }


        while (av_read_frame(formatContext, &packet) >= 0 && this->open_video == true) {
            if (packet.stream_index == videoStreamIndex) {

                //ret = 0 说明返回了解码器
                int ret = avcodec_send_packet(codecContext, &packet);

                //从解码器获得帧后zhen = 0
                //frame存储了解码后的视频信息
                int zhen = avcodec_receive_frame(codecContext, frame);

                //QImage image(frame->data[0], frame->width, frame->height, frame->linesize[0], QImage::Format_RGB888);
                // 创建
                QImage img (frame->width, frame->height, QImage::Format_RGB888);

                uint8_t* dst[] = { img.bits() };
                int dstStride[4];
                // AV_PIX_FMT_RGB24 对应于 QImage::Format_RGB888
                av_image_fill_linesizes(dstStride, AV_PIX_FMT_RGB24, frame->width);
                // 转换
                sws_scale(swsContext, frame->data, (const int*)frame->linesize,
                    0, codecContext->height, dst, dstStride);
//                QImage image = ConvertImage(frame);

                qDebug()<<"frame->data[0] = "<<frame->data[0]<<",frame->width = "<<frame->width<<",frame->height = "<<frame->height<<
                     "frame->linesize[0] = "<<frame->linesize[0]<<",ret = "<< ret<<",帧率 = "<<zhen;


                QPixmap pixmap = QPixmap::fromImage(img);


                // 获取QLabel的大小
                int labelWidth = ui->video_region->width();
                int labelHeight = ui->video_region->height();

                QPixmap rotatedPixmap = pixmap.transformed(QTransform().rotate(180));
                QPixmap scaledPixmap = rotatedPixmap.scaled(labelWidth, labelHeight, Qt::IgnoreAspectRatio);

                ui->video_region->setPixmap(scaledPixmap);

                qApp->processEvents();
                qDebug()<<packet.data<<"   "<<packet.size;

            }
            av_packet_unref(&packet);
        }

        av_frame_free(&frame);
        sws_freeContext(swsContext);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);

        return 0;
    });
}

int Index::initVideo()
{
    int labelWidth = ui->video_region->width();
    int labelHeight = ui->video_region->height();
    // D:/镜流/QQ图片20230730210057.png
    // :/images/waitMe.jpg
    QPixmap pixmap(":/images/waitMe.jpg");
    pixmap = pixmap.scaled(labelWidth, labelHeight, Qt::IgnoreAspectRatio);
    ui->video_region->setPixmap(pixmap);
}

int Index::initVideoPlace()
{
    int windowWidth = this->width();   // 获取窗体的宽度
    int windowHeight = this->height(); // 获取窗体的高度

    qDebug()<< windowWidth<<"   "<< windowHeight;
}

void Index::on_pushButton_3_clicked()
{
    this->open_video = false;
    initVideo();
    initVideoPlace();
}


//打开控制是建立TCP连接
void Index::on_open_control_clicked()
{
    if(on_open_control_flag == false){
        QFuture<void> TCPConnect = QtConcurrent::run([=]() {
            //禁止按钮的使用，避免开启多个多线程
            //ui->open_control->setEnabled(false);

            QString serverAddress = "192.168.31.17"; // 请替换为实际的服务器IP地址
            int port = 6666; // 请替换为实际的端口号
            QString message = "@!!K*"; // 要发送的消息
            QString result = tcpClientConnet(serverAddress,port,message,this->tcpSocket);
            qDebug()<<result;
            if(result == "22\r\n\r"){
                ui->open_control->setText("关闭连接");
                on_open_control_flag = true;
                //receiveData();
            }
            //ui->open_control->setEnabled(true);
        });
    }else {
        //ui->open_control->setEnabled(false);
        if(receiveDataFalag == true){
            // 创建一个消息框
            QMessageBox messageBox;

            // 设置消息框的标题
            messageBox.setWindowTitle("关闭连接");

            // 设置消息框的文本内容
            messageBox.setText("请关闭数据接收，再关闭设备连接");

            // 设置消息框的图标，例如：QMessageBox::Information, QMessageBox::Warning, QMessageBox::Critical, 等等
            messageBox.setIcon(QMessageBox::Warning);

            // 设置消息框的按钮，例如：QMessageBox::Ok, QMessageBox::Yes, QMessageBox::No, 等等
            messageBox.setStandardButtons(QMessageBox::Ok);

            // 显示消息框并等待用户响应
            messageBox.exec();
        }else{
            QFuture<void> TCPDisonnect = QtConcurrent::run([=]() {
                tcpSocket.disconnectFromHost();
                on_open_control_flag = false;
                ui->open_control->setText("打开连接");
            });
        }
    }
}

//左右按钮
void Index::on_key_left_m_pressed()
{

    if(on_open_control_flag == true){
        Left_and_right = 'A';
        tcpSocket.write("@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*");
        tcpSocket.flush();
        qDebug()<<"@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*";
    }
}

void Index::on_key_left_m_released()
{
    if(on_open_control_flag == true){
        Left_and_right = '!';
        keyReleased();
    }
}

void Index::on_key_right_m_pressed()
{
    if(on_open_control_flag == true){
        Left_and_right = 'D';
        tcpSocket.write("@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*");
        tcpSocket.flush();
        qDebug()<<"@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*";
    }
}

void Index::on_key_right_m_released()
{
    if(on_open_control_flag == true){
        Left_and_right = '!';
        keyReleased();
    }
}

//按键释放函数
void Index::keyReleased()
{
    QFuture<void> TCPDisonnect = QtConcurrent::run([=]() {
        tcpSocket.write("@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*");
        tcpSocket.flush();
        qDebug()<<"@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*";
        tcpSocket.write("@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*");
        tcpSocket.flush();
        tcpSocket.write("@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*");
        tcpSocket.flush();
    });
}


//上下按钮
void Index::on_key_up_m_pressed()
{
    if(on_open_control_flag == true){
        Up_and_Down = 'W';
        tcpSocket.write("@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*");
        tcpSocket.flush();
        qDebug()<<"@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*";
    }
}

void Index::on_key_up_m_released()
{
    if(on_open_control_flag == true){
        Up_and_Down = '!';
        keyReleased();
    }
}

void Index::on_key_down_m_pressed()
{
    if(on_open_control_flag == true){
        Up_and_Down = 'S';
        tcpSocket.write("@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*");
        tcpSocket.flush();
        qDebug()<<"@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*";
    }
}

void Index::on_key_down_m_released()
{
    if(on_open_control_flag == true){
        Up_and_Down = '!';
        keyReleased();
    }
}

//按键按下监测
void Index::keyPressEvent(QKeyEvent *event)
{
    // 获取按下的键的键值
    int key = event->key();
    switch (key) {
    case Qt::Key_A:
    {
        if(pressKeyA == false && event->isAutoRepeat() == false)
        {
            on_key_left_m_pressed();
            pressKeyA = true;
            qDebug()<<key;
        }
        break;
    }
    case Qt::Key_D:
    {
        if(pressKeyD == false && event->isAutoRepeat() == false)
        {
            on_key_right_m_pressed();
            pressKeyD = true;
            qDebug()<<key;
        }
        break;
    }
    case Qt::Key_W:
    {
        if(pressKeyS == false && event->isAutoRepeat() == false)
        {
            on_key_up_m_pressed();
            pressKeyW = true;
            qDebug()<<key;
        }
        break;
    }
    case Qt::Key_S:
    {
        if(pressKeyS == false && event->isAutoRepeat() == false)
        {
            on_key_down_m_pressed();
            pressKeyS = true;
            qDebug()<<key;
        }
        break;
    }
    case Qt::Key_E:
    {
        if(event->isAutoRepeat() == false)
        {
            on_key_reset_m_clicked();
            qDebug()<<key;
        }
        break;
    }
    }

    // 调用基类的事件处理函数，传递事件给父类
    //QWidget::keyPressEvent(event);
}

//按键松开
void Index::keyReleaseEvent(QKeyEvent *event)
{
    // 获取松开的键的键值
    int key = event->key();

    // 执行自定义操作，根据松开的键执行不同的逻辑
    switch (key) {
    case Qt::Key_A:
    {
        if(pressKeyA == true && event->isAutoRepeat() == false)
        {
            on_key_left_m_released();
            pressKeyA = false;
            qDebug()<<"re"<<key;
        }
        break;
    }
    case Qt::Key_D:
    {
        if(pressKeyD == true && event->isAutoRepeat() == false)
        {
            on_key_right_m_released();
            pressKeyD = false;
            qDebug()<<key;
        }
        break;
    }
    case Qt::Key_W:
    {
        if(pressKeyW == true && event->isAutoRepeat() == false)
        {
            on_key_up_m_released();
            pressKeyW = false;
            qDebug()<<key;
        }
        break;
    }
    case Qt::Key_S:
    {
        if(pressKeyS == true && event->isAutoRepeat() == false)
        {
            on_key_down_m_released();
            pressKeyS = false;
            qDebug()<<key;
        }
        break;
    }
    }
    //QWidget::keyReleaseEvent(event); // 传递事件给父类
}

//恢复初始状态
void Index::on_key_reset_m_clicked()
{
    QString msg = "@00!*";
    if(on_open_control_flag == true){
        tcpSocket.write(msg.toUtf8());
        tcpSocket.flush();
        qDebug()<< msg;
    }
}

int Index::getTimeAndShow()
{
    QFuture<void> Time_my_p = QtConcurrent::run([=]() {
        while (1) {
            // 获取当前日期和时间
            QDateTime currentDateTime = QDateTime::currentDateTime();

            // 将日期和时间格式化为字符串
            QString currentDateTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");

            QThread::sleep(1);

            ui->Time_my->setText(currentDateTimeString);
        }
    });
}


//接收数据
void Index:: receiveData()
{
    //QFuture<void> receivedata = QtConcurrent::run([=]() {
        int star = 0, stop = 0;
        int flag = 0;
        if(receiveDataFalag == true)
        //while(1)
        {
            if((tcpSocket.state() == QAbstractSocket::ConnectedState) && tcpSocket.waitForReadyRead()) {
                QByteArray responseData = tcpSocket.readAll();
                QString str = responseData;
                for (int i = 0; i < str.size(); i++)
                {
                    if (str[i] == '@')
                    {
                        flag++;
                        star = i;
                    }

                    if (flag != 0 && str[i] == '*')
                    {
                        flag = 0;
                        stop = i;

                        //QString s = str.Substring(star + 1, stop-1);
                        QString s = str.mid(star + 1, stop-1);
                        qDebug()<<"12312312"<<s;
                        QStringList stringList = s.split('#');

                        ui->MQ2_ppm->setText("可燃气体浓度："+stringList[1]+"ppm");

                        ui->MQ_135->setText("空气质量："+stringList[0]);

                        ui->DHT11_btn_H->setText("湿度："+stringList[2]);

                        float MQ_135_data = stringList[0].toFloat();

                        float MQ2_ppm_data = stringList[1].toFloat();

                        float Hui = stringList[2].toFloat();


                        if(MQ_135_data <= 1)
                        {
                            ui->mq135->setValue_m(int(MQ_135_data*ui->mq135->width()));
                        }

                        if(MQ2_ppm_data >= 100)
                        {
                            ui->mq2_h->setValue_m(int(ui->mq2_h->width()));
                        }else
                        {
                            ui->mq2_h->setValue_m(int(ui->mq2_h->width()/100*MQ2_ppm_data));
                        }

                        if(MQ_135_data >= 0.95)
                        {
                            ui->mq135_la->setText("空气:优");
                        }else if(MQ_135_data >= 0.8)
                        {
                            ui->mq135_la->setText("空气:良");
                        }else if(MQ_135_data >= 0.6)
                        {
                            ui->mq135_la->setText("空气:一般");
                        }else if(MQ_135_data >= 0.4)
                        {
                            ui->mq135_la->setText("空气:差");
                        }else{
                            ui->mq135_la->setText("空气:较差");
                        }

                        if(MQ2_ppm_data < 25)
                        {
                            ui->mq2_mpp_la->setText("可燃气:正常");
                        }else {
                            ui->mq2_mpp_la->setText("可燃气:危险");
                        }

                        if((int)Hui <= 100 )
                        {
                            ui->dht11_w->setValue_m(int(Hui)*2 - 2);
                        }

                        break;
                    }
                }
            }
        }
//    });
//    receivedata.waitForFinished();
//    qDebug()<<"接收线程关闭";
}


// 在槽函数中处理接收到的数据
void Index::handleReadyRead()
{
    QByteArray responseData = tcpSocket.readAll();
    QString str = responseData;

    qDebug() << str;
    // 进行数据处理逻辑
    // ...
}

//接收数据函数
void Index::on_startReceiveData_btn_clicked()
{
    ui->startReceiveData_btn->setEnabled(false); // 禁用按钮
    if(tcpSocket.state() == QAbstractSocket::ConnectedState)
    {
        if(receiveDataFalag == false){
            receiveDataFalag = true;
            receiveDataFlagWithHardware = "C";
            tcpSocket.write("@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*");
            tcpSocket.flush();
            //receiveData();
            receiveDataTimer.start();


            qDebug()<<"@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*";
            ui->startReceiveData_btn->setText("停止接收数据");
        }else {
            receiveDataTimer.stop();
            ui->startReceiveData_btn->setText("开始接收数据");
            receiveDataFlagWithHardware = "S";
            tcpSocket.write("@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*");
            tcpSocket.flush();
            qDebug()<<"@"+Up_and_Down.toUtf8()+Left_and_right.toUtf8()+receiveDataFlagWithHardware.toUtf8()+"*";
            receiveDataFalag = false;
        }
    }else {
        // 创建一个消息框
        QMessageBox messageBox;

        // 设置消息框的标题
        messageBox.setWindowTitle("连接");

        // 设置消息框的文本内容
        messageBox.setText("未连接，请连接再打开数据接收");

        // 设置消息框的图标，例如：QMessageBox::Information, QMessageBox::Warning, QMessageBox::Critical, 等等
        messageBox.setIcon(QMessageBox::Warning);

        // 设置消息框的按钮，例如：QMessageBox::Ok, QMessageBox::Yes, QMessageBox::No, 等等
        messageBox.setStandardButtons(QMessageBox::Ok);

        // 显示消息框并等待用户响应
        messageBox.exec();
    }
    ui->startReceiveData_btn->setEnabled(true); // 禁用按钮
}
