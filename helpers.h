#ifndef HELPERS_H
#define HELPERS_H

#include "stdio.h"
#include "stdarg.h"

#define true 1
#define false 0

typedef int bool;

int ClampI(int val, int min, int max);
double ClampD(double val, int min, int max);
float ClampF(float val, int min, int max);

int ClampI(int val, int min, int max) {
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}
double ClampD(double val, int min, int max) {
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}
float ClampF(float val, int min, int max) {
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}
void Log(const char *format, ...) {
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	printf("\n");
	fflush(stdout);
}
// hi
#endif