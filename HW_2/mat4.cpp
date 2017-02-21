#include "mat4.h"
#include "vec4.h"
#include <cmath>
#include <assert.h>

using namespace std;

mat4::mat4() {
    vec4 fst(1, 0, 0, 0);
    vec4 snd(0, 1, 0, 0);
    vec4 trd(0, 0, 1, 0);
    vec4 fth(0, 0, 0, 1);
    this->data[0] = fst;
    this->data[1] = snd;
    this->data[2] = trd;
    this->data[3] = fth;
}

mat4::mat4(float diag) {
    vec4 fst(diag, 0, 0, 0);
    vec4 snd(0, diag, 0, 0);
    vec4 trd(0, 0, diag, 0);
    vec4 fth(0, 0, 0, diag);
    this->data[0] = fst;
    this->data[1] = snd;
    this->data[2] = trd;
    this->data[3] = fth;
}

mat4::mat4(const vec4 &col0, const vec4 &col1, const vec4 &col2, const vec4 &col3) {
    this->data[0] = col0;
    this->data[1] = col1;
    this->data[2] = col2;
    this->data[3] = col3;
}

mat4::mat4(const mat4 &m2) {
    this->data[0] = m2[0];
    this->data[1] = m2[1];
    this->data[2] = m2[2];
    this->data[3] = m2[3];
}

const vec4& mat4::operator [](unsigned int index) const {
    const vec4 &v = this->data[index];
    return v;
}

vec4& mat4::operator [](unsigned int index) {
    return (this->data[index]);
}

const vec4& mat4::operator ()(unsigned int index) const {
    assert(index >= 0 && index < 4);
    const vec4 &v = this->data[index];
    return v;
}

vec4& mat4::operator ()(unsigned int index) {
    assert(index >= 0 && index < 4);
    return this->data[index];
}

mat4 mat4::rot(float angle, float x, float y, float z) {
    float len = sqrt(x * x + y * y + z * z);
    vec4 d(x / len, y / len, z / len, 0);
    double r = angle / 180.0 * M_PI;
    vec4 c1((1 - cos(r)) * d[0] * d[0] + cos(r),
            (1 - cos(r)) * d[0] * d[1] + sin(r) * d[2],
            (1 - cos(r)) * d[0] * d[2] - sin(r) * d[1], 0);
    vec4 c2((1 - cos(r)) * d[0] * d[1] - sin(r) * d[2],
            (1 - cos(r)) * d[1] * d[1] + cos(r),
            (1 - cos(r)) * d[1] * d[2] + sin(r) * d[0], 0);
    vec4 c3((1 - cos(r)) * d[0] * d[2] + sin(r) * d[1],
            (1 - cos(r)) * d[1] * d[2] - sin(r) * d[0],
            (1 - cos(r)) * d[2] * d[2] + cos(r), 0);
    vec4 c4(0, 0, 0, 1);
    mat4 rotate(c1, c2, c3, c4);
    return rotate;
}

mat4 mat4::trans(float x, float y, float z) {
    vec4 fst(1, 0, 0, 0);
    vec4 snd(0, 1, 0, 0);
    vec4 trd(0, 0, 1, 0);
    vec4 fth(x, y, z, 1);
    mat4 trn(fst, snd, trd, fth);
    return trn;
}

mat4 mat4::scale(float x, float y, float z) {
    vec4 fst(x, 0, 0, 0);
    vec4 snd(0, y, 0, 0);
    vec4 trd(0, 0, z, 0);
    vec4 fth(0, 0, 0, 1);
    mat4 scl(fst, snd, trd, fth);
    return scl;
}

mat4& mat4::operator =(const mat4 &m2) {
    this->data[0] = m2[0];
    this->data[1] = m2[1];
    this->data[2] = m2[2];
    this->data[3] = m2[3];
    return *this;
}

bool mat4::operator ==(const mat4 &m2) const {
    return this->data[0] == m2[0] && this->data[1] == m2[1] && this->data[2] == m2[2] && this->data[3] == m2[3];
}

bool mat4::operator !=(const mat4 &m2) const {
    return this->data[0] != m2[0] || this->data[1] != m2[1] || this->data[2] != m2[2] || this->data[3] != m2[3];
}

mat4& mat4::operator +=(const mat4 &m2) {
    this->data[0] = this->data[0] + m2[0];
    this->data[1] = this->data[1] + m2[1];
    this->data[2] = this->data[2] + m2[2];
    this->data[3] = this->data[3] + m2[3];
    return *this;
}

mat4& mat4::operator -=(const mat4 &m2) {
    this->data[0] = this->data[0] - m2[0];
    this->data[1] = this->data[1] - m2[1];
    this->data[2] = this->data[2] - m2[2];
    this->data[3] = this->data[3] - m2[3];
    return *this;
}

