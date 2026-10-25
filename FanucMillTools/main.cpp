#include "FanucMTMainWnd.h"
#include "FanucNC.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/qsplashscreen.h>
#include <QtWidgets/qstylefactory.h>
#include <QtWidgets/qstyle.h>

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::cyan);
    // Set remaining dark palette roles (Base, Text, Button, etc.)
    app.setPalette(darkPalette);
    QSplashScreen* splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/res/FanucNC.PNG"));
    splash->show();
    Qt::Alignment topRight = Qt::AlignRight | Qt::AlignTop;
    FanucMTMainWnd MainWnd;
    splash->showMessage(QObject::tr("Loading FANUC MILL TOOLS..."), topRight, Qt::white);
    splash->finish(&MainWnd);
    delete splash;
    MainWnd.show();
    return app.exec();
}
