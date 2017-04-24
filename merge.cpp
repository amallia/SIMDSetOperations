#include <cstdio>
#include <chrono>

#include "constants.h"

#include "merge/naive.hpp"
#include "merge/stl.hpp"


void run(uint32_t **lists, size_t (*func)(const uint32_t*,size_t,const uint32_t*,size_t,uint32_t*)){
	uint32_t *merge_list = (uint32_t*)aligned_alloc(64, 2*arraySize*sizeof(uint32_t));
	auto t_start = std::chrono::high_resolution_clock::now();
	size_t merge_count=0;
	for(size_t i=0; i<listCount; ++i){
		for(size_t j=i+1; j<listCount; ++j){
			merge_count += func(lists[i], arraySize, lists[j], arraySize, merge_list);
		}
	}
	auto t_end = std::chrono::high_resolution_clock::now();
	printf("Wall clock time passed: %10.2f ms - %lu\n",
		std::chrono::duration<double, std::milli>(t_end-t_start).count(),
		merge_count
	);
	free(merge_list);
}

int main(){
	auto t_start = std::chrono::high_resolution_clock::now();
	// load lists from file which was generated by genLists
	FILE *fd = fopen("test.dat", "rb");
	if(!fd){
		puts("couldn't open test.dat");
		return -1;
	}
	uint32_t **lists = new uint32_t*[listCount];
	for(size_t i=0; i<listCount; ++i){
		lists[i] = (uint32_t*)aligned_alloc(64, arraySize*sizeof(uint32_t));
		fread(lists[i], 4, arraySize, fd);
	}
	fclose(fd);
	auto t_end = std::chrono::high_resolution_clock::now(); 
	printf("preparing lists done - %f ms\n",
		std::chrono::duration<double, std::milli>(t_end-t_start).count()
	);

	puts("naive scalar merge:");
	run(lists, merge_scalar);
	puts("stl merge:");
	run(lists, merge_scalar_stl);

	// cleanup
	for(size_t i=0; i<listCount; ++i){
		free(lists[i]);
	}
	delete[] lists;

	return 0;
}
