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

    Vec3Df getObjectPos() const;
    Vec3Df getObjectMobile() const;

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
    void updateRealTime();
    // Updated by both WindowModel and Scene
    void updateObjects();
    // Updated by RenderThread
    void updateProgressBar();
    // Updated by WindowModel
    void updateStatus();
    // Updated by Scene and RayTracer
    void updateMotionBlur();

    QActionGroup * actionGroup;
    QGroupBox * controlWidget;
    QString currentDirectory;

    // Needed for further actions

    QGroupBox *mBlurGroupBox;
    QSpinBox * mBlurNbImagesSpinBox;

    QPushButton *stopRenderButton;
    QPushButton *renderButton;
    QProgressBar *renderProgressBar;
    QCheckBox *realTimeCheckBox;
    QLabel *durtiestQualityLabel;
    QComboBox *durtiestQualityComboBox;
    QSpinBox *qualityDividerSpinBox;

    QComboBox *shadowTypeList;
    QSpinBox *shadowSpinBox;

    QSpinBox *PTDepthSpinBox;
    QSpinBox *PTNbRaySpinBox;
    QSpinBox *PTMaxAngleSpinBox;
    QCheckBox *PTOnlyCheckBox;
    QCheckBox *PBGICheckBox;
    QSpinBox * PTIntensitySpinBox;

    QSpinBox *AANbRaySpinBox;

    QSpinBox *AONbRaysSpinBox;
    QSpinBox *AOMaxAngleSpinBox;
    QDoubleSpinBox *AORadiusSpinBox;
    QCheckBox *AOOnlyCheckBox;

    QComboBox *objectsList;
    QCheckBox *objectEnableCheckBox;
    QDoubleSpinBox *objectPosSpinBoxes[3];
    QLabel *objectMobileLabel;
    QDoubleSpinBox *objectMobileSpinBoxes[3];
    QComboBox *objectMaterialsList;

    QComboBox *materialsList;
    QDoubleSpinBox *materialDiffuseSpinBox;
    QDoubleSpinBox *materialSpecularSpinBox;
    QDoubleSpinBox *materialColorSpinBoxes[3];
    QDoubleSpinBox *materialGlossyRatio;

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
