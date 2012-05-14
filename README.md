RayMini ENHANCED
================

Authors
-------

- Tamy Boubekeur (tamy.boubekeur@telecom-paristech.fr)
- Bertrand Chazot (bertrand.chazot@gmail.com)
- Samuel Mokrani (samuel.mokrani@gmail.com)
- Axel Schumacher (axel.schumacher@gmail.com)

Compiling
----------
Required library: GCC >v4, OpenGL, QT >v4.4, libQGLViewer and GLEW (for GPU extensions)
Edit the file raymini.pro to adapt it to your configuration (has been tested under Linux Ubuntu).

On linux: 
- qmake raymini.pro
- make -j9
- ./raymini <scene>

Available scenes: 
-    room: simple room
-    rs: room with sphere
-    rsm: room with mirror sphere
-    rsglas: room with glass sphere
-    rsglos: room with glossy sphere
-    lights: severals light and a ram
-    meshs: severals meshes and a light (moving ram)
-    outdoor: rhino in front of a mirror + sky box
-    pool : pool table
-    mg : mirror and glass
-    sphere : 3 spheres and grey ground (PT)
-    mesh <mesh_path>


Features
--------

- Basic ray tracing with material-specific BRDF
- KDTrees are used to optimize intersection tests
- Anti aliasing
- Hard and soft shadows
- Ambient occlusion
- Mirror materials
- Prism materials
- Path tracing and Point-Based Global Illumination
- Motion blur
- Focal effect
- Texture and normal mapping
- Multi threaded real-time on CPU
- Mesh transformation (rotation, scale)
- User friendly GUI using MVC design pattern
