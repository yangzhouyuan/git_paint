#include "paint_widget.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <memory>
#include <QDebug>
#include <QPushButton>
#include <QColorDialog>
#include "paint_view.h"
#include "paint_scene.h"
#include <QComboBox>
#include <QGroupBox>
#include <QFileDialog>
#include <QRadioButton>
#include <QStandardPaths>


struct impl_paint_widget
{
    paint_scene* scene;
    paint_view* view;
    QRadioButton* freebutton;
    QRadioButton* linebutton;
    QRadioButton* rectbutton;
    QRadioButton* ellipsebutton;
    QRadioButton* circlebutton;
    QRadioButton* polygonbutton;
    QRadioButton* arcbutton;

    QPushButton* save;
    QPushButton* pen_color;
    QPushButton* brush_color;
    QComboBox* pen_thickness;
    QGroupBox* settinggroupbox;
    QPushButton* clearbutton;
};

paint_widget::paint_widget(QWidget *parent)
    : QWidget(parent)
{

}

bool paint_widget::init()
{
    init_data ();
    init_layout ();
    init_button ();
    this->resize(400, 300);

    return true;
}

bool paint_widget::init_data()
{
    data = make_unique<impl_paint_widget> ();

    data->scene = paint_scene::make (QRectF (QPointF (-5, -5), QPointF (205, 165)), this).release();
    data->view =  paint_view::make (data->scene, this).release ();
    data->settinggroupbox = new QGroupBox("Setting", this);

    data->freebutton = new QRadioButton("Free", data->settinggroupbox);
    data->linebutton =  new QRadioButton("Line", data->settinggroupbox);
    data->rectbutton =  new QRadioButton("Rect", data->settinggroupbox);
    data->ellipsebutton = new QRadioButton("Ellipse", data->settinggroupbox);
    data->circlebutton =  new QRadioButton("Circle", data->settinggroupbox);
    data->polygonbutton = new QRadioButton("Polygon", data->settinggroupbox);
    data->arcbutton = new QRadioButton("Arc", data->settinggroupbox);

    data->freebutton->setChecked(true);

    this->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px;");

    data->save = new QPushButton ("Save", this);
//    data->save->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px;");
    data->pen_color = new QPushButton (this);
//    data->pen_color->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px;");
    data->brush_color = new QPushButton ("Brush_Color", this);
//    data->brush_color->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px;");

    data->pen_thickness = new QComboBox (this);
    data->pen_thickness->addItem("细");
    data->pen_thickness->addItem("粗");

    data->clearbutton = new QPushButton("Clear", this);
//    data->clearbutton->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px;");


    return true;
}

bool paint_widget::init_layout()
{
    auto layout = std::make_unique<QHBoxLayout> ();
    auto vlayout = std::make_unique<QVBoxLayout> ();
    auto setting_vlayout = std::make_unique<QVBoxLayout> ();
    setting_vlayout->addWidget(data->freebutton);
    setting_vlayout->addWidget(data->linebutton);
    setting_vlayout->addWidget(data->rectbutton);
    setting_vlayout->addWidget(data->ellipsebutton);
    setting_vlayout->addWidget(data->circlebutton);
    setting_vlayout->addWidget(data->polygonbutton);
    setting_vlayout->addWidget(data->arcbutton);


    data->settinggroupbox->setLayout(setting_vlayout.release());
    vlayout->addWidget(data->settinggroupbox);


    vlayout->addItem (new QSpacerItem (0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding)); //布局管理器对按钮的优化
    vlayout->addWidget(data->clearbutton);
    vlayout->addWidget(data->save);
    vlayout->addWidget(data->pen_color);
    vlayout->addWidget(data->brush_color);
    vlayout->addWidget(data->pen_thickness);



    layout->addLayout(vlayout.release());
    layout->addWidget(data->view);

    this->setLayout(layout.release ());

    return true;
}

unique_ptr<paint_widget> paint_widget::make(QWidget *parent)
{
    auto ret = unique_ptr<paint_widget> (new paint_widget (parent));
    if (ret->init() == false)
    {
        return nullptr;
    }
    return ret;
}

paint_widget::~paint_widget()
{

}

void paint_widget::set_view_status ()
{
    auto button = dynamic_cast<QRadioButton*> (sender ());

    if (button == nullptr)
    {
        return;
    }

    auto button_text = button->text();
    if (button_text == "Free")
    {
        data->view->set_type(paint_view::draw_type::FREE);
    }
    else if (button_text == "Line")
    {
        data->view->set_type(paint_view::draw_type::LINE);
    }
    else if (button_text == "Rect")
    {
        data->view->set_type(paint_view::draw_type::RECT);
    }
    else if (button_text == "Ellipse")
    {
        data->view->set_type(paint_view::draw_type::ELLIPSE);
    }
    else if (button_text == "Circle")
    {
        data->view->set_type(paint_view::draw_type::CIRCLE);
    }
    else if (button_text == "Polygon")
    {
        data->view->set_type(paint_view::draw_type::POLYGON);
    }
}

void paint_widget::init_button()
{
    for (auto button :
    {
         data->circlebutton,
         data->ellipsebutton,
         data->linebutton,
         data->rectbutton,
         data->freebutton,
         data->polygonbutton})
    {
        if (button != nullptr)
        {
            button->setCheckable(true);
            connect(button, &QRadioButton::clicked, this, &paint_widget::set_view_status);
        }
    }

    connect (data->save, &QPushButton::clicked, this, &paint_widget::save_pixmap);
    data->save->setCursor(Qt::PointingHandCursor);

    connect(data->pen_color, &QPushButton::clicked, this, &paint_widget::choose_pen_color);
    connect(data->brush_color, &QPushButton::clicked, this, &paint_widget::choose_brush_color);
    connect(data->pen_thickness, &QComboBox::currentTextChanged, this, &paint_widget::choose_pen_thickness);
    connect (data->clearbutton, &QPushButton::clicked, data->view, &paint_view::clear);
}

void paint_widget::save_pixmap()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                      QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/新建图像文件",
                                                      tr("Images (*.png);; BMP file (*.bmp) ;; xxx (*.xxx)"));
    data->view->pixmap().save(fileName);
}

void paint_widget::choose_brush_color()
{
    QColor color = QColorDialog::getColor ();
    if (color.isValid())
    {
        data->view->set_brush(color);

        QPalette brush_pal;
        brush_pal.setColor(QPalette::ButtonText, color);
        data->brush_color->setPalette(brush_pal);
    }

}

void paint_widget::choose_pen_thickness(const QString &text)
{
    auto pen = data->view->pen();
    pen.setWidthF(text == "粗" ? 3.0 : 1.0);
    data->view->set_pen(pen);
}

void paint_widget::choose_pen_color()
{
    QColor color = QColorDialog::getColor ();
    if (color.isValid())
    {
        auto pen = data->view->pen();
        pen.setColor(color);
        data->view->set_pen(pen);

        QPalette pen_pal;
        pen_pal.setColor(QPalette::ButtonText, color);
        data->pen_color->setPalette(pen_pal);

    }
}
