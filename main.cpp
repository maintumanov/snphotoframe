#include <QApplication>
#include "photoframe.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    PhotoFrame w;
    w.show();
    return a.exec();
}
