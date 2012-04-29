#include <QApplication>
#include <QPlastiqueStyle>

#include "Controller.h"
#include "QTUtils.h"

int main (int argc, char **argv) {
    srand(time(NULL));
    QApplication raymini(argc, argv);
    setBoubekQTStyle (raymini);
    QApplication::setStyle (new QPlastiqueStyle);

    Controller controller(&raymini);
    controller.initAll();

    return raymini.exec ();
}
