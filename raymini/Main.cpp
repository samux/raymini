//#include <QDir>
//#include <QPixmap>
//#include <QSplashScreen>
//#include <QCleanlooksStyle>
//#include <string>
//#include <iostream>

#include "Controller.h"

//using namespace std;

int main (int argc, char **argv) {
    srand(time(NULL));

    Controller controller(argc, argv);
    controller.initAll();

    return controller.exec();
}
