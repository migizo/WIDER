/*
  ==============================================================================

    Ease.h
    Created: 14 May 2021 10:51:51am
    Author:  migizo

  ==============================================================================
*/

#pragma once
/*
 t : 時間(進行度)
 b : 開始の値(開始時の座標やスケールなど)
 c : 開始と終了の値の差分
 d : Tween(トゥイーン)の合計時間
 */
namespace Ease {

enum Type {
    In,
    Out,
    InOut
};
enum Func {
    Linear,
    Cubic,
    Exp
};
    
struct Cubic{
    inline static float In(float t, float b, float c, float d) {
        t /= d;
        return c*t*t*t + b;
    }
    inline static float Out(float t, float b, float c, float d) {
        t /= d;
        t = t - 1.0;
        return c*(t*t*t + 1.0) + b;
    }
    inline static float InOut(float t, float b, float c, float d) {
        t /= d/2.0;
        if (t < 1.0) return c/2.0*t*t*t + b;
        t = t - 2.0;
        return c/2.0 * (t*t*t + 2.0) + b;
    }
};

struct Exp{
    inline static float In(float t, float b, float c, float d) {
        return c * pow(2.0, (10.0*(t/d - 1.0))) + b;
    }
    inline static float Out(float t, float b, float c, float d) {
        return c * (-(pow(2.0, (-10.0 * t/d))) + 1) + b;
    }
    inline static float InOut(float t, float b, float c, float d) {
        t /= d/2.0;
        if (t < 1.0) return c/2.0 * pow(2.0, (10.0 * (t-1.0))) + b;
        t = t - 1;
        return c/2.0 * (-(pow(2.0, (-10.0*t))) + 2.0 ) + b;
    }
};
}
