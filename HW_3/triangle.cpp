#include "triangle.h"
#include "vec4.h"
#include <cmath>

triangle::triangle() {
    vec4 v(0, 0, 0, 0);
    vert[0] = v;
    vert[1] = v;
    vert[2] = v;
}

triangle::triangle(vec4 v1, vec4 v2, vec4 v3) {
    vert[0] = v1;
    vert[1] = v2;
    vert[2] = v3;
}

vec4& triangle::operator [](unsigned int index) {
    return vert[index];
}

const vec4& triangle::operator [](unsigned int index) const {
    const vec4 &v = vert[index];
    return v;
}

void triangle::setBounds(int minx, int maxx, int miny, int maxy) {
    min_x = minx;
    max_x = maxx;
    min_y = miny;
    max_y = maxy;
    if (std::abs(vert[1][0] - vert[0][0]) < .000000001) {
        m0.isVert = true;
        m0.isFlat = false;
        if (vert[0][0] == min_x) {
            vertAtStart = true;
            vertAtEnd = false;
            vertical = min_x;
        }
        else {
            vertAtStart = false;
            vertAtEnd = true;
            vertical = max_x;
        }
    }
    else {
        m0.m = (vert[1][1] - vert[0][1]) / (vert[1][0] - vert[0][0]);
        m0.isVert = false;
        if (std::abs(m0.m) < .000000001) {
            m0.isFlat = true;
        }
        else {
            m0.isFlat = false;
        }
        b0 = vert[0][1] - m0.m * vert[0][0];
    }
    if (std::abs(vert[2][0] - vert[1][0]) < .000000001) {
        m1.isVert = true;
        m1.isFlat = false;
        if (vert[1][0] == min_x) {
            vertAtStart = true;
            vertAtEnd = false;
            vertical = min_x;
        }
        else {
            vertAtStart = false;
            vertAtEnd = true;
            vertical = max_x;
        }
    }
    else {
        m1.m = (vert[2][1] - vert[1][1]) / (vert[2][0] - vert[1][0]);
        m1.isVert = false;
        if (std::abs(m1.m) < .000000001) {
            m1.isFlat = true;
        }
        else {
            m1.isFlat = false;
        }
        b1 = vert[1][1] - m1.m * vert[1][0];
    }
    if (std::abs(vert[0][0] - vert[2][0]) < .000000001) {
        m2.isVert = true;
        m2.isFlat = false;
        if (vert[2][0] == min_x) {
            vertAtStart = true;
            vertAtEnd = false;
            vertical = min_x;
        }
        else {
            vertAtStart = false;
            vertAtEnd = true;
            vertical = max_x;
        }
    }
    else {
        m2.m = (vert[0][1] - vert[2][1]) / (vert[0][0] - vert[2][0]);
        m2.isVert = false;
        if (std::abs(m2.m) < .000000001) {
            m2.isFlat = true;
        }
        else {
            m2.isFlat = false;
        }
        b2 = vert[2][1] - m2.m * vert[2][0];
    }
}

vec4 triangle::getBoundingBox() const{
    vec4 v(min_x, max_x, min_y, max_y);
    return v;
}

int triangle::inBox(int x1, int x2, int defaul) {
    if (x1 >= min_x && x1 <= max_x) {
        return x1;
    }
    else if (x2 >= min_x && x2 <= max_x) {
        return x2;
    }
    else {
        return defaul;
    }
}

void triangle::getRange(int x1, int x2, int x3) {
    if (x1 < min_x || x1 > max_x) {
        t.x_start = fmin(x2, x3);
        t.x_end = fmax(x2, x3);
    }
    else if (x2 < min_x || x2 > max_x) {
        t.x_start = fmin(x1, x3);
        t.x_end = fmax(x1, x3);
    }
    else if (x3 < min_x && x3 > max_x) {
        t.x_start = fmin(x2, x1);
        t.x_end = fmax(x2, x1);
    }
    else {
        t.x_end = 0;
        t.x_start = 0;
    }
}