mat4& mat4::operator *=(float c) {
    this->data[0] = this->data[0] * c;
    this->data[1] = this->data[1] * c;
    this->data[2] = this->data[2] * c;
    this->data[3] = this->data[3] * c;
    return *this;
}

mat4& mat4::operator /=(float c) {
    this->data[0] = this->data[0] / c;
    this->data[1] = this->data[1] / c;
    this->data[2] = this->data[2] / c;
    this->data[3] = this->data[3] / c;
    return *this;
}

mat4 mat4::operator +(const mat4 &m2) const {
    mat4 add(this->data[0] + m2[0], this->data[1] + m2[1],
            this->data[2] + m2[2], this->data[3] + m2[3]);
    return add;
}

mat4 mat4::operator -(const mat4 &m2) const {
    mat4 sub(this->data[0] - m2[0], this->data[1] - m2[1],
            this->data[2] - m2[2], this->data[3] - m2[3]);
    return sub;
}

mat4 mat4::operator *(float c) const {
    mat4 mul(this->data[0] * c, this->data[1] * c,
            this->data[2] * c, this->data[3] * c);
    return mul;
}

mat4 mat4::operator /(float c) const{
    mat4 div(this->data[0] / c, this->data[1] / c,
            this->data[2] / c, this->data[3] / c);
    return div;
}

mat4 mat4::operator *(const mat4 &m2) const {
    vec4 r1(this->data[0][0], this->data[1][0], this->data[2][0], this->data[3][0]);
    vec4 r2(this->data[0][1], this->data[1][1], this->data[2][1], this->data[3][1]);
    vec4 r3(this->data[0][2], this->data[1][2], this->data[2][2], this->data[3][2]);
    vec4 r4(this->data[0][3], this->data[1][3], this->data[2][3], this->data[3][3]);
    vec4 c1(dot(r1, m2[0]), dot(r2, m2[0]), dot(r3, m2[0]), dot(r4, m2[0]));
    vec4 c2(dot(r1, m2[1]), dot(r2, m2[1]), dot(r3, m2[1]), dot(r4, m2[1]));
    vec4 c3(dot(r1, m2[2]), dot(r2, m2[2]), dot(r3, m2[2]), dot(r4, m2[2]));
    vec4 c4(dot(r1, m2[3]), dot(r2, m2[3]), dot(r3, m2[3]), dot(r4, m2[3]));
    mat4 result(c1, c2, c3, c4);
    return result;
}

vec4 mat4::operator *(const vec4 &v) const {
    vec4 r1(this->data[0][0], this->data[1][0], this->data[2][0], this->data[3][0]);
    vec4 r2(this->data[0][1], this->data[1][1], this->data[2][1], this->data[3][1]);
    vec4 r3(this->data[0][2], this->data[1][2], this->data[2][2], this->data[3][2]);
    vec4 r4(this->data[0][3], this->data[1][3], this->data[2][3], this->data[3][3]);
    vec4 nv(dot(r1, v), dot(r2, v), dot(r3, v), dot(r4, v));
    return nv;
}

mat4 mat4::transpose() const {
    vec4 r1(this->data[0][0], this->data[1][0], this->data[2][0], this->data[3][0]);
    vec4 r2(this->data[0][1], this->data[1][1], this->data[2][1], this->data[3][1]);
    vec4 r3(this->data[0][2], this->data[1][2], this->data[2][2], this->data[3][2]);
    vec4 r4(this->data[0][3], this->data[1][3], this->data[2][3], this->data[3][3]);
    mat4 tran(r1, r2, r3, r4);
    return tran;
}

const vec4& mat4::col(unsigned int index) const {
    const vec4 &v = this->data[index];
    return v;
}

vec4& mat4::col(unsigned int index) {
    return this->data[index];
}

mat4 operator*(float c, const mat4 &m) {
    mat4 mul = m * c;
    return mul;
}

vec4 operator *(const vec4 &v, const mat4 &m) {
    vec4 r(dot(v, m[0]), dot(v, m[1]), dot(v, m[2]), dot(v, m[3]));
    return r;
}

std::ostream &operator<<(std::ostream &o, const mat4 &m) {
    vec4 r1(m[0][0], m[1][0], m[2][0], m[3][0]);
    vec4 r2(m[0][1], m[1][1], m[2][1], m[3][1]);
    vec4 r3(m[0][2], m[1][2], m[2][2], m[3][2]);
    vec4 r4(m[0][3], m[1][3], m[2][3], m[3][3]);
    o << endl << r1 << endl << r2 << endl << r3 << endl << r4;
    return o;
}
