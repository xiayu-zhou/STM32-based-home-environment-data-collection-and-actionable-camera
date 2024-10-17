#ifndef HORIZONTAL_M_H
#define HORIZONTAL_M_H

#include <QWidget>

class horizontal_m : public QWidget
{
    Q_OBJECT

public:
    explicit horizontal_m(QWidget *parent = nullptr);
    ~horizontal_m();
    void paintEvent(QPaintEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void setValue_m(int x);

private:
    void HroizonalFillColor(QPainter &painter,int x);

    int m_width = this->width();
    int m_height = this->height();
    int station_flag;

private slots:
    signals:
};

#endif // HORIZONTAL_M_H
