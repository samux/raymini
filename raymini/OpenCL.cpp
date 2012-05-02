#include "OpenCL.h"
#include "Controller.h"
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>

using namespace cl;

OpenCL::OpenCL(Controller * c): c(c) {
    try {
        context = new Context(CL_DEVICE_TYPE_GPU);

        std::ifstream sourceFile("square.cl");
        std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
        Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));

        program = new Program(*context, source);

        std::vector<Device> devices = context->getInfo<CL_CONTEXT_DEVICES>();

        try {
            program->build(devices);
        }
        catch (Error& err)
        {
            std::cerr << "Building failed, " << err.what() << "(" << err.err() << ")" 
                << "\nRetrieving build log\n"	
                << program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0])
                << "\n";
            return;
        }

        kernel = new Kernel(*program, "squareArray");

        cmdQ = new CommandQueue(*context, devices[0]);

        std::vector<Vertex> vs = c->getScene()->getObjects()[0]->getMesh().getVertices();
        vertices.resize(vs.size());
        for(unsigned int i = 0; i < vs.size(); i++) {
            vertices[i].v1 = vs[i].getPos()[0];
            vertices[i].v2 = vs[i].getPos()[1];
            vertices[i].v3 = vs[i].getPos()[2];
            std::cout << vertices[i].v2 << std::endl;
        }

        std::vector<Triangle> ts = c->getScene()->getObjects()[0]->getMesh().getTriangles();
        triangles.resize(ts.size());
        for(unsigned int i = 0; i < ts.size(); i++) {
            triangles[i].v1 = ts[i].getVertex(0);
            triangles[i].v2 = ts[i].getVertex(1);
            triangles[i].v3 = ts[i].getVertex(2);
        }
    }
    catch (Error& err)
    {
        std::cerr << "An OpenCL error occured, " << err.what()
            << "\nError num of " << err.err() << "\n";
        return;
    }

}

void OpenCL::getImage ( const Vec3Df & camPos,
        const Vec3Df & viewDirection,
        const Vec3Df & upVector,
        const Vec3Df & rightVector,
        float fieldOfView,
        float aspectRatio,
        unsigned int screenWidth,
        unsigned int screenHeight,
        unsigned int * pixBuf) {

    unsigned int pixelCount = screenHeight*screenWidth;

    try {
        Cam cam;
        cam.pos.v1 = camPos[0];
        cam.pos.v2 = camPos[1];
        cam.pos.v3 = camPos[2];

        cam.dir.v1 = viewDirection[0];
        cam.dir.v2 = viewDirection[1];
        cam.dir.v3 = viewDirection[2];

        cam.upVector.v1 = upVector[0];
        cam.upVector.v2 = upVector[1];
        cam.upVector.v3 = upVector[2];

        cam.rightVector.v1 = rightVector[0];
        cam.rightVector.v2 = rightVector[1];
        cam.rightVector.v3 = rightVector[2];

        cam.FoV = fieldOfView;
        cam.aspectRatio = aspectRatio;

        std::cout << rightVector << std::endl;
        std::cout << upVector << std::endl;

        Buffer vertBuffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                                sizeof(Vert) * vertices.size(), &vertices[0]);
        Buffer triBuffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                                sizeof(Tri) * triangles.size(), &triangles[0]);

        Buffer pixBuffer(*context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
                        sizeof(unsigned int) * pixelCount, &pixBuf[0]);

        Buffer widthBuffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                        sizeof(unsigned int), &screenWidth);

        Buffer heightBuffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                        sizeof(unsigned int), &screenHeight);

        Buffer camBuffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                        sizeof(Cam), &cam);

        kernel->setArg(0, vertBuffer);
        kernel->setArg(1, triBuffer);
        kernel->setArg(2, pixBuffer);
        kernel->setArg(3, widthBuffer);
        kernel->setArg(4, heightBuffer);
        kernel->setArg(5, camBuffer);

        cmdQ->enqueueNDRangeKernel(*kernel, NullRange, NDRange(pixelCount), NDRange(4));
        cmdQ->enqueueReadBuffer(pixBuffer, true, 0, sizeof(unsigned int)*pixelCount, &pixBuf[0]);

        std::cout << "Finished!\n";
    }
    catch (Error& err) {
        std::cerr << "An OpenCL error occured, " << err.what()
            << "\nError num of " << err.err() << "\n";
        return;
    }
}

