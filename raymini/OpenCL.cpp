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

        std::ifstream sourceFile("kernel.cl");
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

        nb_obj = c->getScene()->getObjects().size();
        vertices.resize(nb_obj);
        triangles.resize(nb_obj);
        nb_vert.resize(nb_obj);
        nb_tri.resize(nb_obj);
        nb_vert_total = 0;
        nb_tri_total = 0;

        unsigned int index_vert = 0;
        unsigned int index_tri = 0;

        for(unsigned int idx_obj = 0; idx_obj < nb_obj; idx_obj++) {
            std::vector<Vertex> vs = c->getScene()->getObjects()[idx_obj]->getMesh().getVertices();
            vertices.resize(vertices.size() + vs.size());
            for(unsigned int i = 0; i < vs.size(); i++) {
                for(unsigned int j = 0; j < 3; j++) {
                    vertices[index_vert + i].p.p[j] = vs[i].getPos()[j];
                    vertices[index_vert + i].n.p[j] = vs[i].getNormal()[j];
                }
            }

            std::vector<Triangle> ts = c->getScene()->getObjects()[idx_obj]->getMesh().getTriangles();
            triangles.resize(triangles.size() + ts.size());
            for(unsigned int i = 0; i < ts.size(); i++) {
                for(unsigned int j = 0; j < 3; j++) {
                    triangles[index_tri + i].v[j] = ts[i].getVertex(j);
                }
            }

            nb_vert[idx_obj] = vs.size();
            nb_tri[idx_obj] = ts.size();

            index_vert += nb_vert[idx_obj];
            index_tri += nb_tri[idx_obj];

            nb_vert_total += nb_vert[idx_obj];
            nb_tri_total += nb_tri[idx_obj];
            std::cout << "OBJ: "  << idx_obj << " NB_TRI: " << nb_tri[idx_obj] << std::endl;
            std::cout << "OBJ: "  << idx_obj << " NB_VERT: " << nb_vert[idx_obj] << std::endl;
        }

        nb_objBuffer = new Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                        sizeof(unsigned int), &nb_obj);

        vertBuffer = new Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                                sizeof(Vert) * nb_vert_total, &vertices[0]);

        nb_vertBuffer = new Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                        sizeof(int)*nb_obj, &nb_vert[0]);

        triBuffer = new Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                                sizeof(Tri) * nb_tri_total, &triangles[0]);

        nb_triBuffer = new Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                        sizeof(unsigned int)*nb_obj, &nb_tri[0]);

        // Light
        nb_light = c->getScene()->getLights().size();
        lights.resize(nb_light);
        for(unsigned int i = 0; i < nb_light; i++) {
            for(unsigned int j = 0; j < 3; j++) {
                lights[i].p[j] = c->getScene()->getLights()[i]->getPos()[j];
            }
        }
        std::cout << "NB LIGHT: " << nb_light << std::endl;

        lightBuffer = new Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                                sizeof(Vec) * nb_light, &lights[0]);

        nb_lightBuffer = new Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                        sizeof(unsigned int), &nb_light);
        

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

        kernel->setArg(0, *nb_objBuffer);
        kernel->setArg(1, *vertBuffer);
        kernel->setArg(2, *nb_vertBuffer);
        kernel->setArg(3, *triBuffer);
        kernel->setArg(4, *nb_triBuffer);
        kernel->setArg(5, pixBuffer);
        kernel->setArg(6, widthBuffer);
        kernel->setArg(7, heightBuffer);
        kernel->setArg(8, camBuffer);
        kernel->setArg(9, *lightBuffer);
        //kernel->setArg(9, *nb_lightBuffer);

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

