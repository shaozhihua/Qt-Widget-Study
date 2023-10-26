#include "basesetttingwidget.h"
#include "ui_basesetttingwidget.h"
#include <QPushButton>
#include <QBitmap>
#include <QDebug>
#include <QPainter>

BaseSetttingWidget::BaseSetttingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BaseSetttingWidget),
    m_parent(parent)
{
    ui->setupUi(this);

    this->setMinimumHeight(100);
    this->setMinimumWidth(150);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowSystemMenuHint); // 设置成无边框对话框
    this->setMouseTracking(true);
    ui->widget->setMouseTracking(true);
    ui->widget_2->setMouseTracking(true);
    isPressed = false;
    this->dir = NONE;

    //这个是窗口半透明化的前提。
    setAttribute(Qt::WA_TranslucentBackground);
    /// 所有的方法当设置了父窗体时，则无法再使用鼠标进行缩放窗体
    /////////////////////
    //方法1：这种方法也可行
    //唯一的缺点是必须在该widget上再嵌套一个子widget否则就是透明的。
    //this->setStyleSheet("border-image:url(:/ButtonRes/res/mask_test1.png)");
    //setAttribute(Qt::WA_TranslucentBackground,true);


    ////////////////////
    //
    // 方法2：以下这种方案是可行的，
    // 在界面上创建一个widget，然后在这个widget上布局，
    // 唯一的缺点就是无法缩放这种无标题栏的窗口。
    //m_pMaskLayer = new QWidget(this);
    //m_pMaskLayer->setAttribute(Qt::WA_TransparentForMouseEvents);
    //m_pMaskLayer->setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    //m_pMaskLayer->resize(this->size());//设置窗口大小
    //m_pMaskLayer->setVisible(true);
    //this->stackUnder(m_pMaskLayer);
    //ui->widget->raise();
    connect(ui->pushButton,&QPushButton::clicked,[=](){
        //this->hide();
        qDebug()<<"clicked";
        //this->setGeometry(100,10,600,300);
    });
    /////////////
    //方法3：该方法可行，并且不用套一个子Widget
//    this->setAutoFillBackground(true);
//    QPalette palette = this->palette();
//    palette.setBrush(QPalette::Window, QBrush(QPixmap(":/ButtonRes/res/mask_test1.png")));
//    setPalette(palette);
    //setWindowOpacity(0.45);
    //尽可能别用setwindowOpacity,因为这时整个窗体的透明度，包括上面的控件。
    //////////
    // 方法4：重写绘图事件
    // 重写绘图事件是可以的。并且，不需要额外提供一个子Widget

    // 方法5，修改窗体的样式表background-color: rgba(0, 0, 0, 180);
    // 该方法行不通，大概率是我调用有问题。
    ////////////////
    // 综上所述，重写绘图事件是最好的办法。





}

BaseSetttingWidget::~BaseSetttingWidget()
{
    delete ui;
}

void BaseSetttingWidget::mousePressEvent(QMouseEvent *event)
{

    //move_point = event->globalPos() - pos();
    isPressed = true;
    switch(event->button()) {
    case Qt::LeftButton:
        isPressed = true;
        if(dir != NONE)
        {
            this->mouseGrabber();
        }
        else {
            dragPosition = event->globalPos() - this->frameGeometry().topLeft();
        }
        break;
    case Qt::RightButton:
        this->hide();
        break;
    default:
        QWidget::mousePressEvent(event);
    }




}

