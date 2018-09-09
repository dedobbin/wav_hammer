#include <stdlib.h>
#include "utils.h"

int my_random(int min, int max)
{
	if (min < 0)
		min = 0;
	if (max < min)
		max = min;
	double random_value = (double)rand() / (RAND_MAX + 1);
	int result = min + random_value * (max - min);
	return result;
}
