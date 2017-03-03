#ifndef CAMERA_H
#define CAMERA_H
#include "vec4.h"
#include "mat4.h"

class camera
{
struct frustum {
    float left, right, top, bottom, near, far;
};

private:
    frustum frust;
    vec4 location, x, y, z;
    mat4 view_trans, view_orient, project, final;
public:
    camera();
    camera(float l, float r, float t, float b, float n, float f, float e_x, float e_y, float e_z, float c_x, float c_y, float c_z, float u_x, float u_y, float u_z);

    camera& operator =(camera &c);
    vec4 transform(vec4 &v);
    vec4 rotate(vec4 &v) {return view_orient * v;}
};

#endif // CAMERA_H
