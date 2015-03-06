#include <stdio.h>
#include <stdlib.h>

#include <OpenCL/opencl.h>

#define RANGE 10

void errchk(cl_int error, char *location){
	if (error != CL_SUCCESS){
		printf("Error at %s\n", location);
		exit(error);
	}
}

int main(){
	cl_int error;
	cl_platform_id platform;
	cl_context context;
	cl_command_queue queue;
	cl_device_id device;

	error = oclGetPlatformID(&platform);
	errchk(error, "oclGetPlatformID");

	error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	errchk(error, "clGetDeviceIDs");

	context = clCreateContext(0, 1, &device, NULL, NULL, &error);
	errchk(error, "clCreateContext");

	queue = clCreateCommandQueue(context, device, 0, error);
	errchk(error, "clCreateCommandQueue");

	//Allocate some local memory space for the results
	int *loc_divs = (int *)malloc(sizeof(int) * RANGE);

	//Allocate OpenCL memory for results
	cl_mem divs = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
			sizeof(cl_int) * RANGE, NULL, &error);
	errchk(error, "clCreateBuffer");

	size_t src_size;
	const char *path = shrFindFilePath("1_kernel.cl", NULL);
	const char *source = oclLoadProgSource(path, "", &src_size);
	cl_program program = clCreateProgramWithSource(context, 1, &source,
							&src_size, &error);
	errchk(error, "clCreateProgramWithSource");

	error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	errchk(error, "clBuildProgram");

	char *build_log;
	size_t log_size;
	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL,
				       &log_size);
	build_log = (char *)malloc(sizeof(char) * (log_size+1));
	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
				       log_size, build_log, NULL);
	build_log[log_size] = '\0';
	printf("BUILD LOG:\n%s\n", build_log);
	free(build_log);

	cl_kernel div_kernel = clCreateKernel(program, "divisible", &error);
	errchk(error, "clCreateKernel");

	error = clSetKernelArg(div_kernel, 0, sizeof(cl_mem), &divs);
	errchk(error, "clSetKernelArg 0");
	size_t rng = RANGE;
	error = clSetKernelArg(div_kernel, 0, sizeof(size_t), &rng);
	errchk(error, "clSetKernelArg 1");

	const size_t local_ws = 512;
	const size_t global_ws = shrRoundUp(local_ws, RANGE);
	error = clEnqueueNDRangeKernel(queue, div_kernel, 1, NULL, &global_ws,
					      &local_ws, 0, NULL, NULL);
	errchk(error, "clEnqueueNDRangeKernel");

	clEnqueueReadBuffer(queue, divs, CL_TRUE, 0, sizeof(CL_INT) * RANGE, loc_divs, 0, NULL, NULL);

	int i;
	int count = 0;
	for(i=0; i < RANGE; i ++)
		 if (loc_divs[i] == 1)
		 	count ++;
	printf("%d\n", count);

	free(loc_divs);
	clReleaseKernel(div_kernel);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
	clReleaseMemObject(divs);
}
