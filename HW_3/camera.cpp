#include "camera.h"
#include "vec4.h"
#include "mat4.h"

using namespace std;

camera::camera() {
}

camera::camera(float l, float r, float b, float t, float n, float f, float e_x, float e_y, float e_z, float c_x, float c_y, float c_z, float u_x, float u_y, float u_z) {
    frust.left = l;
    frust.right = r;
    frust.top = t;
    frust.bottom = b;
    frust.near = n;
    frust.far = f;
    vec4 l_(e_x, e_y, e_z, 0);
    location = l_;
    vec4 z_(c_x - e_x, c_y - e_y, c_z - e_z, 0);
    z = z_;
    vec4 y_(u_x, u_y, u_z, 0);
    y = y_;
    z.norm();
    y.norm();
    x = cross(z, y);
    x.norm();
    vec4 vt1(1, 0, 0, 0);
    vec4 vt2(0, 1, 0, 0);
    vec4 vt3(0, 0, 1, 0);
    vec4 vt4(-1 * e_x, -1 * e_y,  -1 * e_z, 1);
    mat4 m(vt1, vt2, vt3, vt4);
    view_trans = m;
    vec4 vo1(x[0], y[0], z[0], 0);
    vec4 vo2(x[1], y[1], z[1], 0);
    vec4 vo3(x[2], y[2], z[2], 0);
    vec4 vo4(0, 0, 0, 1);
    mat4 m1(vo1, vo2 , vo3, vo4);
    view_orient = m1;
    vec4 p1(2 * frust.near / (frust.right - frust.left), 0, 0 , 0);
    vec4 p2(0, 2 * frust.near / (frust.top - frust.bottom), 0, 0);
    vec4 p3((frust.right + frust.left) / (frust.right - frust.left), (frust.top + frust.bottom) / (frust.top - frust.bottom), frust.far / (frust.far - frust.near), 1);
    vec4 p4(0, 0, -1 * (frust.far * frust.near) / (frust.far - frust.near), 0);
    mat4 p(p1, p2, p3, p4);
    project = p;
    final = project * view_orient * view_trans;
}

camera& camera::operator =(camera &c) {
    this->frust.left = c.frust.left;
    this->frust.right = c.frust.right;
    this->frust.top = c.frust.top;
    this->frust.bottom = c.frust.bottom;
    this->frust.near = c.frust.near;
    this->frust.far = c.frust.far;
    this->location = c.location;
    this->x = c.x;
    this->y = c.y;
    this->z = c.z;
    this->view_orient = c.view_orient;
    this->view_trans = c.view_trans;
    this->project = c.project;
    this->final = c.final;
    return *this;
}

vec4 camera::transform(vec4 &v) {
    return final *v;
}
