#include <stdio.h>
#include <mkl.h>
int main(int argc, char** argv){
	printf("Hello %d", mkl_get_max_threads());
	return 0;
}
