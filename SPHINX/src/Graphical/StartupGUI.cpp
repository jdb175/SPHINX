#include <QApplication>

#include "graphical/GUI.hpp"

using namespace SPHINXProgram::Graphical;
using namespace std;


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    GUI *program = new GUI();
    program->start();
    app.connect(&app, SIGNAL(aboutToQuit()), program, SLOT(onQuit()));
    return app.exec();
}

