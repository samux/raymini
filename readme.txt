RayMini ENHANCED
================

Authors
-------

Tamy Boubekeur (tamy.boubekeur@telecom-paristech.fr)
Bertrand Chazot (bertrand.chazot@gmail.com)
Samuel Mokrani (samuel.mokrani@gmail.com)
Axel Schumacher (axel.schumacher@gmail.com)

Compiling
----------
Required library: GCC >v4, OpenGL, QT >v4.4, libQGLViewer and GLEW (for GPU extensions)
Edit the file raymini.pro to adapt it to your configuration (has been tested under Linux Ubuntu).

On linux: 
 qmake raymini.pro
 make -j9
 ./raymini

Features
--------

- Basic ray tracing with material-specific BRDF
- KDTrees are used to optimize intersection tests
- User friendly GUI using MVC design pattern
- Anti aliasing
- Hard and soft shadows
- Ambient occlusion
- Mirror materials
- Prism materials
- Path tracing and Point-Based Global Illumination
- Motion blur
- Focal effect
