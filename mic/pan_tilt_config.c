#include "pan_tilt_config.h"
#include <math.h>
#include "mmath.h"

#define PI 3.14159
#define TICKS_PR_REV (360*3)
#define RAD_PR_TICK (2 * PI / TICKS_PR_REV)
#define MAX_VOLTAGE 12

FP32 ticks_to_rad(INT16S ticks)
{
    return ticks * RAD_PR_TICK;
}

INT8S voltage_to_dutycycle(FP32 voltage)
{
    FP32 temp = voltage / MAX_VOLTAGE;
    temp *= 128;
    temp = round(temp);
    temp = clamp(temp, -128, 127);
    return (INT8S)temp;
}
