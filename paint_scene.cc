#include "paint_scene.h"
#include <QPainter>
#include <QPixmap>


bool paint_scene::init()
{
    data = unique_ptr<impl_paint_scene> (new impl_paint_scene);
    setBackgroundBrush(QColor (230, 230, 230));

    data->background_pix = QPixmap (30, 30);
    data->background_pix.fill(Qt::white);


    //------------------------------------------------------------------//
    QPainter painter (&data->background_pix);  //绘制背景图片
    painter.setPen(QColor (85, 255, 255));
    painter.setBrush(QColor (85, 255, 255));
    painter.drawRect(QRect (0, 0, 15, 15));
    painter.drawRect(QRectF (15, 15, 15, 15));
    //-------------------------------------------------------------------//

    return true;

}

//背景色
void paint_scene::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawBackground(painter, rect);
    painter->setBrush(data->background_pix);

    painter->drawRect(QRectF (0, 0, 200, 160));
}

//前景色
void paint_scene::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawForeground(painter, rect);
    painter->setBrush(Qt::transparent);
    painter->drawRect(QRectF (0, 0, 200, 160));

}

paint_scene::~paint_scene()
{

}
