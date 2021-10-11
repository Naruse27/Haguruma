#include "Mathf.h"
#include <random>

// ���`�⊮
float Mathf::Lerp(float a, float b, float t)
{
    return a * (1.0f - t) + (b * t);
}

// �w��͈͂̃����_���l���v�Z����
float Mathf::RandomRange(float min, float max)
{
    float value = static_cast<float>(rand()) / RAND_MAX;
    return min + (max - min) * value;
}