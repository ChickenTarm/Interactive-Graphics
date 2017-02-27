#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "vec4.h"
#include <iostream>

class triangle
{
struct triple {
    int x_start, x_end;
    float y;
};

struct tuple {
    int x_start, x_end;
};

struct slope {
    float m;
    bool isVert, isFlat;
};

private:
    vec4 vert[3];
    float b0, b1, b2;
    slope m0, m1, m2;
    bool vertAtStart, vertAtEnd;
    int vertical;
    tuple t;
public:
    int min_x, max_x, min_y, max_y;
    triple *pix;
    triangle();
    triangle(vec4 v1, vec4 v2, vec4 v3);
    vec4 &operator[](unsigned int index);
    const vec4 &operator[](unsigned int index) const;
    void setBounds(int minx, int maxx, int miny, int maxy);
    vec4 getBoundingBox() const;
    int inBox(int x1, int x2, int defaul);
    void getRange(int x1, int x2, int x3);
    void getPixels();
};

std::ostream &operator<<(std::ostream &o, const triangle &t);

#endif // TRIANGLE_H