void triangle::getPixels() {
    pix = (triple *) malloc(sizeof(triple) * (max_y - min_y));
//    printf("m0: %f, %d, %d\n", m0.m, m0.isVert, m0.isFlat);
//    printf("m1: %f, %d, %d\n", m1.m, m1.isVert, m1.isFlat);
//    printf("m2: %f, %d, %d\n", m2.m, m2.isVert, m2.isFlat);
//    printf("verts: %d, %d\n", vertAtStart, vertAtEnd);
    if (vertAtStart) {
        if (m0.isVert) {
            printf("m0.isVert\n");
            for (int i = min_y; i < max_y; i = i + 1) {
                float y = i + .5;
                int e_x1 = roundf((y - b1) / m1.m);
                int e_x2 = roundf((y - b2) / m2.m);
                int e_x = inBox(e_x1, e_x2, vertical);
                pix[i - min_y].x_start = vertical;
                pix[i - min_y].x_end = e_x;
                pix[i - min_y].y = y;
            }
        }
        else if (m1.isVert) {
            printf("m1.isVert\n");
            for (int i = min_y; i < max_y; i = i + 1) {
                float y = i + .5;
                int e_x0 = roundf((y - b0) / m0.m);
                int e_x2 = roundf((y - b2) / m2.m);
                int e_x = inBox(e_x0, e_x2, vertical);
                pix[i - min_y].x_start = vertical;
                pix[i - min_y].x_end = e_x;
                pix[i - min_y].y = y;
            }
        }
        else {
            printf("m2.isVert\n");
            for (int i = min_y; i < max_y; i = i + 1) {
                float y = i + .5;
                int e_x1 = roundf((y - b1) / m1.m);
                int e_x0 = roundf((y - b0) / m0.m);
                int e_x = inBox(e_x1, e_x0, vertical);
                pix[i - min_y].x_start = vertical;
                pix[i - min_y].x_end = e_x;
                pix[i - min_y].y = y;
//                printf("%d: (%d, %d, %f)\n", i - min_y, pix[i - min_y].x_start, pix[i - min_y].x_end, pix[i - min_y].y);
            }
        }
    }
    else if (vertAtEnd) {
        if (m0.isVert) {
            for (int i = min_y; i < max_y; i = i + 1) {
                float y = i + .5;
                int e_x1 = roundf((y - b1) / m1.m);
                int e_x2 = roundf((y - b2) / m2.m);
                int e_x = inBox(e_x1, e_x2, vertical);
                pix[i - min_y].x_start = e_x;
                pix[i - min_y].x_end = vertical;
                pix[i - min_y].y = y;
            }
        }
        else if (m1.isVert) {
            for (int i = min_y; i < max_y; i = i + 1) {
                float y = i + .5;
                int e_x0 = roundf((y - b0) / m0.m);
                int e_x2 = roundf((y - b2) / m2.m);
                int e_x = inBox(e_x0, e_x2, vertical);
                pix[i - min_y].x_start = e_x;
                pix[i - min_y].x_end = vertical;
                pix[i - min_y].y = y;
//                printf("%d: (%d, %d, %f)\n", i - min_y, pix[i - min_y].x_start, pix[i - min_y].x_end, pix[i - min_y].y);
            }
        }
        else {
            for (int i = min_y; i < max_y; i = i + 1) {
                float y = i + .5;
                int e_x1 = roundf((y - b1) / m1.m);
                int e_x0 = roundf((y - b0) / m0.m);
                int e_x = inBox(e_x1, e_x0, vertical);
                pix[i - min_y].x_start = e_x;
                pix[i - min_y].x_end = vertical;
                pix[i - min_y].y = y;
            }
        }
    }
    else {
        for (int i = min_y; i < max_y; i = i + 1) {
            float y = i + .5;
            int e_x0 = roundf((y - b0) / m0.m);
            int e_x1 = roundf((y - b1) / m1.m);
            int e_x2 = roundf((y - b2) / m2.m);
            getRange(e_x0, e_x1, e_x2);
            pix[i - min_y].x_start = t.x_start;
            pix[i - min_y].x_end = t.x_end;
            pix[i - min_y].y = y;
        }
    }
    for (int j = 0; j < (max_y - min_y); j = j + 1) {
        printf("%d: (%d, %d, %f)\n", j, pix[j].x_start, pix[j].x_end, pix[j].y);
    }
}

std::ostream &operator<<(std::ostream &o, const triangle &t) {
    o  << "vertex_1: "<< t[0] << std::endl << "vertex_2: "<< t[1] << std::endl << "vertex_3: "<< t[2] << std::endl << "Box: " << t.getBoundingBox() << std::endl;
    return o;
}
