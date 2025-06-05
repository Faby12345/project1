#include <QApplication>
#include "MainWindow.h"
#include "test.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    runAllTests();
    w.show();
    return app.exec();
}
