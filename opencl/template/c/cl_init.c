/**
 * @file cl_init.c
 * @author Sravan Senthilnathan
 * @brief OpenCL Automatic Setup
 * @version 0.1
 * @date 2023-07-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "cl_init.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// OpenCL includes
#include <CL/cl.h>
  
void cl_deinit(cl_handle* handle){
  clFinish(handle->device_queue);
  clFlush(handle->device_queue);

  clReleaseCommandQueue(handle->device_queue);
  clReleaseContext(handle->device_context);
  clReleaseDevice(handle->device_id);

  free(handle->platform_name);
  free(handle->device_name);
}

/**
 * @brief Fetch the kernel source and create a string type object
 * 
 * @param path path of the kernel
 * @return kernel_str* 
 */
kernel_str* fetchSource(char* path){
  kernel_str* str = (kernel_str*)malloc(sizeof(kernel_str));

  FILE* file_handle;
  char* kernel_code;
  size_t source_size;
  
  file_handle = fopen(path, "r");
  if (!file_handle) {
      fprintf(stderr, "Failed to load kernel!\n");
      exit(1);
  }
  kernel_code = (char*)malloc(SOURCE_SIZE);
  source_size = fread(kernel_code, 1, SOURCE_SIZE, file_handle);
  fclose(file_handle);

  str->kernel_code = kernel_code;
  str->size = source_size;

  return str;
}

/**
 * @brief Query the available openCL platforms
 * 
 * @return cl_platform_id* 
 */
cl_platform_id* queryClPlatforms(){
  cl_uint num_platforms;
  cl_int cl_err;
  
  cl_err = clGetPlatformIDs(0, NULL, &num_platforms);
  if(cl_err != CL_SUCCESS){ printf("Error querying number of OpenCL platforms!\n"); exit(1); }

  cl_platform_id* platforms = NULL;
  platforms = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id));

  cl_err = clGetPlatformIDs(num_platforms, platforms, NULL);

  return platforms;
}

/**
 * @brief Query the openCL devices and fetch their ID
 * 
 * @param platforms list of openCL platforms
 * @param platform_num the index of the platform
 * @param device_type the type of openCL compute device (CPU/GPU/TPU)
 * @param device_num the index of the device
 * @return cl_device_id 
 */
cl_device_id queryClDevice(cl_platform_id* platforms, int platform_num,  int device_type, int device_num){
  cl_uint num_devices;
  cl_int cl_err;

  cl_device_id device_id = NULL; 
  cl_err = clGetDeviceIDs(platforms[platform_num], device_type, device_num, 
          &device_id, &num_devices);
  if(cl_err != CL_SUCCESS){ printf("Error querying OpenCL device ID!\n"); exit(1); }

  return device_id;
}

/**
 * @brief Fetch the name of the openCL platform
 * 
 * @param platforms list of openCL platforms
 * @param platform_num the index of the platform
 * @return char* pointer to name string
 */
char* queryClPlatformName(cl_platform_id* platforms, int platform_num){
  cl_int cl_err;
  char platform_name[64];

  cl_err = clGetPlatformInfo(platforms[platform_num], CL_PLATFORM_NAME, sizeof(platform_name), platform_name, NULL);
  if(cl_err != CL_SUCCESS){ printf("Error querying OpenCL platform name!\n"); exit(1); }

  char* platform_name_ptr = (char*)malloc(64 * sizeof(char));
  strcpy(platform_name_ptr, platform_name);

  return platform_name_ptr;
}

/**
 * @brief Fetch the name of the openCL device
 * 
 * @param device_id the openCL device id
 * @return char* pointer to name string
 */
char* queryClDeviceName(cl_device_id device_id){
  cl_int cl_err;
  char device_name[64];
  
  cl_err = clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(device_name), device_name, NULL);
  if(cl_err != CL_SUCCESS){ printf("Error querying number of OpenCL device name!\n"); exit(1); }

  char* device_name_ptr = (char*)malloc(64 * sizeof(char));
  strcpy(device_name_ptr, device_name);

  return device_name_ptr;
}

/**
 * @brief Create a openCL Context object
 * 
 * @param device_id the openCL device id
 * @return cl_context 
 */
cl_context createContext(cl_device_id device_id){
  cl_int cl_err;

  cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &cl_err);
  if(cl_err != CL_SUCCESS){ printf("Error creating compute context!\n"); exit(1); }

  return context;
}

/**
 * @brief Create a openCL Command Queue object
 * 
 * @param context the openCL context of the device
 * @param device_id the openCL device id
 * @return cl_command_queue 
 */
cl_command_queue createCommandQueue(cl_context context, cl_device_id device_id){
  cl_int cl_err;
  // clCreateCommandQueue is part of the OpenCL v1.2 spec, 
  // and has been deprecated with clCreateCommandQueueWithProperties in OpenCL v2.0

  // enable queue profiling 
  cl_command_queue_properties properties[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
  
  cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, properties, &cl_err);
  if(cl_err != CL_SUCCESS){ printf("Error creating context command queue!\n"); exit(1); }

  return command_queue;
}

/**
 * @brief Initialize creating the openCL handle
 * 
 * @param platform_num the platform index number
 * @param device_num the device index number
 * @param device_type the device type (CL_DEVICE_TYPE_CPU/GPU/TPU)
 * @return cl_handle* the handle itself
 */
cl_handle* cl_init(int platform_num, int device_num, int device_type){
  cl_handle* handle = (cl_handle*)malloc(sizeof(cl_handle));
  
  cl_platform_id* platforms = queryClPlatforms();

  handle->device_id = queryClDevice(platforms, platform_num, device_type, device_num);

  handle->platform_name = queryClPlatformName(platforms, platform_num);
  handle->device_name = queryClDeviceName(handle->device_id);

  handle->device_context = createContext(handle->device_id);
  handle->device_queue = createCommandQueue(handle->device_context, handle->device_id);

  return handle;
}

