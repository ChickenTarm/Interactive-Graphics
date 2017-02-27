#include "vec4.h"
#include <cmath>
#include <assert.h>

using namespace std;

vec4::vec4() {
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
}

vec4::vec4(float x, float y, float z, float w) {
    this->data[0] = x;
    this->data[1] = y;
    this->data[2] = z;
    this->data[3] = w;
}

vec4::vec4(const vec4 &v2) {
    this->data[0] = v2[0];
    this->data[1] = v2[1];
    this->data[2] = v2[2];
    this->data[3] = v2[3];
}

float vec4::operator [](unsigned int index) const {
    return data[index];
}

float& vec4::operator [](unsigned int index) {
    float &n = data[index];
    return n;
}

float vec4::operator ()(unsigned int index) const {
    assert (index < 4 && index >= 0);
    return data[index];
}

float& vec4::operator ()(unsigned int index) {
    assert (index < 4 && index >= 0);
    float &n = data[index];
    return n;
}

vec4& vec4::operator =(const vec4 &v2) {
    this->data[0] = v2[0];
    this->data[1] = v2[1];
    this->data[2] = v2[2];
    this->data[3] = v2[3];
   return *this;
}

bool vec4::operator ==(const vec4 &v2) const {
    if (this->data[0] != v2[0]) {
        return false;
    }
    if (this->data[1] != v2[1]) {
        return false;
    }
    if (this->data[2] != v2[2]) {
        return false;
    }
    if (this->data[3] != v2[3]) {
        return false;
    }
    return true;
}

bool vec4::operator !=(const vec4 &v2) const {
    if (this->data[0] != v2[0]) {
        return true;
    }
    if (this->data[1] != v2[1]) {
        return true;
    }
    if (this->data[2] != v2[2]) {
        return true;
    }
    if (this->data[3] != v2[3]) {
        return true;
    }
    return false;
}

vec4& vec4::operator +=(const vec4 &v2) {
    this->data[0] = this->data[0] + v2[0];
    this->data[1] = this->data[1] + v2[1];
    this->data[2] = this->data[2] + v2[2];
    this->data[3] = this->data[3] + v2[3];
    return *this;
}

vec4& vec4::operator -=(const vec4 &v2) {
    this->data[0] = this->data[0] - v2[0];
    this->data[1] = this->data[1] - v2[1];
    this->data[2] = this->data[2] - v2[2];
    this->data[3] = this->data[3] - v2[3];
    return *this;
}

vec4& vec4::operator *=(float c) {
    this->data[0] = this->data[0] * c;
    this->data[1] = this->data[1] * c;
    this->data[2] = this->data[2] * c;
    this->data[3] = this->data[3] * c;
    return *this;
}

vec4& vec4::operator /=(float c) {
    this->data[0] = this->data[0] / c;
    this->data[1] = this->data[1] / c;
    this->data[2] = this->data[2] / c;
    this->data[3] = this->data[3] / c;
    return *this;
}

vec4 vec4::operator +(const vec4 &v2) const {
    vec4 result(this->data[0] + v2[0], this->data[1] + v2[1], this->data[2] + v2[2], this->data[3] + v2[3]);
    return result;
}

vec4 vec4::operator -(const vec4 &v2) const {
    vec4 result(this->data[0] - v2[0], this->data[1] - v2[1], this->data[2] - v2[2], this->data[3] - v2[3]);
    return result;
}

vec4 vec4::operator *(float c) const {
    vec4 result(this->data[0] * c, this->data[1] * c, this->data[2] * c, this->data[3] * c);
    return result;
}

vec4 vec4::operator /(float c) const {
    vec4 result(this->data[0] / c, this->data[1] / c, this->data[2] / c, this->data[3] / c);
    return result;
}

float vec4::length() const {
    return sqrt(this->data[0] * this->data[0] + this->data[1] * this->data[1] +
            this->data[2] * this->data[2] + this->data[3] * this->data[3]);
}

vec4 vec4::normalize() const {
    float len = length();
    if (len == 0) {
        vec4 zero(0, 0, 0, 0);
        return zero;
    }
    vec4 unit(this->data[0] / len, this->data[1] / len, this->data[2] / len, this->data[3] / len);
    return unit;
}

void vec4::norm() {
    float len = length();
    if (len > 0) {
        this->data[0] = this->data[0] / len;
        this->data[1] = this->data[1] / len;
        this->data[2] = this->data[2] / len;
        this->data[3] = this->data[3] / len;
    }
}

float dot(const vec4 &v1, const vec4 &v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];
}

vec4 cross(const vec4 &v1, const vec4 &v2) {
    float x = v1[1] * v2[2] - v1[2] * v2[1];
    float y = v1[2] * v2[0] - v1[0] * v2[2];
    float z = v1[0] * v2[1] - v1[1] * v2[0];
    vec4 crs(x, y, z, 0);
    return crs;
}

vec4 operator *(float c, const vec4 &v) {
    vec4 scale(c * v[0], c * v[1], c * v[2], c * v[3]);
    return scale;
}

std::ostream& operator <<(std::ostream &o, const vec4 &v) {
    o << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";
    return o;
}
