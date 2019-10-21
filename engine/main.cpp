#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "ADT/Assertions.h"

void on_segfault(int sig)
{
	fprintf(stderr, "Error: segmentation fault\n");
	dump_backtrace();
	exit(sig);
}

int main(int argc, char* argv[])
{
	signal(SIGSEGV, on_segfault);
	return 0;
}