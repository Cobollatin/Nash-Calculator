#ifndef NASH_RESOLVER
#define NASH_RESOLVER

#include <algorithm>

void pushback_double(double d, double c[], int& size);
double* biMatrixNashEquilibria
(
	double result[], int& sizeResult, bool mode,
	double** p1, int sizeY,
	double** p2, int sizeX,
	double** _p1, double** _p2,
	double& M
);


#endif // !NASH_RESOLVER


