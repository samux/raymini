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
        }

        std::vector<Triangle> ts = c->getScene()->getObjects()[0]->getMesh().getTriangles();
        triangles.resize(ts.size());
        for(unsigned int i = 0; i < ts.size(); i++) {
            triangles[i].v1 = ts[i].getVertex(0);
            triangles[i].v2 = ts[i].getVertex(1);
            triangles[i].v3 = ts[i].getVertex(2);
        }

        vertBuffer = new Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                                sizeof(Vert) * vertices.size(), &vertices[0]);
        triBuffer = new Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                                sizeof(Tri) * triangles.size(), &triangles[0]);
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
        Buffer pixBuffer(*context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
                        sizeof(unsigned int) * pixelCount, &pixBuf[0]);

        kernel->setArg(0, *vertBuffer);
        kernel->setArg(1, *triBuffer);
        kernel->setArg(2, pixBuffer);

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