void BaseSetttingWidget::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug()<<"move!";
    //临时屏蔽
    //if(isPressed)
    //{
    //    QPoint move_pos = event->globalPos();
    //    move(move_pos - move_point);
    //}

    QPoint gloPoint = event->globalPos();
    // 该tmp是为了防止基窗口没有最大化时而导致的设置窗口缩放问题
    QPoint tmp = QPoint(gloPoint.x() - m_parent->x(),gloPoint.y() - m_parent->y());
    QRect rect = this->rect();

    QPoint tl = mapToParent(rect.topLeft());
    QPoint rb = mapToParent(rect.bottomRight());

    if(!isPressed) {
        this->region(gloPoint);
    } else {

        if(dir != NONE) {
            QRect rMove(tl, rb);
            switch(dir) {
            case LEFT:
                if(rb.x() - tmp.x() <= this->minimumWidth())
                    rMove.setX(tl.x());
                else
                    rMove.setX(tmp.x());
                break;
            case RIGHT:
                rMove.setWidth(tmp.x() - tl.x());
                break;
            case UP:
                if(rb.y() - tmp.y() <= this->minimumHeight())
                    rMove.setY(tl.y());
                else
                    rMove.setY(tmp.y());
                break;
            case DOWN:
                rMove.setHeight(tmp.y() - tl.y());
                break;
            case LEFTTOP:
                if(rb.x() - tmp.x() <= this->minimumWidth())
                    rMove.setX(tl.x());
                else
                    rMove.setX(tmp.x());
                if(rb.y() - tmp.y() <= this->minimumHeight())
                    rMove.setY(tl.y());
                else
                    rMove.setY(tmp.y());
                break;
            case RIGHTTOP:
                rMove.setWidth(tmp.x() - tl.x());
                rMove.setY(tmp.y());
                break;
            case LEFTBOTTOM:
                rMove.setX(tmp.x());
                rMove.setHeight(tmp.y() - tl.y());
                break;
            case RIGHTBOTTOM:
                rMove.setWidth(tmp.x() - tl.x());
                rMove.setHeight(tmp.y() - tl.y());
                break;
            default:
                break;
            }
            this->setGeometry(rMove);
        }
        else {
            move(event->globalPos() - dragPosition);
            event->accept();
        }
    }
    QWidget::mouseMoveEvent(event);




}

void BaseSetttingWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //isPressed = false;
    if(event->button() == Qt::LeftButton) {
        isPressed = false;
        if(dir != NONE) {
            this->releaseMouse();
            this->setCursor(QCursor(Qt::ArrowCursor));
        }
    }
}



void BaseSetttingWidget::paintEvent(QPaintEvent *event)
{
    QPainter dousePainter(this);
    dousePainter.setBrush(QBrush(QColor(0,0, 0, 100)));
    dousePainter.drawRect(this->rect());
}





void BaseSetttingWidget::region(const QPoint &cursorGlobalPoint)
{
    // 获取窗体在屏幕上的位置区域，tl为topleft点，rb为rightbottom点
    QRect rect = this->rect();
    QPoint tl = mapToGlobal(rect.topLeft());
    QPoint rb = mapToGlobal(rect.bottomRight());
    int x = cursorGlobalPoint.x();
    int y = cursorGlobalPoint.y();

    if(tl.x() + PADDING >= x && tl.x() <= x && tl.y() + PADDING >= y && tl.y() <= y) {
        // 左上角
        dir = LEFTTOP;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));  // 设置鼠标形状
    } else if(x >= rb.x() - PADDING && x <= rb.x() && y >= rb.y() - PADDING && y <= rb.y()) {
        // 右下角
        dir = RIGHTBOTTOM;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    } else if(x <= tl.x() + PADDING && x >= tl.x() && y >= rb.y() - PADDING && y <= rb.y()) {
        //左下角
        dir = LEFTBOTTOM;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    } else if(x <= rb.x() && x >= rb.x() - PADDING && y >= tl.y() && y <= tl.y() + PADDING) {
        // 右上角
        dir = RIGHTTOP;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    } else if(x <= tl.x() + PADDING && x >= tl.x()) {
        // 左边
        dir = LEFT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    } else if( x <= rb.x() && x >= rb.x() - PADDING) {
        // 右边
        dir = RIGHT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }else if(y >= tl.y() && y <= tl.y() + PADDING){
        // 上边
        dir = UP;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    } else if(y <= rb.y() && y >= rb.y() - PADDING) {
        // 下边
        dir = DOWN;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }else {
        // 默认
        dir = NONE;
        this->setCursor(QCursor(Qt::ArrowCursor));
    }

}
