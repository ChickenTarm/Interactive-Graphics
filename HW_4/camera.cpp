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
    z.norm();
    vec4 y_(u_x, u_y, u_z, 0);
    y = y_;
    y.norm();
    x = cross(z, y);
    x.norm();
    vec4 vt1(1, 0, 0, 0);
    vec4 vt2(0, 1, 0, 0);
    vec4 vt3(0, 0, 1, 0);
    vec4 vt4(-1 * location[0], -1 * location[1],  -1 * location[2], 1);
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

camera::camera(frustum f, vec4 &nl, vec4 &zn, vec4 &yn, vec4 &xn) {
    frust.left = f.left;
    frust.right = f.right;
    frust.top = f.top;
    frust.bottom = f.bottom;
    frust.near = f.near;
    frust.far = f.far;
    location = nl;
    x = xn;
    y = yn;
    z = zn;
    vec4 vt1(1, 0, 0, 0);
    vec4 vt2(0, 1, 0, 0);
    vec4 vt3(0, 0, 1, 0);
    vec4 vt4(-1 * location[0], -1 * location[1],  -1 * location[2], 1);
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

void camera::setLocation(vec4 &v) {
    location = v;
    vec4 vt1(1, 0, 0, 0);
    vec4 vt2(0, 1, 0, 0);
    vec4 vt3(0, 0, 1, 0);
    vec4 vt4(-1 * location[0], -1 * location[1],  -1 * location[2], 1);
    mat4 m(vt1, vt2, vt3, vt4);
    view_trans = m;
    final = project * view_orient * view_trans;
}

void camera::rotateAxisHorizontal(mat4 &m) {
    x = m * x;
    z = m * z;
    vec4 vo1(x[0], y[0], z[0], 0);
    vec4 vo2(x[1], y[1], z[1], 0);
    vec4 vo3(x[2], y[2], z[2], 0);
    vec4 vo4(0, 0, 0, 1);
    mat4 m1(vo1, vo2 , vo3, vo4);
    view_orient = m1;
    final = project * view_orient * view_trans;
}

void camera::rotateAxisVertical(mat4 &m) {
    y = m * y;
    z = m * z;
    vec4 vo1(x[0], y[0], z[0], 0);
    vec4 vo2(x[1], y[1], z[1], 0);
    vec4 vo3(x[2], y[2], z[2], 0);
    vec4 vo4(0, 0, 0, 1);
    mat4 m1(vo1, vo2 , vo3, vo4);
    view_orient = m1;
    final = project * view_orient * view_trans;
}

void camera::updateProjectMat() {
    vec4 p1(2 * frust.near / (frust.right - frust.left), 0, 0 , 0);
    vec4 p2(0, 2 * frust.near / (frust.top - frust.bottom), 0, 0);
    vec4 p3((frust.right + frust.left) / (frust.right - frust.left), (frust.top + frust.bottom) / (frust.top - frust.bottom), frust.far / (frust.far - frust.near), 1);
    vec4 p4(0, 0, -1 * (frust.far * frust.near) / (frust.far - frust.near), 0);
    mat4 p(p1, p2, p3, p4);
    project = p;
    final = project * view_orient * view_trans;
}

std::ostream &operator<<(std::ostream &o, const camera &c) {
    return o << "location: " << c.location << std::endl << "x: " << c.x << std::endl << "y: " << c.y << std::endl << "z: " << c.z << std::endl << "view_trans: " << c.view_trans << std::endl << "view_orient: " << c.view_orient << std::endl << "project: " << c.project << std::endl;
}
