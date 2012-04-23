TEMPLATE = app
TARGET   = raymini
CONFIG  += qt opengl xml warn_on console release thread
QMAKE_CXXFLAGS += -std=c++0x
QT *= opengl xml
HEADERS = Window.h \
          GLViewer.h \
          QTUtils.h \
          Vertex.h \
          Triangle.h \
          Mesh.h \
          BoundingBox.h \
          Material.h \
          Object.h \
          Light.h \
          Scene.h \
          RayTracer.h \
          Ray.h \
          KDtree.h \
          Noise.h \
          Model.h \
          AntiAliasing.h \
          AmbientOcclusion.h \
          Color.h \
          Shadow.h \
          Brdf.h

SOURCES = Window.cpp \
          GLViewer.cpp \
          QTUtils.cpp \
          Vertex.cpp \
          Triangle.cpp \
          Mesh.cpp \
          BoundingBox.cpp \
          Material.cpp \
          Object.cpp \
          Light.cpp \
          Scene.cpp \
          RayTracer.cpp \
          Ray.cpp \
          KDtree.cpp \
          Brdf.cpp \
          Noise.cpp \
          Model.cpp \
          AntiAliasing.cpp \
          AmbientOcclusion.cpp \
          Color.cpp \
          Shadow.cpp \
          Main.cpp

    DESTDIR=.

win32 {
    INCLUDEPATH += 'C:\Users\boubek\projects\cg\extern\libQGLViewer-2.3.4'
    LIBS += -L"c:\Users\boubek\projects\cg\extern\libQGLViewer-2.3.4\QGLViewer\release" \
            -lQGLViewer2 \
            -lglew32
}
unix {
    LIBS += -lGLEW \
            -lqglviewer-qt4
}

MOC_DIR = .tmp
OBJECTS_DIR = .tmp
