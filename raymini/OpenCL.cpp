#include "OpenCL.h"
#include "Controller.h"
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>
//The OpenCL C++ bindings, with exceptions
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>


OpenCL::OpenCL(Controller * c): c(c) {
    try {
        cl::Context context(CL_DEVICE_TYPE_GPU);

        std::ifstream sourceFile("square.cl");
        std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
        cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));

        cl::Program program(context, source);

        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

        try {
            program.build(devices);
        }
        catch (cl::Error& err)
        {
            std::cerr << "Building failed, " << err.what() << "(" << err.err() << ")" 
                << "\nRetrieving build log\n"	
                << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0])
                << "\n";
            return;
        }

        cl::Kernel kernel(program, "squareArray");

        /*cl::CommandQueue cmdQ(context, devices[0]);

        std::vector<Vect> v = mesh.getV();
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
    catch (cl::Error& err)
    {
        std::cerr << "An OpenCL error occured, " << err.what()
            << "\nError num of " << err.err() << "\n";
        return;
    }

}

