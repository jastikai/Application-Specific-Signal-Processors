#include "fixed_ops.h"

FIXED mulf(FIXED i1, FIXED i2)
{
	int32_t intermediate = (int32_t)((int16_t)i1 * (int16_t)i2);
	/*if((intermediate >> 6) > 32767)
	{
		intermediate = 0x001fffff;
	}
	else if((intermediate >> 6) < -32768)
	{
		intermediate = 0xff200000;
	}*/
	return (FIXED)(intermediate >> 6);
}

FIXED addf(FIXED i1, FIXED i2)
{
	int32_t intermediate = (int32_t)((int16_t)i1 + (int16_t)i2);
	/*if(intermediate > 32767)
	{
		intermediate = 0x00007fff;
	}
	else if(intermediate < -32768)
	{
		intermediate = 0xffff8000;
	}*/
	return (FIXED)intermediate;
}

FIXED subf(FIXED i1, FIXED i2)
{
	int32_t intermediate = (int32_t)((int16_t)i1 - (int16_t)i2);
	/*if(intermediate > 32767)
	{
		intermediate = 0x00007fff;
	}
	else if(intermediate < -32768)
	{
		intermediate = 0xffff8000;
	}*/
	return (FIXED)intermediate;
}
