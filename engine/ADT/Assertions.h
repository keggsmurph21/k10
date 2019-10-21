#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h> 
#include <unistd.h>
#include <cxxabi.h>
#include <signal.h>

static inline void dump_backtrace(FILE *out = stderr, unsigned int max_frames = 63)
{
	// adapted from Timo Bingmann via https://panthema.net/2008/0901-stacktrace-demangled/
    fprintf(out, "stack trace:\n");

    void *addresses[max_frames + 1];
    int num_addresses = backtrace(addresses, sizeof(addresses) / sizeof(void*));

    if (num_addresses == 0) {
    	fprintf(out, "  <no stack trace>\n");
    	return;
    }

    char **symbols = backtrace_symbols(addresses, num_addresses);
    
    size_t func_name_size = 256;
    char *func_name = (char*) malloc(func_name_size);

    // iterate over returned symbol lines, skipping first
    for (int i = 1; i < num_addresses; i++) {

		// find parentheses and +address offset surrounding the mangled name:
		// ./module(function+0x15c) [0x8048a6d]
    	char *begin_name = 0, *begin_offset = 0, *end_offset = 0;
    	for (char *p = symbols[i]; *p; ++p) {
    		if (*p == '(')
    			begin_name = p;
    		else if (*p == '+')
    			begin_offset = p;
    		else if (*p == ')' && begin_offset) {
    			end_offset = p;
    			break;
    		}
    	}

    	if (begin_name && begin_offset && end_offset
    		&& begin_name < begin_offset) {

    		*begin_name++ = '\0';
    		*begin_offset++ = '\0';
    		*end_offset = '\0';

	    	int status;
	    	char* demangled = abi::__cxa_demangle(begin_name, func_name, &func_name_size, &status);
	    	if (status == 0) {
	    		fprintf(out, "  %s : %s+%s\n", symbols[i], demangled, begin_offset);
	    	} else {
	    		fprintf(out, "  %s : %s()+%s\n", symbols[i], begin_name, begin_offset);
	    	}
    	} else {
    		// error parsing? print whole line
    		fprintf(out, "  %s\n", symbols[i]);
    	}
    }

    free(symbols);
    free(func_name);
}

static inline void __assertion_failed(const char* msg, const char* file, unsigned line)
{
    printf("ASSERTION FAILED: %s\n%s:%u\n", msg, file, line);
    dump_backtrace();
    raise(SIGABRT);
}
#define ASSERT(expr) (static_cast<bool>(expr) ? (void)0 : __assertion_failed(#expr, __FILE__, __LINE__))
#define ASSERT_NOT_REACHED() ASSERT(false)