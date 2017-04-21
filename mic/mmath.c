#include "mmath.h"

FP32 clamp(FP32 val, FP32 min, FP32 max)
{
    if (val < min)
        return min;
    if (val > max)
        return max;
    return val;
}
