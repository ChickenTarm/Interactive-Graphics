// Base code written by Jan Allbeck, Chris Czyzewicz, Cory Boatright, Tiantian Liu, Benedict Brown, and Adam Mally
// University of Pennsylvania

// At least some C++ compilers do funny things
// to C's math.h header if you don't define this
#define _USE_MATH_DEFINES

#include "vec4.h"
#include "mat4.h"
#include <iostream>
#include <math.h>
using namespace std;

int main() {
    cout << "Vector tests\n";
    vec4 v1;
    cout << "empty constructor v1: " << v1 <<  endl;
    vec4 v2(1.01, 2.02, 3.03, 4.04);
    cout << "4 argument constructor v2: " << v2 << endl;
    vec4 vc(6, 6, 6, 6);
    vec4 v3(vc);
    cout << "copy constructor v3: " << v3 <<  endl;
    vec4 v4 = v2;
    cout << "assignment v4 = v2: " << v4 << endl;
//    v2(-1); //should crash and does
    cout << "first element of v2: " << v2[0] << endl;
    cout << "second element of v2: " << v2(1) << endl;
    cout << "third element of v2: " << v2[2] << endl;
    cout << "forth element of v2: " << v2(3) << endl;
//    v2(4); //should crash and does

//    Test boolean operators
    cout << "v2 == v4: " << (v2 == v4) << endl;
    cout << "v2 == v1: " << (v2 == v1) << endl;
    cout << "v2 != v4: " << (v2 != v4) << endl;
    cout << "v2 != v1: " << (v2 != v1) << endl;

//    Test vec4 non-const operators
    cout << "v1 += v1: " << (v1 += v1) << endl;
    cout << "v1 += v2: " << (v1 += v2) << endl;
    cout << "v1 += v3: " << (v1 += v3) << endl;
    cout << "v1 += v1: " << (v1 += v1) << endl;
    cout << "v1 -= v2: " << (v1 -= v2) << endl;
    cout << "v1 -= v3: " << (v1 -= v3) << endl;
    cout << "v1 -= v1: " << (v1 -= v1) << endl;
    cout << "v1 *= 0: " << (v1 *= 0) << endl;
    cout << "v1 *= 2: " << (v1 *= 2) << endl;
    cout << "v4 *= 1: " << (v4 *= 1) << endl;
    cout << "v4 *= 2: " << (v4 *= 2) << endl;
    cout << "v4 /= 1: " << (v4 /= 1) << endl;
    cout << "v4 /= 2: " << (v4 /= 2) << endl;
    cout << "v4 *= 0: " << (v4 *= 0) << endl;

//    Test vec4 const operators
    cout << "v1 + v1: " << (v1 + v2) << endl;
    cout << "v1 + v2: " << (v1 + v2) << endl;
    cout << "v1 - v2: " << (v1 - v2) << endl;
    cout << "v1 - v1: " << (v1 - v1) << endl;
    cout << "v1 * 0: " << (v1 * 0) << endl;
    cout << "v1 * 99: " << (v1 * 99) << endl;
    cout << "v2 * 0: " << (v2 * 0) << endl;
    cout << "v2 * 1: " << (v2 * 1) << endl;
    cout << "v2 * 2: " << (v2 * 2) << endl;
    cout << "v1 / 0: " << (v1 / 0) << endl;
    cout << "v1 / 99: " << (v1 / 99) << endl;
    cout << "v2 / 0: " << (v2 / 0) << endl;
    cout << "v2 / 1: " << (v2 / 1) << endl;
    cout << "v2 / 2: " << (v2 / 2) << endl;

//    Test vec4 length
    cout << "length of v1: " << (v1.length()) <<  endl;
    cout << "length of v2: " << (v2.length()) << endl;
    cout << "length of v3: " << (v3.length()) << endl;

//    Test vec4 normalize and norm
    cout << "normalize v1: " << (v1.normalize()) <<  endl;
    cout << "normalize v2: " << (v2.normalize()) <<  endl;
    cout << "normalize v3: " << (v3.normalize()) <<  endl;
    v1.norm();
    cout << "norm of v1: " << v1 << endl;
    v2.norm();
    cout << "norm of v2: " << v2 << endl;
    v3.norm();
    cout << "norm of v3: " << v3 << endl;

//    Test dot and cross
    cout << "v1 dot v1: " << dot(v1, v1) << endl;
    cout << "v1 dot v2: " << dot(v1, v2) << endl;
    cout << "v2 dot v2: " << dot(v2, v2) << endl;
    cout << "v2 dot v3: " << dot(v2, v3) << endl;
    cout << "v3 dot v2: " << dot(v3, v2) << endl;
    cout << "v1 cross v1" << cross(v1, v1) << endl;
    cout << "v1 cross v2" << cross(v1, v2) << endl;
    cout << "v2 cross v2" << cross(v2, v2) << endl;
    cout << "v2 cross v3" << cross(v2, v3) << endl;
    cout << "v3 cross v2" << cross(v3, v2) << endl;

//    Test c * vec4
    cout << "0 * v1: " << (0 * v1) << endl;
    cout << "99 * v1: " << (99 * v1) << endl;
    cout << "0 * v2: " << (0 * v2) << endl;
    cout << "1 * v2: " << (1 * v2) << endl;
    cout << "2 * v2: " << (2 * v2) << endl;

    cout << "Matrix tests\n";
    mat4 id;
    cout << "empty constructor id: " << id << endl;
    mat4 m1(9);
    cout << "diagonal constructor m1: " << m1 << endl;
    mat4 m2(v1, v2, v4, v3);
    cout << "vec4 constructor m2: " << m2 << endl;
    mat4 m3(m2);
//    m1(-1); //should crash and does
    cout << "copy constructor m3: " << m3 << endl;
    cout << "first column of m2: " << (m2[0]) <<  endl;
    cout << "second column of m2: " << (m2(1)) <<  endl;
    cout << "third column of m2: " << (m2(2)) <<  endl;
    cout << "forth column of m2: " << (m2[3]) <<  endl;
//    m1(4); //should crash and does
//    Testing Rotation, Translation, and Scaling matrix
    cout << "90 Rotation about x-axis" << m2.rot(90, 1, 0 , 0) << endl;
    cout << "90 Rotation about y-axis" << m2.rot(90, 0, 1 , 0) << endl;
    cout << "90 Rotation about z-axis" << m2.rot(90, 0, 0 , 1) << endl;
    cout << "180 Rotation about x-axis" << m2.rot(180, 1, 0 , 0) << endl;
    cout << "180 Rotation about y-axis" << m2.rot(180, 0, 1 , 0) << endl;
    cout << "180 Rotation about z-axis" << m2.rot(180, 0, 0 , 1) << endl;
    cout << "45 Rotation about (1, 1, 1)" << m2.rot(45, 1, 1, 1) << endl;
    cout << "(0, 0, 0) Translation" << m2.trans(0, 0, 0) << endl;
    cout << "(1, 0, 0) Translation" << m2.trans(1, 0, 0) << endl;
    cout << "(0, 1, 0) Translation" << m2.trans(0, 1, 0) << endl;
    cout << "(0, 0, 1) Translation" << m2.trans(0, 0, 1) << endl;
    cout << "(1, 2, 3) Translation" << m2.trans(1, 2, 3) << endl;
    cout << "(0, 0, 0) Scaling" << m2.scale(0, 0, 0) << endl;
    cout << "(1, 0, 0) Scaling" << m2.scale(1, 0, 0) << endl;
    cout << "(0, 1, 0) Scaling" << m2.scale(0, 1, 0) << endl;
    cout << "(0, 0, 1) Scaling" << m2.scale(0, 0, 1) << endl;
    cout << "(2, 3, 4) Scaling" << m2.scale(2, 3, 4) << endl;

//    Test assignment
    mat4 m4;
    cout << "assignment m4 = m2" << (m4 = m2) << endl;

//    Test boolean operators
    cout << "m2 == m1: " << (m2 == m1) <<  endl;
    cout << "m2 == m2: " << (m2 == m2) <<  endl;
    cout << "m2 == m4: " << (m2 == m4) <<  endl;
    cout << "m2 != m1: " << (m2 != m1) <<  endl;
    cout << "m2 != m2: " << (m2 != m2) <<  endl;
    cout << "m2 != m4: " << (m2 != m4) <<  endl;

//    Test non-const operators
    cout << "m1 += m1" << (m1 += m1) << endl;
    cout << "m1 += m2" << (m1 += m2) << endl;
    cout << "m1 -= m2" << (m1 -= m2) << endl;
    cout << "m1 -= m1" << (m1 -= m1) << endl;
    cout << "m1 *= 1" << (m1 *= 1) <<  endl;
    cout << "m1 *= 2" << (m1 *= 2) <<  endl;
    cout << "m2 *= 1" << (m2 *= 1) << endl;
    cout << "m2 *= 2" << (m2 *= 2) << endl;
    cout << "m1 /= 1" << (m1 /= 1) << endl;
    cout << "m1 /= 2" << (m1 /= 2) << endl;
    cout << "m2 /= 1" << (m2 /= 1) << endl;
    cout << "m2 /= 2" << (m2 /= 2) << endl;

//    Test const operators
    cout << "m1 + m1" << (m1 + m1) << endl;
    cout << "m1 + m2" << (m1 + m2) << endl;
    cout << "m1 - m2" << (m1 - m2) << endl;
    cout << "m1 - m1" << (m1 - m1) << endl;
    cout << "m1 * 1" << (m1 * 1) <<  endl;
    cout << "m1 * 2" << (m1 * 2) <<  endl;
    cout << "m2 * 1" << (m2 * 1) << endl;
    cout << "m2 * 2" << (m2 * 2) << endl;
    cout << "m1 / 1" << (m1 / 1) << endl;
    cout << "m1 / 2" << (m1 / 2) << endl;
    cout << "m2 / 1" << (m2 / 1) << endl;
    cout << "m2 / 2" << (m2 / 2) << endl;

//    Test matrix and vector multiplication
    cout << "id * m1" << (id * m1) << endl;
    cout << "m1 * id" << (m1 * id) << endl;
    cout << "id * m2" << (id * m2) << endl;
    cout << "m1 * m2" << (m1 * m2) << endl;
    cout << "m2 * m4" << (m2 * m4) << endl;
    cout << "id * v1: " << (id * v1) << endl;
    cout << "id * v2: " << (id * v2) << endl;
    cout << "id * v3: " << (id * v3) << endl;
    cout << "m1 * v1: " << (m1 * v1) << endl;
    cout << "m1 * v2: " << (m1 * v2) << endl;
    cout << "m1 * v3: " << (m1 * v3) << endl;
    cout << "m2 * v1: " << (m2 * v1) << endl;
    cout << "m2 * v2: " << (m2 * v2) << endl;
    cout << "m2 * v3: " << (m2 * v3) << endl;

//    Test transpose and column
    cout << "transpose id" << (id.transpose()) << endl;
    cout << "transpose m1" << (m1.transpose()) << endl;
    cout << "transpose m2" << (m2.transpose()) << endl;
    cout << "m2 transpose first column: " << (m2.transpose().col(0)) << endl;
    cout << "m2 transpose first column: " << (m2.transpose().col(1)) << endl;
    cout << "m2 transpose first column: " << (m2.transpose().col(2)) << endl;
    cout << "m2 transpose irst column: " << (m2.transpose().col(3)) << endl;
}
