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


class Window : public QMainWindow {
    Q_OBJECT
    public:
    Window();
    virtual ~Window();

    static void showStatusMessage (const QString & msg);

public slots :
    void renderRayImage ();
    void setShadowMode (int);
    void setShadowNbRays (int);
    void setBGColor ();
    void showRayImage ();
    void exportGLImage ();
    void exportRayImage ();
    void about ();
    void changeAntiAliasingType(int index);
    void setNbRayAntiAliasing(int);
    void changeAmbientOcclusionNbRays(int index);
    void setAmbientOcclusionMaxAngle(int);
    void setAmbientOcclusionRadius(double);
    void setAmbientOcclusionIntensity(int);
    void setOnlyAO(bool);
    void enableFocal(bool);
    void setFocal();
    void setDepthPathTracing(int);
    void setNbRayPathTracing(int);
    void setMaxAnglePathTracing(int);
    void setIntensityPathTracing(int);
    void setOnlyPT(bool);
    void setNbImagesSpinBox(int);
    void selectLight(int);
    void enableLight(bool);
    void setLightRadius(double);
    void setLightIntensity(double);
    void setLightPos();

private :
    void initControlWidget ();
    // return -1 if no light selected
    int getSelectedLightIndex();

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

    GLViewer * viewer;
};

#endif // WINDOW_H


// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
