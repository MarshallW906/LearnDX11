#include "DirectXTemplatePCH.h"
#include "FuncUtils.h"


float FuncUtils::SimpleClamp(float v, float min, float max)
{
	if (v < min) return min;
	if (v > max) return max;
	return v;
}

float FuncUtils::ClampAngle(float v)
{
	static int lo = -180;
	static int hi = lo + 360;
	while (v > hi || v < lo)
	{
		if (v > hi)
		{
			v -= 360;
		}
		if (v < lo)
		{
			v += 360;
		}
	}
	return v;
}
