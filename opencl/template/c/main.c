/**
 * @file main.c
 * @author Sravan Senthilnathan
 * @brief sample code for opencl experiments
 * @version 0.1
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

// standard library includes
#include <stdio.h>
#include <stdlib.h>

// OpenCL includes
#include <CL/cl.h>

// kernel source size (in bytes)
#define SOURCE_SIZE 4096
#define KERNEL_SOURCE "kernel.cl"


int main(){
    // allocate memory to store sample data

        const int listSize = 1024 * 1024;
        int* vecA = (int*)malloc(sizeof(int) * listSize);
        int* vecB = (int*)malloc(sizeof(int) * listSize);
        for(int i = 0; i < listSize; i++) {
            vecA[i] = i;
            vecB[i] = listSize - i;
        }
    // 

    // fetch kernel

        FILE* fileHandle;
        char* kernelStr;
        size_t sourceSize;
        
        fileHandle = fopen(KERNEL_SOURCE, "r");
        if (!fileHandle) {
            fprintf(stderr, "Failed to load kernel!\n");
            exit(1);
        }
        kernelStr = (char*)malloc(SOURCE_SIZE);
        sourceSize = fread(kernelStr, 1, SOURCE_SIZE, fileHandle);
        fclose(fileHandle);

    // get cl platform and device details

        cl_device_id deviceId = NULL;   
        cl_uint numPlatforms, numDevices;
        cl_int clErr;

    // query the number of platforms

        clErr = clGetPlatformIDs(0, NULL, &numPlatforms);
        if(clErr != CL_SUCCESS){ printf("Error querying number of OpenCL platforms!\n"); exit(1); }

        cl_platform_id* platforms = NULL;
        platforms = (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));

        clErr = clGetPlatformIDs(numPlatforms, platforms, NULL);

    // ID the primary cl compute device (should be the mali gpu here)

        clErr = clGetDeviceIDs(platforms[1], CL_DEVICE_TYPE_GPU, 1, 
                &deviceId, &numDevices);
        if(clErr != CL_SUCCESS){ printf("Error querying OpenCL device ID!\n"); exit(1); }


    // print diagnostic platform and device info

        char platformName[64];
        clErr = clGetPlatformInfo(platforms[1], CL_PLATFORM_NAME, sizeof(platformName), platformName, NULL);
        if(clErr != CL_SUCCESS){ printf("Error querying OpenCL platform name!\n"); exit(1); }
        printf("Platform Name: %s\n", platformName);

        char deviceName[64];
        clErr = clGetDeviceInfo(deviceId, CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
        if(clErr != CL_SUCCESS){ printf("Error querying number of OpenCL device name!\n"); exit(1); }
        printf("Device Name: %s\n", deviceName);
        

    // create a cl context (handle) to the primary device

        cl_context context = clCreateContext(NULL, 1, &deviceId, NULL, NULL, &clErr);
        if(clErr != CL_SUCCESS){ printf("Error creating compute context!\n"); exit(1); }


    // create a command queue for the cl context
        // clCreateCommandQueue is part of the OpenCL v1.2 spec, 
        // and has been deprecated with clCreateCommandQueueWithProperties in OpenCL v2.0

        // enable queue profiling 
        cl_command_queue_properties properties[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
        
        cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, deviceId, properties, &clErr);
        if(clErr != CL_SUCCESS){ printf("Error creating context command queue!\n"); exit(1); }


    // Create memory buffers on the device for each vector and write data to it

        cl_mem objA = clCreateBuffer(context, CL_MEM_READ_ONLY, 
                listSize * sizeof(int), NULL, &clErr);
        if(clErr != CL_SUCCESS){ printf("Error allocating memory obj A!\n"); exit(1); }

        cl_mem objB = clCreateBuffer(context, CL_MEM_READ_ONLY,
                listSize * sizeof(int), NULL, &clErr);
        if(clErr != CL_SUCCESS){ printf("Error allocating memory obj B!\n"); exit(1); }

        cl_mem objC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
                listSize * sizeof(int), NULL, &clErr);
        if(clErr != CL_SUCCESS){ printf("Error allocating memory obj C!\n"); exit(1); }


    // Copy the lists A and B to their respective memory buffers

        clErr = clEnqueueWriteBuffer(command_queue, objA, CL_TRUE, 0,
                listSize * sizeof(int), vecA, 0, NULL, NULL);

        clErr = clEnqueueWriteBuffer(command_queue, objB, CL_TRUE, 0, 
                listSize * sizeof(int), vecB, 0, NULL, NULL);


    // create the program, build it and create the cl kernel

        cl_program program = clCreateProgramWithSource(context, 1, 
                (const char **)&kernelStr, (const size_t *)&sourceSize, &clErr);
        if(clErr != CL_SUCCESS){ printf("Error creating program!\n"); exit(1); }
    
        // Build the program
        clErr = clBuildProgram(program, 1, &deviceId, NULL, NULL, NULL);
        if(clErr != CL_SUCCESS){ printf("Error building program!\n"); exit(1); }
        else{ printf("program built successfully\n"); }

        // Create the OpenCL kernel
        cl_kernel kernel = clCreateKernel(program, "vector_add", &clErr);
        if(clErr != CL_SUCCESS){ printf("Error creating kernel!\n"); exit(1); }
        else{ printf("kernel created successfully\n"); }


    // emplace kernel argument objects

        clErr = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&objA);
        if(clErr != CL_SUCCESS){ printf("Error setting kernel arg 0!\n"); exit(1); }

        clErr = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&objB);
        if(clErr != CL_SUCCESS){ printf("Error setting kernel arg 1!\n"); exit(1); }

        clErr = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&objC);
        if(clErr != CL_SUCCESS){ printf("Error setting kernel arg 2!\n"); exit(1); }

        //clErr = clSetKernelArg(kernel, 3, sizeof(int), &listSize);

    // setup event for time diagnostic
        cl_event event = clCreateUserEvent(context, &clErr);

    // queue the kernel and execute it

        size_t globalObjSize = listSize; // total list object size
        size_t localObjSize = 64; // divided work size

        clErr = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, 
                &globalObjSize, &localObjSize, 0, NULL, &event);
        if(clErr != CL_SUCCESS){ printf("kernel failed to execute!\n"); exit(1); }
        else{ printf("kernel executed successfully\n"); }


    // read out of the result buffer into local memory

        int* vecC = (int*)malloc(sizeof(int) * listSize);
        clErr = clEnqueueReadBuffer(command_queue, objC, CL_TRUE, 0, 
            listSize * sizeof(int), vecC, 0, NULL, NULL);
        if(clErr != CL_SUCCESS){ printf("failed to read from buffer!\n"); exit(1); }
        

    // optional: print the result
        //      for(int i = 0; i < listSize; i++)
        //  printf("%d + %d = %d\n", vecA[i], vecB[i], vecC[i]);


    // release buffers and memory objects

        clFinish(command_queue);
        clFlush(command_queue);
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseMemObject(objA);
        clReleaseMemObject(objB);
        clReleaseMemObject(objC);


    // queue event profiling to check execution time 

        clWaitForEvents(1, &event);

        cl_ulong start = 0, end = 0;
        double exec_time;     
    
        clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
        clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);

        exec_time = end - start;     
    
        printf("Execution time in milliseconds = %0.5f ms\n", (exec_time / 10e6));
        printf("Execution time in seconds = %0.5f s\n", (exec_time / 10e9));          

    return 0;
}