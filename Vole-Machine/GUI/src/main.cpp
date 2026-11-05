#include <QApplication>
#include <QStringList>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("VoleSimulator");
    QApplication::setOrganizationName("FCAI-CU");

    MainWindow window;
    window.show();

    // Optional: "vole_gui program.txt" opens a program at startup.
    const QStringList arguments = QApplication::arguments();
    if (arguments.size() > 1) window.openProgram(arguments.at(1));

    return app.exec();
}
