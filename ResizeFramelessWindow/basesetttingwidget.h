#ifndef BASESETTTINGWIDGET_H
#define BASESETTTINGWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <windows.h>
#include <windowsx.h>



const int g_nBorder = 5;
#define PADDING 2
enum Direction { UP=0, DOWN=1, LEFT, RIGHT, LEFTTOP, LEFTBOTTOM, RIGHTBOTTOM, RIGHTTOP, NONE };

namespace Ui {
class BaseSetttingWidget;
}

class BaseSetttingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BaseSetttingWidget(QWidget *parent = nullptr);
    ~BaseSetttingWidget();


protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

private:
    void region(const QPoint &cursorGlobalPoint);

private:
    Ui::BaseSetttingWidget *ui;
    QPoint dragPosition;   // 窗口移动拖动时需要记住的点
    Direction dir;        // 窗口大小改变时，记录改变方向
    QWidget *m_pMaskLayer = nullptr;
    bool isPressed;
    QPoint move_point;
    QWidget *m_parent = nullptr;

};

#endif // BASESETTTINGWIDGET_H
