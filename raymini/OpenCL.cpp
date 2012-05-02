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

        std::string deviceName;
        devices[0].getInfo((cl_device_info)CL_DEVICE_NAME, &deviceName);
        std::cout << "NAME: " << deviceName << std::endl;

        cl_device_type deviceType;
        devices[0].getInfo((cl_device_info)CL_DEVICE_TYPE, &deviceType);
        std::cout << "TYPE: " << deviceType << std::endl;

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
            for(unsigned int j = 0; j < 3; j++) {
                vertices[i].p.p[j] = vs[i].getPos()[j];
                vertices[i].n.p[j] = vs[i].getNormal()[j];
            }
        }

        std::vector<Triangle> ts = c->getScene()->getObjects()[0]->getMesh().getTriangles();
        triangles.resize(ts.size());
        for(unsigned int i = 0; i < ts.size(); i++) {
            for(unsigned int j = 0; j < 3; j++) {
                triangles[i].v[j] = ts[i].getVertex(j);
            }
        }

        nb_vert = vertices.size();
        nb_tri = triangles.size();

        vertBuffer = new Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                                sizeof(Vert) * vertices.size(), &vertices[0]);

        nb_vertBuffer = new Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                        sizeof(unsigned int), &nb_vert);

        triBuffer = new Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                                sizeof(Tri) * triangles.size(), &triangles[0]);

        nb_triBuffer = new Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                        sizeof(unsigned int), &nb_tri);

        kernel->setArg(0, *vertBuffer);
        kernel->setArg(1, *nb_vertBuffer);
        kernel->setArg(2, *triBuffer);
        kernel->setArg(3, *nb_triBuffer);
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
        for(unsigned int i = 0; i < 3; i++) {
            cam.pos.p[i] = camPos[i];
            cam.dir.p[i] = viewDirection[i];
            cam.upVector.p[i] = upVector[i];
            cam.rightVector.p[i] = rightVector[i];

        }

        cam.FoV = fieldOfView;
        cam.aspectRatio = aspectRatio;

        Buffer pixBuffer(*context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
                        sizeof(unsigned int) * pixelCount, &pixBuf[0]);

        Buffer widthBuffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                        sizeof(unsigned int), &screenWidth);

        Buffer heightBuffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                        sizeof(unsigned int), &screenHeight);

        Buffer camBuffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                        sizeof(Cam), &cam);

        kernel->setArg(4, pixBuffer);
        kernel->setArg(5, widthBuffer);
        kernel->setArg(6, heightBuffer);
        kernel->setArg(7, camBuffer);

        std::cout << "GPU Started!\n";
        cmdQ->enqueueNDRangeKernel(*kernel, NullRange, NDRange(pixelCount), NDRange(128));
        cmdQ->enqueueReadBuffer(pixBuffer, true, 0, sizeof(unsigned int)*pixelCount, &pixBuf[0]);
        std::cout << "Finished!\n";
    }
    catch (Error& err) {
        std::cerr << "An OpenCL error occured, " << err.what()
            << "\nError num of " << err.err() << "\n";
        return;
    }
}

