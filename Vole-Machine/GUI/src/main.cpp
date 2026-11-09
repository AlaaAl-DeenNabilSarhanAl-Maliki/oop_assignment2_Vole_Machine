// main.cpp
#include <QApplication>
#include <QCoreApplication>
#include <QStringList>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Must match the names used by QSettings in MainWindow (Vole / VoleGui).
    QCoreApplication::setOrganizationName("Vole");
    QCoreApplication::setApplicationName("VoleGui");

    MainWindow window;
    window.show();

    // "VoleGui program.txt" or "Open with..." from the file manager.
    const QStringList args = QCoreApplication::arguments();
    if (args.size() > 1)
        window.openProgram(args.at(1));

    return app.exec();
}