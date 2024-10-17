#include "horizontal_m.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

horizontal_m::horizontal_m(QWidget *parent) :
    QWidget(parent)
{
    this->setMouseTracking(true);
    this->station_flag = 0;
}

horizontal_m::~horizontal_m()
{
}

void horizontal_m::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QBrush brush;
    brush.setColor(QColor(240,248,255));
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    QPen pen;
    pen.setColor(Qt::blue);
    painter.setPen(pen);
    painter.setBrush(brush);

    painter.drawRect(0,0,m_width - 1,m_height - 1);

    HroizonalFillColor(painter,station_flag);
}

void horizontal_m::setValue_m(int x)
{
    station_flag = x;
    QWidget::repaint();
}

//设置占比
void horizontal_m::HroizonalFillColor(QPainter &painter,int x)
{
    QBrush brush;
    brush.setColor(QColor(0,191,255));
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    QPen pen;
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawRect(1,1,x-1,12);
}

//刷新大小
void horizontal_m::resizeEvent(QResizeEvent *event) {
    m_width = this->width();
    m_height = this->height();
    qDebug()<<m_width<<"#"<< m_height;
    QWidget::resizeEvent(event);  // 调用基类的处理函数
}

