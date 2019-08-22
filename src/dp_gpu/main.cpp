#include <iostream>
#include <fstream>

#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.hpp>

void compute_double_rows(int* const double_rows, const int n, int col, uint32_t x, uint32_t y, int piece_count) {
	if(col == 0) {
		double_rows[(x << n) | y] = piece_count;
		return;
	}

	--col;
	compute_double_rows(double_rows, n, col, x, y, piece_count);

	if(col == 0) return;

	--col; ++piece_count;
	x |= 0b11u << col;
	y |= 0b01u << col;
	compute_double_rows(double_rows, n, col, x, y, piece_count);

	y ^= 0b11u << col;
	compute_double_rows(double_rows, n, col, x, y, piece_count);

	x ^= 0b01u << col;
	y |= 0b11u << col;
	compute_double_rows(double_rows, n, col, x, y, piece_count);

	x ^= 0b11u << col;
	compute_double_rows(double_rows, n, col, x, y, piece_count);

	if(col == 0) return;

	--col; ++piece_count;
	x |= 0b111u << col;
	y |= 0b111u << col;
	compute_double_rows(double_rows, n, col, x, y, piece_count);
}

std::string get_kernel_code() {
	std::ifstream in("kernel.cl");
	return std::string(
		(std::istreambuf_iterator<char>(in)),
		std::istreambuf_iterator<char>());
}

int main() {
	/* int n; */
	/* std::cin >> n; // non-square later */

	/* int* double_rows = new int[1 << (2*n)]; */
	/* compute_double_rows(double_rows, n, n, 0, 0, 0); */

	// get all platforms (drivers)
	std::vector<cl::Platform> all_platforms;
	cl::Platform::get(&all_platforms);

	if (all_platforms.size() == 0) {
		std::cout << "No platforms found. Check OpenCL installation!\n";
		exit(1);
	}
	for(int i = 0; i < all_platforms.size(); ++i) {
		std::cout << "Available platform: " << all_platforms[i].getInfo<CL_PLATFORM_NAME>() << '\n';
	}
	cl::Platform default_platform = all_platforms[0];
	std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

	// get default device (CPUs, GPUs) of the default platform
	std::vector<cl::Device> all_devices;
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
	if(all_devices.size() == 0) {
		std::cout << "No devices found. Check OpenCL installation!\n";
		exit(1);
	}

	for(int i = 0; i < all_devices.size(); ++i) {
		std::cout << "Available device: " << all_devices[i].getInfo<CL_DEVICE_NAME>() << '\n';
	}

	// use device[1] because that's a GPU; device[0] is the CPU
	cl::Device default_device = all_devices[0];
	std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";

	// a context is like a "runtime link" to the device and platform;
	// i.e. communication is possible
	cl::Context context({default_device});

	// create the program that we want to execute on the device
	cl::Program::Sources sources;

	std::string source = get_kernel_code();
	sources.push_back({source.c_str(), source.size()});

	cl::Program program(context, sources);
	if (program.build({default_device}) != CL_SUCCESS) {
		std::cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
		exit(1);
	}

	const int A[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	const int B[] { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
	static_assert(sizeof(A) == sizeof(B));

    // create buffers on device (allocate space on GPU)
	cl::Buffer buffer_A(context, CL_MEM_READ_ONLY, sizeof(A));
	cl::Buffer buffer_B(context, CL_MEM_READ_ONLY, sizeof(B));
	cl::Buffer buffer_C(context, CL_MEM_READ_WRITE, sizeof(A));

	// create a queue (a queue of commands that the GPU will execute)
	cl::CommandQueue queue(context, default_device);

	// push write commands to queue
	queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(A), A);
	queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(B), B);

	//run the kernel
	cl::Kernel kernel_add = cl::Kernel(program, "simple_add");
	kernel_add.setArg(0, buffer_A);
	kernel_add.setArg(1, buffer_B);
	kernel_add.setArg(2, buffer_C);
	queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(sizeof(A) / sizeof(A[0])), cl::NullRange);
	queue.finish();

	//read result C from the device to array C
	int C[sizeof(A) / sizeof(A[0])];
	queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, sizeof(C), C);

	std::cout << "result:\n";
	for(int x: C) std::cout << " " << x;
	std::cout << "\n";
}
