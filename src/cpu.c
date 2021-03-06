#include <stdio.h>
#include <string.h>
#include <timer.h>
#include "cpu.h"

char cpu_brand[4 * 4 * 3 + 1];

void cpu_init() {
	uint32_t* p = (uint32_t*)cpu_brand;
	
	uint32_t eax = 0x80000002;
	for(int i = 0; i < 12; i += 4) {
		__asm volatile("cpuid"
			: "=a"(p[i + 0]), "=b"(p[i + 1]), "=c"(p[i + 2]), "=d"(p[i + 3]) 
			: "a"(eax++));
	}
	
	printf("\tBrand: %s\n", cpu_brand);
}

bool cpu_has_feature(int feature) {
	uint32_t a, b, c, d;
	
	#define INFO(cmd) __asm volatile("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"((cmd)))
	#define EXT(cmd) __asm volatile("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(0x80000000 + (cmd)))
	
	switch(feature) {
		case CPU_FEATURE_SSE_4_1:
			INFO(0x01);
			return !!(c & 0x80000);
		case CPU_FEATURE_SSE_4_2:
			INFO(0x01);
			return !!(c & 0x100000);
		case CPU_FEATURE_MONITOR_MWAIT:
			INFO(0x01);
			return !!(c & 0x8);
		case CPU_FEATURE_MWAIT_INTERRUPT:
			INFO(0x05);
			return !!(c & 0x2);
		case CPU_FEATURE_TURBO_BOOST:
			INFO(0x06);
			return !!(a & 0x2);
		case CPU_FEATURE_INVARIANT_TSC:
			EXT(0x07);
			return !!(d & 0x100);
		default:
			return false;
	}
}
