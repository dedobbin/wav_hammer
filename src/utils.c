#include <stdlib.h>
#include "utils.h"

int random(int min, int max)
{
	if (min < 0)
		min = 0;
	if (max < min)
		max = min;
	int result = (double)rand() / (RAND_MAX + 1) * (max - min) + min;
	return result;
}