#include "paint_view.h"
#include <QWidget>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QDebug>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QColor>
struct impl_paint_view
{
    paint_view::draw_type type = paint_view::draw_type::FREE; //设置初始化绘制类型
    QPointF begin;

    unique_ptr<QGraphicsPathItem> curve_item = nullptr;
    unique_ptr<QGraphicsLineItem> line_item = nullptr;
    unique_ptr<QGraphicsRectItem> rect_item = nullptr;
    unique_ptr<QGraphicsEllipseItem> ellipse_item = nullptr;
    unique_ptr<QGraphicsEllipseItem> circle_item = nullptr;

    vector<unique_ptr<QGraphicsLineItem>> polygon_lines;


    vector<QGraphicsItem*> graphics;

    QPen pen;
    QBrush brush;
};

paint_view::~paint_view()
{

}

paint_view::draw_type paint_view::return_type()
{
    return data->type;
}

void paint_view::set_type(paint_view::draw_type t)
{
    if (t == data->type)
    {
        return;
    }

    data->type = t;
    emit type_changed (t);
}

QPixmap paint_view::pixmap()
{
    QPixmap pix {200, 160};
    pix.fill(Qt::transparent);
    QPainter painter (&pix);

    for (auto & it : data->graphics)
    {
        const auto ptr = it;
        {
            auto line = dynamic_cast<QGraphicsLineItem*>(ptr);
            if (line != nullptr)
            {
                painter.setPen(line->pen());
                painter.drawLine(line->line());
                continue;
            }
        }

        {
            auto rect = dynamic_cast<QGraphicsRectItem*>(ptr);
            if (rect != nullptr)
            {
                painter.setPen(rect->pen());
                painter.setBrush(rect->brush());
                painter.drawRect(rect->rect());
                continue;
            }
        }

        {
            auto ellipse = dynamic_cast<QGraphicsEllipseItem*>(ptr);
            if (ellipse != nullptr)
            {
                painter.setPen(ellipse->pen());
                painter.setBrush(ellipse->brush());
                painter.drawEllipse(ellipse->rect());
                continue;
            }
        }

        {
            auto circle = dynamic_cast<QGraphicsEllipseItem*>(ptr);
            if (circle != nullptr)
            {
                painter.setPen(circle->pen());
                painter.setBrush(circle->brush());
                painter.drawEllipse(circle->rect());
                continue;
            }
        }

        {
            auto path = dynamic_cast<QGraphicsPathItem*>(ptr);
            if (path != nullptr)
            {
                painter.setPen(path->pen());
                painter.setBrush(path->brush());
                painter.drawPath(path->path());
                continue;
            }
        }
    }

    return pix;
}

QPen paint_view::pen()
{
    return data->pen;
}

void paint_view::set_pen(QPen pen)
{
    data->pen = pen;
}

QBrush paint_view::brush()
{
    return data->brush;
}

void paint_view::set_brush(QBrush brush)
{
    data->brush = brush;
}

void paint_view::clear()
{
    if (!data->graphics.empty())
    {
        delete data->graphics.back();
        data->graphics.pop_back();
    }
}


bool paint_view::init()
{
    data = make_unique<impl_paint_view> ();
    setViewportUpdateMode(FullViewportUpdate);
    setMouseTracking(true);
    setRenderHints (QPainter::Antialiasing);

    connect (this, &paint_view::type_changed, [this] (auto && )
    {
        data->curve_item = nullptr;
        data->line_item = nullptr;
        data->rect_item = nullptr;
        data->ellipse_item = nullptr;
        data->circle_item = nullptr;

        data->polygon_lines.clear();
    });


    return true;
}

paint_view::paint_view(QWidget *parent)
    :QGraphicsView (parent)
{

}

paint_view::paint_view(QGraphicsScene *scene, QWidget *parent)
    :QGraphicsView (scene, parent)
{

}



void paint_view::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent (event);

    switch (return_type())
    {
    case paint_view::draw_type::FREE:
        curve_press_event(event);
        break;
    case paint_view::draw_type::LINE:
        straight_press_event(event);
        break;
    case paint_view::draw_type::RECT:
        rect_press_event(event);
        break;
    case paint_view::draw_type::ELLIPSE:
        ellipse_press_event(event);
        break;
    case paint_view::draw_type::CIRCLE:
        circle_press_event(event);
        break;
    case paint_view::draw_type::POLYGON:
        polygon_press_event(event);
        break;
    }


}

