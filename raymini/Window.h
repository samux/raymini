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
#include <QLineEdit>

#include <vector>
#include <string>

#include "GLViewer.h"
#include "QTUtils.h"
#include "Observer.h"
#include "Vec3D.h"
#include "NamedClass.h"

class Controller;

class Window : public QMainWindow, public Observer {
    Q_OBJECT
public:
    Window(Controller *, MiniGLViewer *);
    virtual ~Window();

    static void showStatusMessage (const QString & msg);

    virtual void update(const Observable *);

    Vec3Df getLightPos() const;

    Vec3Df getObjectPos() const;
    Vec3Df getObjectMobile() const;

    Vec3Df getNoiseTextureOffset() const;

    /** Get a program color [0-1] and create a Qt Icon */
    static QIcon createIconFromColor(Vec3Df color);

private :
    Controller *controller;

    void initControlWidget ();

    // Update functions
    void updateLights(const Observable *);
    void updateObjects(const Observable *);
    void updateMesh(const Observable *);
    void updateMaterials(const Observable *);
    void updateColorTextures(const Observable *);
    void updateNormalTextures(const Observable *);
    void updateMapping(const Observable *);
    void updateFocus(const Observable *);
    void updateRealTime(const Observable *);
    void updateProgressBar(const Observable *);
    void updateStatus(const Observable *);
    void updatePreview(const Observable *);
    void updateMotionBlur(const Observable *);
    void updateShadows(const Observable *);
    void updateAntiAliasing(const Observable *);
    void updateAmbientOcclusion(const Observable *);
    void updatePathTracing(const Observable *);
    void updateBackgroundColor(const Observable *);

    QActionGroup * actionGroup;
    QGroupBox * controlWidget;
    QString currentDirectory;

    /**
     * Update all fields on a QComboBox
     * Do leave type empty if "no type selected" field not wanted
     * T has to be NamedClass
     */
    template <typename T>
    static void updateList(QComboBox *, const std::vector<T*>&, int index, QString type=QString());

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
    QCheckBox *PTOnlyCheckBox;
    QCheckBox *PBGICheckBox;
    QDoubleSpinBox * PTIntensitySpinBox;

    QSpinBox *AANbRaySpinBox;

    QSpinBox *AONbRaysSpinBox;
    QSpinBox *AOMaxAngleSpinBox;
    QDoubleSpinBox *AORadiusSpinBox;
    QCheckBox *AOOnlyCheckBox;

    QComboBox *objectsList;
    QCheckBox *objectEnableCheckBox;
    QLabel *objectNameLabel;
    QLineEdit *objectNameEdit;
    QDoubleSpinBox *objectPosSpinBoxes[3];
    QLabel *objectMobileLabel;
    QDoubleSpinBox *objectMobileSpinBoxes[3];
    QLabel *objectMaterialLabel;
    QComboBox *objectMaterialsList;

    QComboBox *meshesList;
    QLabel *meshViewerLabel;
    MiniGLViewer *meshViewer;
    QPushButton *meshLoadOffButton;
    QPushButton *meshLoadSquareButton;
    QPushButton *meshLoadCubeButton;

    QComboBox *mappingObjectsList;
    QDoubleSpinBox *mappingUScale;
    QDoubleSpinBox *mappingVScale;
    QPushButton *mappingSphericalPushButton;
    QPushButton *mappingSquarePushButton;
    QPushButton *mappingCubePushButton;

    QComboBox *materialsList;
    QLabel *materialNameLabel;
    QLineEdit *materialNameEdit;
    QDoubleSpinBox *materialDiffuseSpinBox;
    QDoubleSpinBox *materialSpecularSpinBox;
    QDoubleSpinBox *materialGlossyRatio;
    QLabel *materialColorTextureLabel;
    QComboBox *materialColorTexturesList;
    QLabel *materialNormalTextureLabel;
    QComboBox *materialNormalTexturesList;
    QDoubleSpinBox *glassAlphaSpinBox;

    QComboBox *colorTexturesList;
    QLabel *colorTextureNameLabel;
    QLineEdit *colorTextureNameEdit;
    QPushButton *colorTextureColorButton;
    QLabel *colorTextureTypeLabel;
    QComboBox *colorTextureTypeList;
    QPushButton *colorTextureFileButton;
    QLabel *colorTextureNoiseLabel;
    QComboBox *colorTextureNoiseList;

    QComboBox *normalTexturesList;
    QLabel *normalTextureNameLabel;
    QLineEdit *normalTextureNameEdit;
    QLabel *normalTextureTypeLabel;
    QComboBox *normalTextureTypeList;
    QPushButton *normalTextureFileButton;
    QLabel *normalTextureNoiseOffsetLabel;
    QDoubleSpinBox *normalTextureNoiseOffsetSpinBox[3];
    QLabel *normalTextureNoiseTypeLabel;
    QComboBox *normalTextureNoiseTypeList;

    QComboBox *lightsList;
    QPushButton *lightAddButton;
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
