#include "Functions.h"

MAVFilter_f_t *Initialize_MAVFilter_f(const uint16_t size)
{
    MAVFilter_f_t *obj = (MAVFilter_f_t *)malloc(sizeof(MAVFilter_f_t));

    obj->buffer = (float *)malloc(sizeof(float) * size);
    memset(obj->buffer, 0.0f, sizeof(float) * size);
    obj->size = size;

    return obj;
}

float calculateMovingAverage_f(MAVFilter_f_t *obj, const float newValue)
{
    float sum = 0.0f;
    uint16_t i = 0;

    for (i = 0; i < obj->size - 1; i++)
    {
        obj->buffer[i] = obj->buffer[i + 1];
        sum += obj->buffer[i];
    }
    obj->buffer[i] = newValue;
    sum += obj->buffer[i];

    return (sum / (float)obj->size);
}