void paint_view::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent (event);

    const auto scene_pos = mapToScene(event->pos());
    const QRectF rect (0, 0, 200, 160);
    setCursor(rect.contains(scene_pos) ? Qt::CrossCursor : Qt::ForbiddenCursor);


//    if (!rect.contains(scene_pos))
//    {
//        return;
//    }

    switch (return_type())
    {
    case paint_view::draw_type::FREE:
        curve_move_event(event);
        break;
    case paint_view::draw_type::LINE:
        straight_move_event(event);
        break;
    case paint_view::draw_type::RECT:
        rect_move_event(event);
        break;
    case paint_view::draw_type::ELLIPSE:
        ellipse_move_event(event);
        break;
    case paint_view::draw_type::CIRCLE:
        circle_move_event(event);
        break;
    case paint_view::draw_type::POLYGON:
        polygon_move_event(event);
        break;
    }



}

void paint_view::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent (event);

    switch (return_type())
    {
    case paint_view::draw_type::FREE:
        curve_release_event(event);
        break;
    case paint_view::draw_type::LINE:
        straight_release_event(event);
        break;
    case paint_view::draw_type::RECT:
        rect_release_event(event);
        break;
    case paint_view::draw_type::ELLIPSE:
        ellipse_release_event(event);
        break;
    case paint_view::draw_type::CIRCLE:
        circle_release_event(event);
        break;
    case paint_view::draw_type::POLYGON:
        polygon_release_event(event);
        break;
    }


}

void paint_view::straight_press_event(QMouseEvent *event)
{
    data->begin = mapToScene (event->pos());
}

void paint_view::straight_move_event(QMouseEvent *event)
{
    if (event->buttons() != Qt::LeftButton)
    {
        return;
    }
    const auto pos = mapToScene(event->pos());

    if( data->line_item == nullptr)
    {
        auto straight_item = scene()->addLine(QLineF(data->begin, pos));
        straight_item->setPen(data->pen);
        data->line_item.reset(straight_item);
    }
    else
    {
        data->line_item->setLine(QLineF(data->begin, pos));
    }
}

void paint_view::straight_release_event(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(data->line_item == nullptr)
    {
        return;
    }
    data->graphics.emplace_back (data->line_item.release());
}

void paint_view::curve_press_event(QMouseEvent *event)
{
    data->begin = mapToScene (event->pos());
}

void paint_view::curve_move_event(QMouseEvent *event)
{
    if (! (event->buttons() & Qt::LeftButton))
    {
        return;
    }
    auto pos = mapToScene(event->pos());     //保存鼠标移动后的点
    if (data->curve_item == nullptr)   //判断这个曲线类是否为空
    {
        QPainterPath path (data->begin);     //把鼠标按下的点加入路径
        path.lineTo(pos); //从起始点画直线到鼠标移动后的点

        const auto added_path = scene ()->addPath(path, data->pen); //将这段路径设置到scene中
        data->curve_item.reset(added_path); //将路径设置回对象中
    }
    else
    {
        auto path = data->curve_item->path(); //获取当前的路径
        path.lineTo(pos);  //在此路径后面再加入一条直线到鼠标移动后的点
        data->curve_item->setPath(path);
    }
}

void paint_view::curve_release_event(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(data->curve_item == nullptr)
    {
        return;
    }

    data->graphics.emplace_back (data->curve_item.release());
}

void paint_view::rect_press_event(QMouseEvent *event)
{
    data->begin = mapToScene (event->pos());
}

void paint_view::rect_move_event(QMouseEvent *event)
{
    if (! (event->buttons() & Qt::LeftButton))
    {
        return;
    }

    const auto pos = mapToScene(event->pos());
    const auto x_min = std::min (data->begin.x(), pos.x());
    const auto y_min = std::min (data->begin.y(), pos.y());
    const auto x_max = std::max (data->begin.x(), pos.x());
    const auto y_max = std::max (data->begin.y(), pos.y());
    const QPointF left_top (x_min, y_min);
    const QPointF bottom_right (x_max, y_max);

    if( data->rect_item == nullptr)
    {
        auto rect_item = scene()->addRect(QRectF(left_top, bottom_right));
        rect_item->setPen(data->pen);
        rect_item->setBrush(data->brush);

        data->rect_item.reset(rect_item);
    }
    else
    {
        data->rect_item->setRect(QRectF(left_top, bottom_right));
    }
}

