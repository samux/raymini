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

        cmdQ = new CommandQueue(context, devices[0]);

        /*std::vector<Vect> v = mesh.getV();
        std::vector<Tri> t = mesh.getT();

        pixels = (unsigned int *)malloc(sizeof(unsigned int) * pixelCount);
        for(unsigned int i = 0; i < pixelCount; ++i)
            pixels[i] = 0;

        

        cl::Buffer vertBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(Vect) * v.size(), &v[0]);
        cl::Buffer triBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(Tri) * t.size(), &t[0]);
        cl::Buffer pixBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * pixelCount, &pixels[0]);


        kernel.setArg(0, vertBuffer);
        kernel.setArg(1, triBuffer);
        kernel.setArg(2, pixBuffer);
        cmdQ.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(pixelCount), cl::NDRange(4));


        cmdQ.enqueueReadBuffer(pixBuffer, true, 0, sizeof(unsigned int)*pixelCount, &pixels[0]);

        std::cout << "Finished!\n";
        return;*/
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
                        unsigned int * pixBuffer) {


}

