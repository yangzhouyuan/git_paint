#pragma once

#include <QWidget>
#include <memory>

using std::unique_ptr;
using std::make_unique;

struct impl_paint_widget;

class paint_widget final: public QWidget
{
    Q_OBJECT
public:
    static unique_ptr<paint_widget> make (QWidget* parent = nullptr);
    ~paint_widget();
protected:
    paint_widget(QWidget *parent = 0);

    bool init ();

    bool init_data ();
    bool init_layout ();
    void init_button ();

private:
    void set_view_status ();
    void uncheck_others ();
    void save_pixmap ();
    void choose_pen_color ();
    void choose_brush_color ();
    void choose_pen_thickness (const QString &text);
    void add_brush_color ();
private:
    unique_ptr<impl_paint_widget> data;
};

