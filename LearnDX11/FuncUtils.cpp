#include "DirectXTemplatePCH.h"
#include "FuncUtils.h"

namespace FuncUtils
{
    float SimpleClamp(float v, float min, float max)
    {
        if (v < min) return min;
        if (v > max) return max;
        return v;
    }
}
