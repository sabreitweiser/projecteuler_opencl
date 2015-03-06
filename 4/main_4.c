#include <stdio.h>
#include <stdlib.h>

#include <OpenCL/opencl.h>

#define RANGE 999

void errchk(cl_int error, char *location){
	if (error != CL_SUCCESS){
		printf("Error at %s; error %d\n", location, error);
		exit(error);
    }
}

int main(){
	cl_int error;
	cl_platform_id platform;
	cl_context context;
	cl_command_queue queue;
	cl_device_id device;

	error = clGetPlatformIDs(1, &platform, NULL);
	errchk(error, "oclGetPlatformID");

	error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	errchk(error, "clGetDeviceIDs");

	context = clCreateContext(0, 1, &device, NULL, NULL, &error);
	errchk(error, "clCreateContext");

	queue = clCreateCommandQueue(context, device, 0, &error);
	errchk(error, "clCreateCommandQueue");

	//Allocate some local memory space for the results
	int *loc_largest_pals = (int *)malloc(sizeof(int) * RANGE);

	//Allocate OpenCL memory for results
	cl_mem largest_pals = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
			sizeof(cl_int) * RANGE, NULL, &error);
	errchk(error, "clCreateBuffer");

	FILE *src = fopen("kernel_4.cl", "r");
	fseek(src, 0, SEEK_END);
	size_t src_size = ftell(src);
	char *source = (char *)malloc(sizeof(char)*(src_size+1));
	rewind(src);
	fread(source, sizeof(char), src_size, src);
	source[src_size] = '\0';
    
	cl_program program = clCreateProgramWithSource(context, 1, &source,
							&src_size, &error);
	errchk(error, "clCreateProgramWithSource");

	error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);

	char *build_log;
	size_t log_size;
	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL,
				       &log_size);
	build_log = (char *)malloc(sizeof(char) * (log_size+1));
	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
				       log_size, build_log, NULL);
	build_log[log_size] = '\0';
	if (error != CL_SUCCESS)
		printf("BUILD LOG:\n%s\n", build_log);
	free(build_log);
    
	errchk(error, "clBuildProgram");

	cl_kernel largest_pal_kernel = clCreateKernel(program, "largest_pal",
							&error);
	errchk(error, "clCreateKernel");

	error = clSetKernelArg(largest_pal_kernel, 0, sizeof(cl_mem),
				&largest_pals);
	errchk(error, "clSetKernelArg 0");

	const size_t local_ws = 256;
	size_t global_ws = 0;
	while (global_ws < RANGE)
		global_ws += local_ws;
	error = clEnqueueNDRangeKernel(queue, largest_pal_kernel, 1, NULL,
					&global_ws, &local_ws, 0, NULL, NULL);
	errchk(error, "clEnqueueNDRangeKernel");

	clEnqueueReadBuffer(queue, largest_pals, CL_TRUE, 0,
				sizeof(cl_int) * RANGE,
				loc_largest_pals, 0, NULL, NULL);

	int i;
	int max = 0;
	for(i=0; i < RANGE; i++)
		if (loc_largest_pals[i] >= max)
			max = loc_largest_pals[i];
	printf("%d\n", max);

	free(loc_largest_pals);
	clReleaseKernel(largest_pal_kernel);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
	clReleaseMemObject(largest_pals);
}
