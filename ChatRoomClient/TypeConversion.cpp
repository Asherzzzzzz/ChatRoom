#include "TypeConversion.h"

char to_char(int n)
{
	return '0' + n;
}

int to_int(const char* c, int cLen)
{
	int int_c = 0;

	for (int i = 0; i < cLen; i++)
	{
		int_c *= 10;
		int_c += c[i] - '0';
	}

	return int_c;
}