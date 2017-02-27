#pragma once
#include <QGraphicsScene>
#include <memory>
#include <QPixmap>

using std::unique_ptr;
using std::make_unique;
struct impl_paint_scene;

struct impl_paint_scene
{
    QPixmap background_pix;
};


class paint_scene : public QGraphicsScene
{
public:
    template<typename ... ARGS>
    static unique_ptr<paint_scene> make (ARGS && ... args)
    {
        unique_ptr<paint_scene> ret (new paint_scene (std::forward<ARGS> (args)...));
        if (ret == nullptr or ret->init() == false)
        {
            return nullptr;
        }
        return ret;
    }

    virtual ~paint_scene();
protected:
    template<typename ... ARGS>
    paint_scene (ARGS&& ... args) : QGraphicsScene (std::forward<ARGS> (args)...) {}
    bool init ();

    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground (QPainter * painter, const QRectF & rect) override;



private:
    unique_ptr<impl_paint_scene> data;
};


