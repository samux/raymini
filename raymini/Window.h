#ifndef WINDOW_H
#define WINDOW_H

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
#include <QProgressBar>

#include <vector>
#include <string>

#include "GLViewer.h"
#include "QTUtils.h"
#include "Observer.h"
#include "Vec3D.h"

class Controller;

class Window : public QMainWindow, public Observer {
    Q_OBJECT
public:
    Window(Controller *);
    virtual ~Window();

    static void showStatusMessage (const QString & msg);

    virtual void update(Observable *);

    Vec3Df getLightPos() const;
    Vec3Df getLightColor() const;

private :
    Controller *controller;

    void initControlWidget ();

    // Update functions
    void updateFromScene();
    void updateFromRayTracer();
    void updateFromWindowModel();

    // Updated by both WindowModel and Scene
    void updateLights();
    // Updated by both WindowModel and RayTracer
    void updateFocus();
    // Updated by both WindowModel and Scene
    void updateObjects();
    // Updated by RenderThread
    void updateProgressBar();
    // Updated by WindowModel
    void updateRealTime();
    void updateStatus();

    QActionGroup * actionGroup;
    QGroupBox * controlWidget;
    QString currentDirectory;

    // Needed for further actions
    QPushButton *stopRenderButton;
    QPushButton *renderButton;
    QProgressBar *renderProgressBar;
    QCheckBox *realTimeCheckBox;

    QComboBox *shadowTypeList;
    QSpinBox *shadowSpinBox;

    QSpinBox *PTNbRaySpinBox;
    QSpinBox *PTMaxAngleSpinBox;
    QCheckBox *PTOnlyCheckBox;
    QCheckBox *PBGICheckBox;

    QSpinBox *AANbRaySpinBox;

    QSpinBox *AOMaxAngleSpinBox;
    QDoubleSpinBox *AORadiusSpinBox;
    QCheckBox *AOOnlyCheckBox;

    QComboBox *objectsList;
    QCheckBox *objectEnableCheckBox;

    QComboBox *lightsList;
    QCheckBox *lightEnableCheckBox;
    QDoubleSpinBox *lightPosSpinBoxes[3];
    QDoubleSpinBox *lightColorSpinBoxes[3];
    QDoubleSpinBox *lightRadiusSpinBox;
    QDoubleSpinBox *lightIntensitySpinBox;

    QComboBox *focusTypeComboBox;
    QCheckBox *changeFocusFixingCheckBox;
    QSpinBox *focusNbRaysSpinBox;
    QDoubleSpinBox *focusApertureSpinBox;
};

#endif // WINDOW_H


// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
