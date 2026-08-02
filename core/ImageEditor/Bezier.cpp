#include "Layer.h"

char Layer::drawBezier(Vector2 a, Vector2 b, Vector2 c, int color) {
    const int steps = 32;
    Vector2 prev = a;
    for(int i = 1; i <= steps; i++) {
        float t = (float)i / steps;
        float mt = 1.0f - t;
        Vector2 p = a * (mt * mt) + b * (2.0f * mt * t) + c * (t * t);
        drawLine2D(prev, p, color);
        prev = p;
    }

    return 0;
}