#include "Tween.h"
#include <math.h>
#define PI 3.14159265
float Tween::Linear(float start, float end, float percent)
{
    return start + (end - start)* percent;
}

float Tween::InSine(float start, float end, float percent)
{
    return end - (end - start) * cos ( percent * PI / 2);
}

float Tween::OutSine(float start, float end, float percent)
{
    return start + (end - start) * sin (percent * PI / 2);
}

float Tween::InOutSine(float start, float end, float percent)
{
    return start + (end - start) / 2 * (1 - cos(percent * PI));
}

float Tween::InQuad(float start, float end, float percent)
{
    return start + (end - start) * percent * percent;
}

float Tween::OutQuad(float start, float end, float percent)
{
    return start + (end - start)*(2.0f* percent - percent* percent);
}

float Tween::InOutQuad(float start, float end, float percent)
{
    if (percent < 0.5f)
        return start + (end - start) * 2.0f * percent * percent;
    return start + (end - start) * (1.5f * percent - 0.5f - percent * percent);
}
