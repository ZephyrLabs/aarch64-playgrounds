/**
 * @file main.c
 * @author Sravan Senthilnathan
 * @brief sample code for opencl experiments
 * @version 0.1
 * @date 2023-07-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

// standard library includes
#include <stdio.h>
#include <stdlib.h>

// OpenCL includes
#include "cl_init.h"

// kernel source
#define KERNEL_SOURCE "kernel.cl"

int main(){
    // allocate memory to store sample data

        const int listSize = 1024;
        int* vecA = (int*)malloc(sizeof(int) * listSize);
        int* vecB = (int*)malloc(sizeof(int) * listSize);
        for(int i = 0; i < listSize; i++) {
            vecA[i] = i;
            vecB[i] = listSize - i;
        }
    // 

    kernel_str* kernel_data = fetchSource(KERNEL_SOURCE);
    cl_int cl_err;
    cl_handle* handle = cl_init(1, 1, CL_DEVICE_TYPE_GPU);

    printf("OpenCL platform: %s\n", handle->platform_name);
    printf("OpenCL device: %s\n", handle->device_name);


    // Create memory buffers on the device for each vector and write data to it

        cl_mem objA = clCreateBuffer(handle->device_context, CL_MEM_READ_ONLY, 
                listSize * sizeof(int), NULL, &cl_err);
        if(cl_err != CL_SUCCESS){ printf("Error allocating memory obj A!\n"); exit(1); }

        cl_mem objB = clCreateBuffer(handle->device_context, CL_MEM_READ_ONLY,
                listSize * sizeof(int), NULL, &cl_err);
        if(cl_err != CL_SUCCESS){ printf("Error allocating memory obj B!\n"); exit(1); }

        cl_mem objC = clCreateBuffer(handle->device_context, CL_MEM_WRITE_ONLY, 
                listSize * sizeof(int), NULL, &cl_err);
        if(cl_err != CL_SUCCESS){ printf("Error allocating memory obj C!\n"); exit(1); }


    // Copy the lists A and B to their respective memory buffers

        cl_err = clEnqueueWriteBuffer(handle->device_queue, objA, CL_TRUE, 0,
                listSize * sizeof(int), vecA, 0, NULL, NULL);

        cl_err = clEnqueueWriteBuffer(handle->device_queue, objB, CL_TRUE, 0, 
                listSize * sizeof(int), vecB, 0, NULL, NULL);

        // create the program, build it and create the cl kernel

        cl_program program = clCreateProgramWithSource(handle->device_context, 1, 
                (const char **)&kernel_data->kernel_code, (const size_t *)&kernel_data->size, &cl_err);
        if(cl_err != CL_SUCCESS){ printf("Error creating program!\n"); exit(1); }
    
        // Build the program
        cl_err = clBuildProgram(program, 1, &handle->device_id, NULL, NULL, NULL);
        if(cl_err != CL_SUCCESS){ printf("Error building program!\n"); exit(1); }
        else{ printf("program built successfully\n"); }

        // Create the OpenCL kernel
        cl_kernel kernel = clCreateKernel(program, "vector_add", &cl_err);
        if(cl_err != CL_SUCCESS){ printf("Error creating kernel!\n"); exit(1); }
        else{ printf("kernel created successfully\n"); }


    // emplace kernel argument objects

        cl_err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&objA);
        if(cl_err != CL_SUCCESS){ printf("Error setting kernel arg 0!\n"); exit(1); }

        cl_err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&objB);
        if(cl_err != CL_SUCCESS){ printf("Error setting kernel arg 1!\n"); exit(1); }

        cl_err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&objC);
        if(cl_err != CL_SUCCESS){ printf("Error setting kernel arg 2!\n"); exit(1); }

        //cl_err = clSetKernelArg(kernel, 3, sizeof(int), &listSize);

    // setup event for time diagnostic
        cl_event event = clCreateUserEvent(handle->device_context, &cl_err);

    // queue the kernel and execute it

        size_t globalObjSize = listSize; // total list object size
        size_t localObjSize = 64; // divided work size

        cl_err = clEnqueueNDRangeKernel(handle->device_queue, kernel, 1, NULL, 
                &globalObjSize, &localObjSize, 0, NULL, &event);
        if(cl_err != CL_SUCCESS){ printf("kernel failed to execute!\n"); exit(1); }
        else{ printf("kernel executed successfully\n"); }


    // read out of the result buffer into local memory

        int* vecC = (int*)malloc(sizeof(int) * listSize);
        cl_err = clEnqueueReadBuffer(handle->device_queue, objC, CL_TRUE, 0, 
            listSize * sizeof(int), vecC, 0, NULL, NULL);
        if(cl_err != CL_SUCCESS){ printf("failed to read from buffer!\n"); exit(1); }

    // queue event profiling to check execution time 

        clWaitForEvents(1, &event);

        cl_ulong start = 0, end = 0;
        double exec_time;     
    
        clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
        clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);

        exec_time = end - start;     
    
        printf("Execution time in milliseconds = %0.5f ms\n", (exec_time / 10e6));
        printf("Execution time in seconds = %0.5f s\n", (exec_time / 10e9));   

        cl_deinit(handle);       
}