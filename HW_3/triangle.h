#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "vec4.h"
#include <iostream>
#include "camera.h"

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

struct row {
    float start_z, end_z;
    float y;
};

struct row_col {
    float red_sx;
    float green_sx;
    float blue_sx;
    float red_ex;
    float green_ex;
    float blue_ex;
    float y;
};

private:
    vec4 vert[3];
    float b0, b1, b2;
    slope m0, m1, m2;
    bool vertAtStart, vertAtEnd;
    int vertical;
    tuple t;
    float recent_z;
    row r;
    row_col rc;
public:
    vec4 norm[3];
    vec4 vert_color[3];
    vec4 diffuse;
    vec4 diffuse_rgb;
    int min_x, max_x, min_y, max_y;
    triple *pix;
    triangle();
    triangle(vec4 v1, vec4 v2, vec4 v3, vec4 n1, vec4 n2, vec4 n3, vec4 diff);
    vec4 &operator[](unsigned int index);
    const vec4 &operator[](unsigned int index) const;
    void rotateNorms(camera &c);
    void setBounds(int minx, int maxx, int miny, int maxy);
    vec4 getBoundingBox() const;
    int inBox(int x1, int x2, int defaul);
    void withRangeV(int x1, int x2, int x3, int v, bool vertStarted);
    void getRange(int x1, int x2, int x3, float y);
    bool vertStart(float y);
    void getPixels();
    int getStartx(int p) {return pix[p].x_start;}
    int getEndx(int p) {return pix[p].x_end;}
    float getY(int p) {return pix[p].y;}
    float getZ(int sx, int ex, float y, int mx, int method);
    int smallest(float i0, float i1, float i2, bool f0, bool f1, bool f2);
    float trilinear(int sx, int ex, float y, int mx);
    float trilinear_perspective(int sx, int ex, float y, int mx);
    float bary_z(float y, int mx);
    vec4 getColor(int sx, int ex, float y, int mx, int method);
    vec4 bary(float y, int mx);
    vec4 bary_perspective(float y, int mx);
    vec4 gouraud(int sx, int ex, float y, int mx);
    vec4 gouraud_perspective(int sx, int ex, float y, int mx);
};

std::ostream &operator<<(std::ostream &o, const triangle &t);

#endif // TRIANGLE_H
