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

    Vec3Df getObjectPos() const;
    Vec3Df getObjectMobile() const;

    /** Get a program color [0-1] and create a Qt Icon */
    static QIcon createIconFromColor(Vec3Df color);

private :
    Controller *controller;

    void initControlWidget ();

    // Update functions
    void updateFromScene();
    void updateFromRayTracer();
    void updateFromWindowModel();

    // Updated by both WindowModel and Scene
    void updateLights();
    void updateObjects();
    void updateMaterials();
    void updateTextures();
    // Updated by both WindowModel and RayTracer
    void updateFocus();
    void updateRealTime();
    // Updated by RenderThread and WindowModel
    void updateProgressBar();
    void updateStatus();
    // Updated by WindowModel
    void updatePreview();
    // Updated by Scene and RayTracer
    void updateMotionBlur();

    QActionGroup * actionGroup;
    QGroupBox * controlWidget;
    QString currentDirectory;

    // Needed for further actions

    QTabWidget *rayTabs;

    QCheckBox *wireframeCheckBox;
    QComboBox *modeList;
    QCheckBox *surfelsCheckBox;
    QCheckBox *kdtreeCheckBox;

    QWidget *mBlurGroupBox;
    QSpinBox * mBlurNbImagesSpinBox;

    QPushButton *stopRenderButton;
    QPushButton *renderButton;
    QProgressBar *renderProgressBar;
    QCheckBox *realTimeCheckBox;
    QCheckBox *dragCheckBox;
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
    QLabel *objectMaterialLabel;
    QComboBox *objectMaterialsList;

    QComboBox *materialsList;
    QDoubleSpinBox *materialDiffuseSpinBox;
    QDoubleSpinBox *materialSpecularSpinBox;
    QDoubleSpinBox *materialGlossyRatio;
    QLabel *materialTextureLabel;
    QComboBox *materialTexturesList;

    QComboBox *texturesList;
    QPushButton *textureColorButton;

    QComboBox *lightsList;
    QCheckBox *lightEnableCheckBox;
    QDoubleSpinBox *lightPosSpinBoxes[3];
    QPushButton *lightColorButton;
    QDoubleSpinBox *lightRadiusSpinBox;
    QDoubleSpinBox *lightIntensitySpinBox;

    QComboBox *focusTypeComboBox;
    QCheckBox *changeFocusFixingCheckBox;
    QSpinBox *focusNbRaysSpinBox;
    QDoubleSpinBox *focusApertureSpinBox;

    QPushButton * bgColorButton;
};

#endif // WINDOW_H


// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