void paint_view::rect_release_event(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(data->rect_item == nullptr)
    {
        return;
    }

    data->graphics.emplace_back (data->rect_item.release());
}

void paint_view::ellipse_press_event(QMouseEvent *event)
{
    data->begin = mapToScene(event->pos());
}

void paint_view::ellipse_move_event(QMouseEvent *event)
{
    if (! (event->buttons() & Qt::LeftButton))
    {
        return;
    }

    const auto pos = mapToScene(event->pos());

    const auto x_min = std::min( data->begin.x(), pos.x());
    const auto y_min = std::min( data->begin.y(), pos.y());
    const auto x_max = std::max( data->begin.x(), pos.x());
    const auto y_max = std::max( data->begin.y(), pos.y());
    const QPointF left_top (x_min, y_min);
    const QPointF bottom_right (x_max, y_max);

    if( data->ellipse_item == nullptr )
    {
        auto ellipse_item = scene()->addEllipse(QRectF(left_top, bottom_right));
        ellipse_item->setPen(data->pen);
        ellipse_item->setBrush(data->brush);

        data->ellipse_item.reset(ellipse_item);
    }
    else
    {
        data->ellipse_item->setRect(QRectF(left_top, bottom_right));
    }
}

void paint_view::ellipse_release_event(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(data->ellipse_item == nullptr)
    {
        return;
    }

    data->graphics.emplace_back (data->ellipse_item.release());
}

void paint_view::circle_press_event(QMouseEvent *event)
{
    data->begin = mapToScene(event->pos());
}

void paint_view::circle_move_event(QMouseEvent *event)
{
    if (! (event->buttons() & Qt::LeftButton))
    {
        return;
    }

    const auto pos = mapToScene(event->pos());
    const qreal len = QLineF (data->begin, pos).length(); //圆的半径
    const QPointF offset (len, len); //坐标的偏移量

    const auto left_top = data->begin - offset;
    const auto bottom_right = data->begin + offset;

    if(data->circle_item == nullptr)
    {
        auto circle_item = scene()->addEllipse(QRectF(left_top, bottom_right));
        circle_item->setPen(data->pen);
        circle_item->setBrush(data->brush);
        data->circle_item.reset(circle_item);
    }
    else
    {
        data->circle_item->setRect(QRectF(left_top, bottom_right));
    }
}

void paint_view::circle_release_event(QMouseEvent *event)
{
    Q_UNUSED(event);
    if( data->circle_item == nullptr)
    {
        return;
    }
    data->graphics.emplace_back (data->circle_item.release());
}

void paint_view::polygon_press_event(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        const auto scene_pos = mapToScene (event->pos());
        data->polygon_lines.emplace_back (
                    scene ()->addLine(QLineF (QPointF (scene_pos), QPointF (scene_pos))));
    }
    else if (event->buttons() == Qt::RightButton)
    {
        if (data->polygon_lines.size() <= 1)
        {
            return;
        }

        QVector<QPointF> points;

        const auto p1 = data->polygon_lines.front()->line().p1();
        points.push_back(p1);

        for (auto & it : data->polygon_lines)
        {
            points.push_back(it->line().p2());
        }

        data->graphics.emplace_back (scene()->addPolygon({points}, data->pen, data->brush));
        data->polygon_lines.clear();
    }
}

void paint_view::polygon_move_event(QMouseEvent *event)
{
    if (data->polygon_lines.empty())
    {
        return;
    }
    const auto line = data->polygon_lines.back()->line();
    const auto p1 = line.p1();
    const auto p2 = line.p2();

    const auto new_p2 = mapToScene(event->pos());
    const auto new_line = QLineF (p1, new_p2);
    data->polygon_lines.back()->setLine(new_line);
}

void paint_view::polygon_release_event(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void paint_view::arc_press_event(QMouseEvent *event)
{
    data->begin = mapToScene(event->pos());
}

void paint_view::arc_move_event(QMouseEvent *event)
{

}

void paint_view::arc_release_event(QMouseEvent *event)
{

}

void paint_view::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(scene ()->sceneRect(), Qt::KeepAspectRatio);
}

void paint_view::leaveEvent(QEvent *event)
{
    QGraphicsView::leaveEvent(event);
}
