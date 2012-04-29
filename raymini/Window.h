#ifndef WINDOW_H
#define WINDOW_H

#include "GLViewer.h"
#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <QCheckBox>
#include <QGroupBox>
#include <QComboBox>
#include <QSlider>
#include <QLCDNumber>
#include <QSpinBox>
#include <QImage>
#include <QLabel>
#include <QPushButton>

#include <vector>
#include <string>

#include "QTUtils.h"
#include "Observer.h"

class Controller;

class Window : public QMainWindow, public Observer {
    Q_OBJECT
    public:
    Window(Controller *);
    virtual ~Window();

    static void showStatusMessage (const QString & msg);

    virtual void update(Observable *);

private :
    Controller *controller;

    void initControlWidget ();

    /** return -1 if no light selected */
    int getSelectedLightIndex();

    // Update functions
    void updateFromScene();
    void updateFromRayTracer();
    void updateFromWindowModel();

    QActionGroup * actionGroup;
    QGroupBox * controlWidget;
    QString currentDirectory;

    // Needed for further actions
    QSpinBox *shadowSpinBox;

    QSpinBox *PTNbRaySpinBox;
    QSpinBox *PTMaxAngleSpinBox;
    QSpinBox *PTIntensitySpinBox;
    QCheckBox *PTOnlyCheckBox;

    QSpinBox *AANbRaySpinBox;

    QSpinBox *AOMaxAngleSpinBox;
    QDoubleSpinBox *AORadiusSpinBox;
    QCheckBox *AOOnlyCheckBox;

    QComboBox *lightsList;
    QCheckBox *lightEnableCheckBox;
    QDoubleSpinBox *lightPosSpinBoxes[3];
    QDoubleSpinBox *lightRadiusSpinBox;
    QDoubleSpinBox *lightIntensitySpinBox;

    QPushButton * selecFocusedObject;
};

#endif // WINDOW_H


// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
