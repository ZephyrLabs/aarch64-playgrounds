/**
 * @file cl_init.h
 * @author Sravan Senthilnathan
 * @brief OpenCL Automatic Setup header
 * @version 0.1
 * @date 2023-07-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#define SOURCE_SIZE 4096

#include <CL/cl.h>

/* cl queue and context handle */
typedef struct cl_handle{
  char* platform_name;
  char* device_name;

  cl_device_id device_id;   
  cl_context device_context;
  cl_command_queue device_queue;
} cl_handle;

typedef struct kernel_str{
  char* kernel_code;
  int size;
} kernel_str;

cl_handle* cl_init(int platform_num, int device_num, int device_type);
void cl_deinit(cl_handle* handle);
kernel_str* fetchSource(char* path);
