#ifndef MATHF
#define MATHF

// 浮動小数算術
class Mathf
{
public:
    // 線形補完
    static float inline Lerp(float a, float b, float t);

    // 指定範囲のランダム値を計算する
    static float inline RandomRange(float min, float max);
};

#endif // !MATHF

