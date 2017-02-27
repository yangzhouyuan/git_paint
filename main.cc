#include "paint_widget.h"
#include <QApplication>
#include "paint_view.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto w = paint_widget::make();
    w->show();

    return a.exec();
}
