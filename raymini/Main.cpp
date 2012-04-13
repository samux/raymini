#include <QApplication>
#include "Window.h"
#include <QDir>
#include <QPixmap>
#include <QSplashScreen>
#include <QPlastiqueStyle>
#include <QCleanlooksStyle>
#include <string>
#include <iostream>

#include "QTUtils.h"

using namespace std;

int main (int argc, char **argv)
{
  QApplication raymini (argc, argv);
  setBoubekQTStyle (raymini);
  QApplication::setStyle (new QPlastiqueStyle);
  Window * window = new Window ();
  window->setWindowTitle ("RayMini: A minimal raytracer.");
  window->show();
  raymini.connect (&raymini, SIGNAL (lastWindowClosed()), &raymini, SLOT (quit()));
  
  return raymini.exec ();
}

