#pragma once
#include <QGraphicsView>
#include <vector>
#include <memory>
#include <QGraphicsPathItem>
#include <QPicture>
#include <QPainter>
#include <QPixmap>
#include <QColor>

using std::vector;
using std::make_unique;
using std::unique_ptr;

struct impl_paint_view;

class paint_view : public QGraphicsView
{
    Q_OBJECT
public:
    enum class draw_type
    {
        FREE,
        LINE,
        RECT,
        ELLIPSE,
        CIRCLE,
        POLYGON
    };
signals:
    void type_changed (paint_view::draw_type);
public:
    template<typename ... ARGS>
    static unique_ptr<paint_view> make (ARGS && ... args)
    {
        unique_ptr<paint_view> ret (new paint_view (std::forward<ARGS> (args)...));
        if (ret == nullptr or ret->init() == false)
        {
            return nullptr;
        }
        return ret;
    }
    ~paint_view () override;


    draw_type return_type ();  //返回绘制类型
    void set_type (draw_type t);
    QPixmap pixmap ();
    QPen pen ();
    void set_pen (QPen pen);
    QBrush brush ();
    void set_brush (QBrush brush);
    void clear ();

protected:
    bool init ();
    paint_view (QWidget *parent = nullptr);
    paint_view (QGraphicsScene *scene, QWidget *parent = nullptr);

    //重写鼠标事件
    void mousePressEvent (QMouseEvent* event) override;
    void mouseMoveEvent (QMouseEvent* event) override;
    void mouseReleaseEvent (QMouseEvent* event) override;

    void resizeEvent (QResizeEvent* event) override;
    void leaveEvent (QEvent* event) override;

private:
    void straight_press_event (QMouseEvent * event);
    void straight_move_event (QMouseEvent * event);
    void straight_release_event (QMouseEvent * event);

    void curve_press_event (QMouseEvent * event);
    void curve_move_event (QMouseEvent* event);
    void curve_release_event (QMouseEvent* event);

    void rect_press_event (QMouseEvent* event);
    void rect_move_event (QMouseEvent* event);
    void rect_release_event (QMouseEvent* event);

    void ellipse_press_event (QMouseEvent* event);
    void ellipse_move_event (QMouseEvent* event);
    void ellipse_release_event (QMouseEvent* event);

    void circle_press_event (QMouseEvent* event);
    void circle_move_event (QMouseEvent* event);
    void circle_release_event (QMouseEvent* event);

    void polygon_press_event (QMouseEvent* event);
    void polygon_move_event (QMouseEvent* event);
    void polygon_release_event (QMouseEvent* event);

    void arc_press_event (QMouseEvent* event);
    void arc_move_event (QMouseEvent* event);
    void arc_release_event (QMouseEvent* event);

private:
    unique_ptr<impl_paint_view> data;

};

