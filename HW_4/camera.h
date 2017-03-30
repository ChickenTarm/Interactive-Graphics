#ifndef CAMERA_H
#define CAMERA_H
#include "vec4.h"
#include "mat4.h"

class camera
{
struct frustum {
    float left, right, top, bottom, near, far;
};

public:
    frustum frust;
    vec4 location, x, y, z;
    mat4 view_trans, view_orient, project, final;
    camera();
    camera(float l, float r, float t, float b, float n, float f, float e_x, float e_y, float e_z, float c_x, float c_y, float c_z, float u_x, float u_y, float u_z);
    camera(frustum f, vec4 &nl, vec4 &zn, vec4 &yn, vec4 &xn);

    camera& operator =(camera &c);
    vec4 transform(vec4 &v);
    vec4 rotate(vec4 &v) {return view_orient * v;}
    void setLocation(vec4 &v);
    void rotateAxisHorizontal(mat4 &m);
    void rotateAxisVertical(mat4 &m);
    void updateProjectMat();
};

std::ostream &operator<<(std::ostream &o, const camera &c);

#endif // CAMERA_H
