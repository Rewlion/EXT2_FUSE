#include "endian_helpers.h"

int is_little_endian_cpu()
{
	union
	{
		int a;
		char b[sizeof(int)];
	} t;

	t.a = 1;
	if(t.b[0] == 1)
		return 1;
	else
		return 0;
}