#include "triangle.h"
#include "vec4.h"
#include <cmath>

#define CLAMP(x) (x> 255 ? 255: ((x) < 0 ? 0 : (x)))

triangle::triangle() {
    vec4 v(0, 0, 0, 0);
    vert[0] = v;
    vert[1] = v;
    vert[2] = v;
}

triangle::triangle(vec4 v1, vec4 v2, vec4 v3, vec4 n1, vec4 n2, vec4 n3, vec4 diff) {
    vert[0] = v1;
    vert[1] = v2;
    vert[2] = v3;
    norm[0] = n1;
    norm[1] = n2;
    norm[2] = n3;
    diffuse = diff;
    r.y = -1;
    rc.y = -1;
    int red = CLAMP(roundf(diffuse[0] * 255));
    int green = CLAMP(roundf(diffuse[1] * 255));
    int blue = CLAMP(roundf(diffuse[2] * 255));
    diffuse_rgb = *(new vec4(red, green, blue, 0));
    vertAtEnd = false;
    vertAtStart = false;
}

vec4& triangle::operator [](unsigned int index) {
    return vert[index];
}

const vec4& triangle::operator [](unsigned int index) const {
    const vec4 &v = vert[index];
    return v;
}

void triangle::rotateNorms(camera &c) {
    norm[0] = c.rotate(norm[0]);
    norm[1] = c.rotate(norm[1]);
    norm[2] = c.rotate(norm[2]);
    float red0 = (norm[0][0] + 1) * 255.0 / 2.0;
    float green0 = (norm[0][1] + 1) * 255.0 / 2.0;
    float blue0 = (norm[0][2] + 1) * 255.0 / 2.0;
    float red1 = (norm[1][0] + 1) * 255.0 / 2.0;
    float green1 = (norm[1][1] + 1) * 255.0 / 2.0;
    float blue1 = (norm[1][2] + 1) * 255.0 / 2.0;
    float red2 = (norm[2][0] + 1) * 255.0 / 2.0;
    float green2 = (norm[2][1] + 1) * 255.0 / 2.0;
    float blue2 = (norm[2][2] + 1) * 255.0 / 2.0;
    vert_color[0] = *(new vec4(red0, green0, blue0, 0));
    vert_color[1] = *(new vec4(red1, green1, blue1, 0));
    vert_color[2] = *(new vec4(red2, green2, blue2, 0));
}

void triangle::setBounds(int minx, int maxx, int miny, int maxy) {
    min_x = minx;
    max_x = maxx;
    min_y = miny;
    max_y = maxy;
    if (std::abs(vert[1][0] - vert[0][0]) < .0000001) {
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
        if (std::abs(m0.m) < .0000001) {
            m0.isFlat = true;
        }
        else {
            m0.isFlat = false;
        }
        b0 = vert[0][1] - m0.m * vert[0][0];
    }
    if (std::abs(vert[2][0] - vert[1][0]) < .0000001) {
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
        if (std::abs(m1.m) < .0000001) {
            m1.isFlat = true;
        }
        else {
            m1.isFlat = false;
        }
        b1 = vert[1][1] - m1.m * vert[1][0];
    }
    if (std::abs(vert[0][0] - vert[2][0]) < .0000001) {
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
        if (std::abs(m2.m) < .0000001) {
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

void triangle::withRangeV(int x1, int x2, int x3, int v, bool vertStarted) {
    int sx1 = fmin(vert[0][0], vert[1][0]);
    int ex1 = fmax(vert[0][0], vert[1][0]);
    int sx2 = fmin(vert[1][0], vert[2][0]);
    int ex2 = fmax(vert[1][0], vert[2][0]);
    int sx3 = fmin(vert[2][0], vert[0][0]);
    int ex3 = fmax(vert[2][0], vert[0][0]);
    if (v == 0) {
        if ((x2 >= sx2 && x2 <= ex2) && (x3 >= sx3 && x3 <= ex3) && !vertStarted) {
            t.x_start = fmax(fmin(x2, x3), min_x);
            t.x_end = fmin(fmax(x2, x3), max_x);
        }
        else {
            if (vertAtStart) {
                t.x_start = x1;
                t.x_end = inBox(x2, x3, max_x);
            }
            else {
                t.x_start = inBox(x2, x3, max_x);
                t.x_end = x1;
            }
        }
    }
    else if (v == 1) {
        if ((x1 >= sx1 && x1 <= ex1) && (x3 >= sx3 && x3 <= ex3) && !vertStarted) {
            t.x_start = fmax(fmin(x1, x3), min_x);
            t.x_end = fmin(fmax(x1, x3), max_x);
        }
        else {
            if (vertAtStart) {
                t.x_start = x2;
                t.x_end = inBox(x1, x3, max_x);
            }
            else {
                t.x_start = inBox(x1, x3, max_x);
                t.x_end = x2;
            }
        }
    }
    else {
        if ((x1 >= sx1 && x1 <= ex1) && (x2 >= sx2 && x2 <= ex2) && !vertStarted) {
            t.x_start = fmax(fmin(x1, x2), min_x);
            t.x_end = fmin(fmax(x1, x2), max_x);
        }
        else {
            if (vertAtStart) {
                t.x_start = x3;
                t.x_end = inBox(x1, x2, max_x);
            }
            else {
                t.x_start = inBox(x1, x2, max_x);
                t.x_end = x3;
            }
        }
    }
}

void triangle::getRange(int x1, int x2, int x3, float y) {
    int sy1 = fmin(vert[0][1], vert[1][1]);
    int ey1 = fmax(vert[0][1], vert[1][1]);
    int sy2 = fmin(vert[1][1], vert[2][1]);
    int ey2 = fmax(vert[1][1], vert[2][1]);
    int sy3 = fmin(vert[2][1], vert[0][1]);
    int ey3 = fmax(vert[2][1], vert[0][1]);
    if ((y < sy1 || y > ey1)) {
        t.x_start = fmax(fmin(x2, x3), min_x);
        t.x_end = fmin(fmax(x2, x3), max_x);
    }
    else if (y < sy2 || y > ey2) {
        t.x_start = fmax(fmin(x1, x3), min_x);
        t.x_end = fmin(fmax(x1, x3), max_x);
    }
    else if (y < sy3 || y > ey3) {
        t.x_start = fmax(fmin(x2, x1), min_x);
        t.x_end = fmin(fmax(x2, x1), max_x);
    }
    else {
        t.x_end = 0;
        t.x_start = 0;
    }
}

bool triangle::vertStart(float y) {
    if (m0.isVert) {
        return y >= fmin(vert[0][1], vert[1][1]) && y <= fmax(vert[0][1], vert[1][1]);
    }
    else if (m1.isVert) {
        return y >= fmin(vert[1][1], vert[2][1]) && y <= fmax(vert[1][1], vert[2][1]);
    }
    else {
        return y >= fmin(vert[0][1], vert[2][1]) && y <= fmax(vert[0][1], vert[2][1]);
    }
}

void triangle::getPixels() {
    pix = (triple *) malloc(sizeof(triple) * (max_y - min_y));
    if (vertAtStart) {
        if (m0.isVert) {
            for (int i = min_y; i < max_y; i = i + 1) {
                float y = i + .5;
                int e_x1 = roundf((y - b1) / m1.m);
                int e_x2 = roundf((y - b2) / m2.m);
                withRangeV(vertical, e_x1, e_x2, 0, vertStart(y));
                pix[i - min_y].x_start = t.x_start;
                pix[i - min_y].x_end = t.x_end;
                pix[i - min_y].y = y;
            }
        }
        else if (m1.isVert) {
            for (int i = min_y; i < max_y; i = i + 1) {
                float y = i + .5;
                int e_x0 = roundf((y - b0) / m0.m);
                int e_x2 = roundf((y - b2) / m2.m);
                withRangeV(e_x0, vertical, e_x2, 1, vertStart(y));
                pix[i - min_y].x_start = t.x_start;
                pix[i - min_y].x_end = t.x_end;
                pix[i - min_y].y = y;
            }
        }
        else {
            for (int i = min_y; i < max_y; i = i + 1) {
                float y = i + .5;
                int e_x1 = roundf((y - b1) / m1.m);
                int e_x0 = roundf((y - b0) / m0.m);
                withRangeV(e_x0, e_x1, vertical, 2, vertStart(y));
                pix[i - min_y].x_start = t.x_start;
                pix[i - min_y].x_end = t.x_end;
                pix[i - min_y].y = y;
            }
        }
    }
    else if (vertAtEnd) {
        if (m0.isVert) {
            for (int i = min_y; i < max_y; i = i + 1) {
                float y = i + .5;
                int e_x1 = roundf((y - b1) / m1.m);
                int e_x2 = roundf((y - b2) / m2.m);
                withRangeV(vertical, e_x1, e_x2, 0, vertStart(y));
                pix[i - min_y].x_start = t.x_start;
                pix[i - min_y].x_end = t.x_end;
                pix[i - min_y].y = y;
            }
        }
        else if (m1.isVert) {
            for (int i = min_y; i < max_y; i = i + 1) {
                float y = i + .5;
                int e_x0 = roundf((y - b0) / m0.m);
                int e_x2 = roundf((y - b2) / m2.m);
                withRangeV(e_x0, vertical, e_x2, 1, vertStart(y));
                pix[i - min_y].x_start = t.x_start;
                pix[i - min_y].x_end = t.x_end;
                pix[i - min_y].y = y;
            }
        }
        else {
            for (int i = min_y; i < max_y; i = i + 1) {
                float y = i + .5;
                int e_x1 = roundf((y - b1) / m1.m);
                int e_x0 = roundf((y - b0) / m0.m);
                withRangeV(e_x0, e_x1, vertical, 2, vertStart(y));
                pix[i - min_y].x_start = t.x_start;
                pix[i - min_y].x_end = t.x_end;
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
            getRange(e_x0, e_x1, e_x2, y);
            pix[i - min_y].x_start = t.x_start;
            pix[i - min_y].x_end = t.x_end;
            pix[i - min_y].y = y;
        }
    }
}

float triangle::getZ(int sx, int ex, float y, int mx, int method) {
    if (method == 0) {
        return trilinear(sx, ex, y, mx);
    }
    else if (method == 1) {
        return trilinear_perspective(sx, ex, y, mx);
    }
    else {
        return bary_z(y, mx);
    }
}

int triangle::smallest(float i0, float i1, float i2, bool f0, bool f1, bool f2) {
    if (i0 <= i1 && i0 <= i2 && !f0) {
        return 0;
    }
    else if (i1 <= i0 && i1 <= i2 && !f1) {
        return 1;
    }
    else if (!f2){
        return 2;
    }
    else {
        return -1;
    }
}

float triangle::trilinear(int sx, int ex, float y, int mx) {
    float sx_z;
    float ex_z;
    if (std::abs(r.y - y) < .0000001) {
        sx_z = r.start_z;
        ex_z = r.end_z;
    }
    else {
        int index = smallest(std::abs((sx * m0.m + b0) - y), std::abs((sx * m1.m + b1) - y), std::abs((sx * m2.m + b2) - y), m0.isFlat, m1.isFlat, m2.isFlat);
        if (vertAtStart && sx == vertical) {
            if (m0.isVert) {
                float side_len = std::abs(vert[0][1] - vert[1][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d1 = side_len - d0;
                sx_z = d0 / side_len * vert[1][2] + d1 / side_len * vert[0][2];
            }
            else if (m1.isVert) {
                float side_len = std::abs(vert[1][1] - vert[2][1]);
                float d1 = std::abs(vert[1][1] - y);
                float d2 = side_len - d1;
                sx_z = d1 / side_len * vert[2][2] + d2 / side_len * vert[1][2];
            }
            else {
                float side_len = std::abs(vert[2][1] - vert[0][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d2 = side_len - d0;
                sx_z = d0 / side_len * vert[2][2] + d2 / side_len * vert[0][2];
            }
        }
        else if (index == 0) {
            float side_len = sqrt((vert[0][0] - vert[1][0]) * (vert[0][0] - vert[1][0]) + (vert[0][1] - vert[1][1]) * (vert[0][1] - vert[1][1]));
            float d0 = sqrt((vert[0][0] - sx) * (vert[0][0] - sx) + (vert[0][1] - y) * (vert[0][1] - y));
            float d1 = side_len - d0;
            sx_z = d0 / side_len * vert[1][2] + d1 / side_len * vert[0][2];
        }
        else if (index == 1) {
            float side_len = sqrt((vert[1][0] - vert[2][0]) * (vert[1][0] - vert[2][0]) + (vert[1][1] - vert[2][1]) * (vert[1][1] - vert[2][1]));
            float d1 = sqrt((vert[1][0] - sx) * (vert[1][0] - sx) + (vert[1][1] - y) * (vert[1][1] - y));
            float d2 = side_len - d1;
            sx_z = d1 / side_len * vert[2][2] + d2 / side_len * vert[1][2];
        }
        else {
            float side_len = sqrt((vert[0][0] - vert[2][0]) * (vert[0][0] - vert[2][0]) + (vert[0][1] - vert[2][1]) * (vert[0][1] - vert[2][1]));
            float d0 = sqrt((vert[0][0] - sx) * (vert[0][0] - sx) + (vert[0][1] - y) * (vert[0][1] - y));
            float d2 = side_len - d0;
            sx_z = d2 / side_len * vert[0][2] + d0 / side_len * vert[2][2];
        }
        index = smallest(std::abs((ex * m0.m + b0) - y), std::abs((ex * m1.m + b1) - y), std::abs((ex * m2.m + b2) - y), m0.isFlat, m1.isFlat, m2.isFlat);
        if (vertAtEnd && ex == vertical) {
            if (m0.isVert) {
                float side_len = std::abs(vert[0][1] - vert[1][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d1 = side_len - d0;
                ex_z = d0 / side_len * vert[1][2] + d1 / side_len * vert[0][2];
            }
            else if (m1.isVert) {
                float side_len = std::abs(vert[1][1] - vert[2][1]);
                float d1 = std::abs(vert[1][1] - y);
                float d2 = side_len - d1;
                ex_z = d1 / side_len * vert[2][2] + d2 / side_len * vert[1][2];
            }
            else {
                float side_len = std::abs(vert[2][1] - vert[0][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d2 = side_len - d0;
                ex_z = d0 / side_len * vert[2][2] + d2 / side_len * vert[0][2];
            }
        }
        else if (index == 0) {
            float side_len = sqrt((vert[0][0] - vert[1][0]) * (vert[0][0] - vert[1][0]) + (vert[0][1] - vert[1][1]) * (vert[0][1] - vert[1][1]));
            float d0 = sqrt((vert[0][0] - ex) * (vert[0][0] - ex) + (vert[0][1] - y) * (vert[0][1] - y));
            float d1 = side_len - d0;
            ex_z = d0 / side_len * vert[1][2] + d1 / side_len * vert[0][2];
        }
        else if (index == 1) {
            float side_len = sqrt((vert[1][0] - vert[2][0]) * (vert[1][0] - vert[2][0]) + (vert[1][1] - vert[2][1]) * (vert[1][1] - vert[2][1]));
            float d1 = sqrt((vert[1][0] - ex) * (vert[1][0] - ex) + (vert[1][1] - y) * (vert[1][1] - y));
            float d2 = side_len - d1;
            ex_z = d1 / side_len * vert[2][2] + d2 / side_len * vert[1][2];
        }
        else {
            float side_len = sqrt((vert[0][0] - vert[2][0]) * (vert[0][0] - vert[2][0]) + (vert[0][1] - vert[2][1]) * (vert[0][1] - vert[2][1]));
            float d0 = sqrt((vert[0][0] - ex) * (vert[0][0] - ex) + (vert[0][1] - y) * (vert[0][1] - y));
            float d2 = side_len - d0;
            ex_z = d2 / side_len * vert[0][2] + d0 / side_len * vert[2][2];
        }
        r.y = y;
        r.start_z = sx_z;
        r.end_z = ex_z;
    }
    if (std::abs(ex - sx) < .0000001) {
        int py = roundf(y + .5);
        if (vert[0][0] == ex && vert[0][1] == py) {
            recent_z = vert[0][2];
            return recent_z;
        }
        else if (vert[1][0] == ex && vert[1][1] == py) {
            recent_z = vert[1][2];
            return recent_z;
        }
        else {
            recent_z = vert[2][2];
            return recent_z;
        }
    }
    recent_z = ((float) ex-mx) / ((float) ex-sx) * sx_z + ((float) mx - sx) / ((float) ex-sx) * ex_z;
    return recent_z;
}

float triangle::trilinear_perspective(int sx, int ex, float y, int mx) {
    float inverse_sx_z;
    float inverse_ex_z;
    if (std::abs(r.y - y) < .0000001) {
        inverse_sx_z = 1.0 / r.start_z;
        inverse_ex_z = 1.0 / r.end_z;
    }
    else {
        int index = smallest(std::abs((sx * m0.m + b0) - y), std::abs((sx * m1.m + b1) - y), std::abs((sx * m2.m + b2) - y), m0.isFlat, m1.isFlat, m2.isFlat);
        if (vertAtStart && sx == vertical) {
            if (m0.isVert) {
                float side_len = std::abs(vert[0][1] - vert[1][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d1 = side_len - d0;
                inverse_sx_z = (d0 / side_len) / vert[1][2] + (d1 / side_len) / vert[0][2];
            }
            else if (m1.isVert) {
                float side_len = std::abs(vert[1][1] - vert[2][1]);
                float d1 = std::abs(vert[1][1] - y);
                float d2 = side_len - d1;
                inverse_sx_z = d1 / side_len / vert[2][2] + d2 / side_len / vert[1][2];
            }
            else {
                float side_len = std::abs(vert[2][1] - vert[0][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d2 = side_len - d0;
                inverse_sx_z = d0 / side_len / vert[2][2] + d2 / side_len / vert[0][2];
            }
        }
        else if (index == 0) {
            float side_len = sqrt((vert[0][0] - vert[1][0]) * (vert[0][0] - vert[1][0]) + (vert[0][1] - vert[1][1]) * (vert[0][1] - vert[1][1]));
            float d0 = sqrt((vert[0][0] - sx) * (vert[0][0] - sx) + (vert[0][1] - y) * (vert[0][1] - y));
            float d1 = side_len - d0;
            inverse_sx_z = d0 / side_len / vert[1][2] + d1 / side_len / vert[0][2];
        }
        else if (index == 1) {
            float side_len = sqrt((vert[1][0] - vert[2][0]) * (vert[1][0] - vert[2][0]) + (vert[1][1] - vert[2][1]) * (vert[1][1] - vert[2][1]));
            float d1 = sqrt((vert[1][0] - sx) * (vert[1][0] - sx) + (vert[1][1] - y) * (vert[1][1] - y));
            float d2 = side_len - d1;
            inverse_sx_z = d1 / side_len / vert[2][2] + d2 / side_len / vert[1][2];
        }
        else {
            float side_len = sqrt((vert[0][0] - vert[2][0]) * (vert[0][0] - vert[2][0]) + (vert[0][1] - vert[2][1]) * (vert[0][1] - vert[2][1]));
            float d0 = sqrt((vert[0][0] - sx) * (vert[0][0] - sx) + (vert[0][1] - y) * (vert[0][1] - y));
            float d2 = side_len - d0;
            inverse_sx_z = d2 / side_len / vert[0][2] + d0 / side_len / vert[2][2];
        }
        index = smallest(std::abs((ex * m0.m + b0) - y), std::abs((ex * m1.m + b1) - y), std::abs((ex * m2.m + b2) - y), m0.isFlat, m1.isFlat, m2.isFlat);
        if (vertAtEnd && ex == vertical) {
            if (m0.isVert) {
                float side_len = std::abs(vert[0][1] - vert[1][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d1 = side_len - d0;
                inverse_ex_z = d0 / side_len / vert[1][2] + d1 / side_len / vert[0][2];
            }
            else if (m1.isVert) {
                float side_len = std::abs(vert[1][1] - vert[2][1]);
                float d1 = std::abs(vert[1][1] - y);
                float d2 = side_len - d1;
                inverse_ex_z = d1 / side_len / vert[2][2] + d2 / side_len / vert[1][2];
            }
            else {
                float side_len = std::abs(vert[2][1] - vert[0][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d2 = side_len - d0;
                inverse_ex_z = d0 / side_len / vert[2][2] + d2 / side_len / vert[0][2];
            }
        }
        else if (index == 0) {
            float side_len = sqrt((vert[0][0] - vert[1][0]) * (vert[0][0] - vert[1][0]) + (vert[0][1] - vert[1][1]) * (vert[0][1] - vert[1][1]));
            float d0 = sqrt((vert[0][0] - ex) * (vert[0][0] - ex) + (vert[0][1] - y) * (vert[0][1] - y));
            float d1 = side_len - d0;
            inverse_ex_z = d0 / side_len / vert[1][2] + d1 / side_len / vert[0][2];
        }
        else if (index == 1) {
            float side_len = sqrt((vert[1][0] - vert[2][0]) * (vert[1][0] - vert[2][0]) + (vert[1][1] - vert[2][1]) * (vert[1][1] - vert[2][1]));
            float d1 = sqrt((vert[1][0] - ex) * (vert[1][0] - ex) + (vert[1][1] - y) * (vert[1][1] - y));
            float d2 = side_len - d1;
            inverse_ex_z = d1 / side_len / vert[2][2] + d2 / side_len / vert[1][2];
        }
        else {
            float side_len = sqrt((vert[0][0] - vert[2][0]) * (vert[0][0] - vert[2][0]) + (vert[0][1] - vert[2][1]) * (vert[0][1] - vert[2][1]));
            float d0 = sqrt((vert[0][0] - ex) * (vert[0][0] - ex) + (vert[0][1] - y) * (vert[0][1] - y));
            float d2 = side_len - d0;
            inverse_ex_z = d2 / side_len / vert[0][2] + d0 / side_len / vert[2][2];
        }
        r.y = y;
        r.start_z = 1.0 / inverse_sx_z;
        r.end_z = 1.0 / inverse_ex_z;
    }
    if (std::abs(ex - sx) < .0000001) {
        int py = roundf(y + .5);
        if (vert[0][0] == ex && vert[0][1] == py) {
            recent_z = vert[0][2];
            return recent_z;
        }
        else if (vert[1][0] == ex && vert[1][1] == py) {
            recent_z = vert[1][2];
            return recent_z;
        }
        else {
            recent_z = vert[2][2];
            return recent_z;
        }
    }
    recent_z = 1.0 / (((float) ex-mx) / ((float) ex-sx) * inverse_sx_z + ((float) mx - sx) / ((float) ex-sx) * inverse_ex_z);
    return recent_z;
}

float triangle::bary_z(float y, int mx) {
    vec4 s1(vert[1][0] - vert[0][0], vert[1][1] - vert[0][1], 0, 0);
    vec4 s2(vert[2][0] - vert[0][0], vert[2][1] - vert[0][1], 0, 0);
    float total_A = .5 * cross(s1, s2).length();
    vec4 d0(vert[0][0] - mx, vert[0][1] - y, 0, 0);
    vec4 d1(vert[1][0] - mx, vert[1][1] - y, 0, 0);
    vec4 d2(vert[2][0] - mx, vert[2][1] - y, 0, 0);
    float a0 = .5 * cross(d1, d2).length() / total_A;
    float a1 = .5 * cross(d0, d2).length() / total_A;
    float a2 = 1 - a0 - a1;
    recent_z =  1.0 / (a0 / vert[0][2] + a1 / vert[1][2] + a2 / vert[2][2]);
    return recent_z;
}

vec4 triangle::getColor(int sx, int ex, float y, int mx, int method) {
    if (method == 0) {
        return diffuse_rgb;
    }
    else if (method == 1) {
        return *(new vec4(255, 255, 255, 0));
    }
    else if (method == 2) {
        int red = roundf(vert_color[0][0]);
        int green = roundf(vert_color[0][1]);
        int blue = roundf(vert_color[0][2]);
        return *(new vec4(red, green, blue, 0));
    }
    else if (method == 3) {
        return gouraud(sx, ex, y, mx);
    }
    else if (method == 4) {
        return bary(y, mx);
    }
    else if (method == 5) {
        return gouraud_perspective(sx, ex, y, mx);
    }
    else {
        return bary_perspective(y, mx);
    }
}

vec4 triangle::bary(float y, int mx) {
    vec4 s1(vert[1][0] - vert[0][0], vert[1][1] - vert[0][1], 0, 0);
    vec4 s2(vert[2][0] - vert[0][0], vert[2][1] - vert[0][1], 0, 0);
    float total_A = .5 * cross(s1, s2).length();
    vec4 d0(vert[0][0] - mx, vert[0][1] - y, 0, 0);
    vec4 d1(vert[1][0] - mx, vert[1][1] - y, 0, 0);
    vec4 d2(vert[2][0] - mx, vert[2][1] - y, 0, 0);
    float a0 = .5 * cross(d1, d2).length() / total_A;
    float a1 = .5 * cross(d0, d2).length() / total_A;
    float a2 = 1 - a0 - a1;
    int redf = CLAMP(roundf(a0 * vert_color[0][0] + a1 * vert_color[1][0] + a2 * vert_color[2][0]));
    int greenf = CLAMP(roundf(a0 * vert_color[0][1] + a1 * vert_color[1][1] + a2 * vert_color[2][1]));
    int bluef = CLAMP(roundf(a0 * vert_color[0][2] + a1 * vert_color[1][2] + a2 * vert_color[2][2]));
    return *(new vec4(redf, greenf, bluef, 0));
}

vec4 triangle::bary_perspective(float y, int mx) {
    vec4 s1(vert[1][0] - vert[0][0], vert[1][1] - vert[0][1], 0, 0);
    vec4 s2(vert[2][0] - vert[0][0], vert[2][1] - vert[0][1], 0, 0);
    float total_A = .5 * cross(s1, s2).length();
    vec4 d0(vert[0][0] - mx, vert[0][1] - y, 0, 0);
    vec4 d1(vert[1][0] - mx, vert[1][1] - y, 0, 0);
    vec4 d2(vert[2][0] - mx, vert[2][1] - y, 0, 0);
    float a0 = .5 * cross(d1, d2).length() / total_A;
    float a1 = .5 * cross(d0, d2).length() / total_A;
    float a2 = 1 - a0 - a1;
    int redf = CLAMP(roundf(recent_z * (a0 * vert_color[0][0] / vert[0][2] + a1 * vert_color[1][0] / vert[1][2] + a2 * vert_color[2][0] / vert[2][2])));
    int greenf = CLAMP(roundf(recent_z * (a0 * vert_color[0][1] / vert[0][2] + a1 * vert_color[1][1] / vert[1][2] + a2 * vert_color[2][1] / vert[2][2])));
    int bluef = CLAMP(roundf(recent_z * (a0 * vert_color[0][2] / vert[0][2] + a1 * vert_color[1][2] / vert[1][2] + a2 * vert_color[2][2] / vert[2][2])));
    return *(new vec4(redf, greenf, bluef, 0));
}

vec4 triangle::gouraud(int sx, int ex, float y, int mx) {
    float red_sx;
    float green_sx;
    float blue_sx;
    float red_ex;
    float green_ex;
    float blue_ex;
    if (std::abs(rc.y - y) < .0000001) {
        red_sx = rc.red_sx;
        green_sx = rc.green_sx;
        blue_sx = rc.blue_sx;
        red_ex = rc.red_ex;
        green_ex = rc.green_ex;
        blue_ex = rc.blue_ex;
    }
    else {
        int index = smallest(std::abs((sx * m0.m + b0) - y), std::abs((sx * m1.m + b1) - y), std::abs((sx * m2.m + b2) - y), m0.isFlat, m1.isFlat, m2.isFlat);
        if (vertAtStart && sx == vertical) {
            if (m0.isVert) {
                float side_len = std::abs(vert[0][1] - vert[1][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d1 = side_len - d0;
                red_sx = d0 / side_len * vert_color[1][0] + d1 / side_len * vert_color[0][0];
                green_sx = d0 / side_len * vert_color[1][1] + d1 / side_len * vert_color[0][1];
                blue_sx = d0 / side_len * vert_color[1][2] + d1 / side_len * vert_color[0][2];
            }
            else if (m1.isVert) {
                float side_len = std::abs(vert[1][1] - vert[2][1]);
                float d1 = std::abs(vert[1][1] - y);
                float d2 = side_len - d1;
                red_sx = d1 / side_len * vert_color[2][0] + d2 / side_len * vert_color[1][0];
                green_sx = d1 / side_len * vert_color[2][1] + d2 / side_len * vert_color[1][1];
                blue_sx = d1 / side_len * vert_color[2][2] + d2 / side_len * vert_color[1][2];
            }
            else {
                float side_len = std::abs(vert[2][1] - vert[0][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d2 = side_len - d0;
                red_sx = d0 / side_len * vert_color[2][0] + d2 / side_len * vert_color[0][0];
                green_sx = d0 / side_len * vert_color[2][1] + d2 / side_len * vert_color[0][1];
                blue_sx = d0 / side_len * vert_color[2][2] + d2 / side_len * vert_color[0][2];
            }
        }
        else if (index == 0) {
            float side_len = sqrt((vert[0][0] - vert[1][0]) * (vert[0][0] - vert[1][0]) + (vert[0][1] - vert[1][1]) * (vert[0][1] - vert[1][1]));
            float d0 = sqrt((vert[0][0] - sx) * (vert[0][0] - sx) + (vert[0][1] - y) * (vert[0][1] - y));
            float d1 = side_len - d0;
            red_sx = d0 / side_len * vert_color[1][0] + d1 / side_len * vert_color[0][0];
            green_sx = d0 / side_len * vert_color[1][1] + d1 / side_len * vert_color[0][1];
            blue_sx = d0 / side_len * vert_color[1][2] + d1 / side_len * vert_color[0][2];
        }
        else if (index == 1) {
            float side_len = sqrt((vert[1][0] - vert[2][0]) * (vert[1][0] - vert[2][0]) + (vert[1][1] - vert[2][1]) * (vert[1][1] - vert[2][1]));
            float d1 = sqrt((vert[1][0] - sx) * (vert[1][0] - sx) + (vert[1][1] - y) * (vert[1][1] - y));
            float d2 = side_len - d1;
            red_sx = d1 / side_len * vert_color[2][0] + d2 / side_len * vert_color[1][0];
            green_sx = d1 / side_len * vert_color[2][1] + d2 / side_len * vert_color[1][1];
            blue_sx = d1 / side_len * vert_color[2][2] + d2 / side_len * vert_color[1][2];
        }
        else {
            float side_len = sqrt((vert[0][0] - vert[2][0]) * (vert[0][0] - vert[2][0]) + (vert[0][1] - vert[2][1]) * (vert[0][1] - vert[2][1]));
            float d0 = sqrt((vert[0][0] - sx) * (vert[0][0] - sx) + (vert[0][1] - y) * (vert[0][1] - y));
            float d2 = side_len - d0;
            red_sx = d0 / side_len * vert_color[2][0] + d2 / side_len * vert_color[0][0];
            green_sx = d0 / side_len * vert_color[2][1] + d2 / side_len * vert_color[0][1];
            blue_sx = d0 / side_len * vert_color[2][2] + d2 / side_len * vert_color[0][2];
        }
        index = smallest(std::abs((ex * m0.m + b0) - y), std::abs((ex * m1.m + b1) - y), std::abs((ex * m2.m + b2) - y), m0.isFlat, m1.isFlat, m2.isFlat);
        if (vertAtEnd && ex == vertical) {
            if (m0.isVert) {
                float side_len = std::abs(vert[0][1] - vert[1][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d1 = side_len - d0;
                red_ex = d0 / side_len * vert_color[1][0] + d1 / side_len * vert_color[0][0];
                green_ex = d0 / side_len * vert_color[1][1] + d1 / side_len * vert_color[0][1];
                blue_ex = d0 / side_len * vert_color[1][2] + d1 / side_len * vert_color[0][2];
            }
            else if (m1.isVert) {
                float side_len = std::abs(vert[1][1] - vert[2][1]);
                float d1 = std::abs(vert[1][1] - y);
                float d2 = side_len - d1;
                red_ex = d1 / side_len * vert_color[2][0] + d2 / side_len * vert_color[1][0];
                green_ex = d1 / side_len * vert_color[2][1] + d2 / side_len * vert_color[1][1];
                blue_ex = d1 / side_len * vert_color[2][2] + d2 / side_len * vert_color[1][2];
            }
            else {
                float side_len = std::abs(vert[2][1] - vert[0][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d2 = side_len - d0;
                red_ex = d0 / side_len * vert_color[2][0] + d2 / side_len * vert_color[0][0];
                green_ex = d0 / side_len * vert_color[2][1] + d2 / side_len * vert_color[0][1];
                blue_ex = d0 / side_len * vert_color[2][2] + d2 / side_len * vert_color[0][2];
            }
        }
        else if (index == 0) {
            float side_len = sqrt((vert[0][0] - vert[1][0]) * (vert[0][0] - vert[1][0]) + (vert[0][1] - vert[1][1]) * (vert[0][1] - vert[1][1]));
            float d0 = sqrt((vert[0][0] - ex) * (vert[0][0] - ex) + (vert[0][1] - y) * (vert[0][1] - y));
            float d1 = side_len - d0;
            red_ex = d0 / side_len * vert_color[1][0] + d1 / side_len * vert_color[0][0];
            green_ex = d0 / side_len * vert_color[1][1] + d1 / side_len * vert_color[0][1];
            blue_ex = d0 / side_len * vert_color[1][2] + d1 / side_len * vert_color[0][2];
        }
        else if (index == 1) {
            float side_len = sqrt((vert[1][0] - vert[2][0]) * (vert[1][0] - vert[2][0]) + (vert[1][1] - vert[2][1]) * (vert[1][1] - vert[2][1]));
            float d1 = sqrt((vert[1][0] - ex) * (vert[1][0] - ex) + (vert[1][1] - y) * (vert[1][1] - y));
            float d2 = side_len - d1 ;
            red_ex = d1 / side_len * vert_color[2][0] + d2 / side_len * vert_color[1][0];
            green_ex = d1 / side_len * vert_color[2][1] + d2 / side_len * vert_color[1][1];
            blue_ex = d1 / side_len * vert_color[2][2] + d2 / side_len * vert_color[1][2];
        }
        else {
            float side_len = sqrt((vert[0][0] - vert[2][0]) * (vert[0][0] - vert[2][0]) + (vert[0][1] - vert[2][1]) * (vert[0][1] - vert[2][1]));
            float d0 = sqrt((vert[0][0] - ex) * (vert[0][0] - ex) + (vert[0][1] - y) * (vert[0][1] - y));
            float d2 = side_len - d0;
            red_ex = d0 / side_len * vert_color[2][0] + d2 / side_len * vert_color[0][0];
            green_ex = d0 / side_len * vert_color[2][1] + d2 / side_len * vert_color[0][1];
            blue_ex = d0 / side_len * vert_color[2][2] + d2 / side_len * vert_color[0][2];
        }
        rc.red_sx = red_sx;
        rc.green_sx = green_sx;
        rc.blue_sx = blue_sx;
        rc.red_ex = red_ex;
        rc.green_ex = green_ex;
        rc.blue_ex = blue_ex;
        rc.y = y;
    }
    if (std::abs(ex - sx) < .0000001) {
        int py = roundf(y + .5);
        if (vert[0][0] == ex && vert[0][1] == py) {
            int red = roundf(vert_color[0][0]);
            int green = roundf(vert_color[0][1]);
            int blue = roundf(vert_color[0][2]);
            return *(new vec4(red, green, blue, 0));
        }
        else if (vert[1][0] == ex && vert[1][1] == py) {
            int red = roundf(vert_color[1][0]);
            int green = roundf(vert_color[1][1]);
            int blue = roundf(vert_color[1][2]);
            return *(new vec4(red, green, blue, 0));
        }
        else {
            int red = roundf(vert_color[2][0]);
            int green = roundf(vert_color[2][1]);
            int blue = roundf(vert_color[2][2]);
            return *(new vec4(red, green, blue, 0));
        }
    }
    int redf = roundf(((float) ex-mx) / ((float) ex-sx) * red_sx + ((float) mx - sx) / ((float) ex-sx) * red_ex);
    int greenf = roundf(((float) ex-mx) / ((float) ex-sx) * green_sx + ((float) mx - sx) / ((float) ex-sx) * green_ex);
    int bluef = roundf(((float) ex-mx) / ((float) ex-sx) * blue_sx + ((float) mx - sx) / ((float) ex-sx) * blue_ex);
    return *(new vec4(redf, greenf, bluef, 0));
}

vec4 triangle::gouraud_perspective(int sx, int ex, float y, int mx) {
    float red_sx;
    float green_sx;
    float blue_sx;
    float red_ex;
    float green_ex;
    float blue_ex;
    if (std::abs(rc.y - y) < .0000001) {
        red_sx = rc.red_sx;
        green_sx = rc.green_sx;
        blue_sx = rc.blue_sx;
        red_ex = rc.red_ex;
        green_ex = rc.green_ex;
        blue_ex = rc.blue_ex;
    }
    else {
        int index = smallest(std::abs((sx * m0.m + b0) - y), std::abs((sx * m1.m + b1) - y), std::abs((sx * m2.m + b2) - y), m0.isFlat, m1.isFlat, m2.isFlat);
        if (vertAtStart && sx == vertical) {
            if (m0.isVert) {
                float side_len = std::abs(vert[0][1] - vert[1][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d1 = side_len - d0;
                red_sx = r.start_z * (d0 / side_len * vert_color[1][0] / vert[1][2] + d1 / side_len * vert_color[0][0] / vert[0][2]);
                green_sx = r.start_z * (d0 / side_len * vert_color[1][1] / vert[1][2] + d1 / side_len * vert_color[0][1] / vert[0][2]);
                blue_sx = r.start_z * (d0 / side_len * vert_color[1][2] / vert[1][2] + d1 / side_len * vert_color[0][2] / vert[0][2]);
            }
            else if (m1.isVert) {
                float side_len = std::abs(vert[1][1] - vert[2][1]);
                float d1 = std::abs(vert[1][1] - y);
                float d2 = side_len - d1;
                red_sx = r.start_z * (d1 / side_len * vert_color[2][0] / vert[2][2] + d2 / side_len * vert_color[1][0] / vert[1][2]);
                green_sx = r.start_z * (d1 / side_len * vert_color[2][1] / vert[2][2] + d2 / side_len * vert_color[1][1] / vert[1][2]);
                blue_sx = r.start_z * (d1 / side_len * vert_color[2][2] / vert[2][2] + d2 / side_len * vert_color[1][2] / vert[1][2]);
            }
            else {
                float side_len = std::abs(vert[2][1] - vert[0][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d2 = side_len - d0;
                red_sx = r.start_z * (d0 / side_len * vert_color[2][0] / vert[2][2] + d2 / side_len * vert_color[0][0] / vert[0][2]);
                green_sx = r.start_z * (d0 / side_len * vert_color[2][1] / vert[2][2] + d2 / side_len * vert_color[0][1] / vert[0][2]);
                blue_sx = r.start_z * (d0 / side_len * vert_color[2][2] / vert[2][2] + d2 / side_len * vert_color[0][2] / vert[0][2]);
            }
        }
        else if (index == 0) {
            float side_len = sqrt((vert[0][0] - vert[1][0]) * (vert[0][0] - vert[1][0]) + (vert[0][1] - vert[1][1]) * (vert[0][1] - vert[1][1]));
            float d0 = sqrt((vert[0][0] - sx) * (vert[0][0] - sx) + (vert[0][1] - y) * (vert[0][1] - y));
            float d1 = side_len - d0;
            red_sx = r.start_z * (d0 / side_len * vert_color[1][0] / vert[1][2] + d1 / side_len * vert_color[0][0] / vert[0][2]);
            green_sx = r.start_z * (d0 / side_len * vert_color[1][1] / vert[1][2] + d1 / side_len * vert_color[0][1] / vert[0][2]);
            blue_sx = r.start_z * (d0 / side_len * vert_color[1][2] / vert[1][2] + d1 / side_len * vert_color[0][2] / vert[0][2]);
        }
        else if (index == 1) {
            float side_len = sqrt((vert[1][0] - vert[2][0]) * (vert[1][0] - vert[2][0]) + (vert[1][1] - vert[2][1]) * (vert[1][1] - vert[2][1]));
            float d1 = sqrt((vert[1][0] - sx) * (vert[1][0] - sx) + (vert[1][1] - y) * (vert[1][1] - y));
            float d2 = side_len - d1;
            red_sx = r.start_z * (d1 / side_len * vert_color[2][0] / vert[2][2] + d2 / side_len * vert_color[1][0] / vert[1][2]);
            green_sx = r.start_z * (d1 / side_len * vert_color[2][1] / vert[2][2] + d2 / side_len * vert_color[1][1] / vert[1][2]);
            blue_sx = r.start_z * (d1 / side_len * vert_color[2][2] / vert[2][2] + d2 / side_len * vert_color[1][2] / vert[1][2]);
        }
        else {
            float side_len = sqrt((vert[0][0] - vert[2][0]) * (vert[0][0] - vert[2][0]) + (vert[0][1] - vert[2][1]) * (vert[0][1] - vert[2][1]));
            float d0 = sqrt((vert[0][0] - sx) * (vert[0][0] - sx) + (vert[0][1] - y) * (vert[0][1] - y));
            float d2 = side_len - d0;
            red_sx = r.start_z * (d0 / side_len * vert_color[2][0] / vert[2][2] + d2 / side_len * vert_color[0][0] / vert[0][2]);
            green_sx = r.start_z * (d0 / side_len * vert_color[2][1] / vert[2][2] + d2 / side_len * vert_color[0][1] / vert[0][2]);
            blue_sx = r.start_z * (d0 / side_len * vert_color[2][2] / vert[2][2] + d2 / side_len * vert_color[0][2] / vert[0][2]);
        }
        index = smallest(std::abs((ex * m0.m + b0) - y), std::abs((ex * m1.m + b1) - y), std::abs((ex * m2.m + b2) - y), m0.isFlat, m1.isFlat, m2.isFlat);
        if (vertAtEnd && ex == vertical) {
            if (m0.isVert) {
                float side_len = std::abs(vert[0][1] - vert[1][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d1 = side_len - d0;
                red_ex = r.end_z * (d0 / side_len * vert_color[1][0] / vert[1][2] + d1 / side_len * vert_color[0][0] / vert[0][2]);
                green_ex = r.end_z * (d0 / side_len * vert_color[1][1] / vert[1][2] + d1 / side_len * vert_color[0][1] / vert[0][2]);
                blue_ex = r.end_z * (d0 / side_len * vert_color[1][2] / vert[1][2] + d1 / side_len * vert_color[0][2] / vert[0][2]);
            }
            else if (m1.isVert) {
                float side_len = std::abs(vert[1][1] - vert[2][1]);
                float d1 = std::abs(vert[1][1] - y);
                float d2 = side_len - d1;
                red_ex = r.end_z * (d1 / side_len * vert_color[2][0] / vert[2][2] + d2 / side_len * vert_color[1][0] / vert[1][2]);
                green_ex = r.end_z * (d1 / side_len * vert_color[2][1] / vert[2][2] + d2 / side_len * vert_color[1][1] / vert[1][2]);
                blue_ex = r.end_z * (d1 / side_len * vert_color[2][2] / vert[2][2] + d2 / side_len * vert_color[1][2] / vert[1][2]);
            }
            else {
                float side_len = std::abs(vert[2][1] - vert[0][1]);
                float d0 = std::abs(vert[0][1] - y);
                float d2 = side_len - d0;
                red_ex = r.end_z * (d0 / side_len * vert_color[2][0] / vert[2][2] + d2 / side_len * vert_color[0][0] / vert[0][2]);
                green_ex = r.end_z * (d0 / side_len * vert_color[2][1] / vert[2][2] + d2 / side_len * vert_color[0][1] / vert[0][2]);
                blue_ex = r.end_z * (d0 / side_len * vert_color[2][2] / vert[2][2] + d2 / side_len * vert_color[0][2] / vert[0][2]);
            }
        }
        else if (index == 0) {
            float side_len = sqrt((vert[0][0] - vert[1][0]) * (vert[0][0] - vert[1][0]) + (vert[0][1] - vert[1][1]) * (vert[0][1] - vert[1][1]));
            float d0 = sqrt((vert[0][0] - ex) * (vert[0][0] - ex) + (vert[0][1] - y) * (vert[0][1] - y));
            float d1 = side_len - d0;
            red_ex = r.end_z * (d0 / side_len * vert_color[1][0] / vert[1][2] + d1 / side_len * vert_color[0][0] / vert[0][2]);
            green_ex = r.end_z * (d0 / side_len * vert_color[1][1] / vert[1][2] + d1 / side_len * vert_color[0][1] / vert[0][2]);
            blue_ex = r.end_z * (d0 / side_len * vert_color[1][2] / vert[1][2] + d1 / side_len * vert_color[0][2] / vert[0][2]);
        }
        else if (index == 1) {
            float side_len = sqrt((vert[1][0] - vert[2][0]) * (vert[1][0] - vert[2][0]) + (vert[1][1] - vert[2][1]) * (vert[1][1] - vert[2][1]));
            float d1 = sqrt((vert[1][0] - ex) * (vert[1][0] - ex) + (vert[1][1] - y) * (vert[1][1] - y));
            float d2 = side_len - d1;
            red_ex = r.end_z * (d1 / side_len * vert_color[2][0] / vert[2][2] + d2 / side_len * vert_color[1][0] / vert[1][2]);
            green_ex = r.end_z * (d1 / side_len * vert_color[2][1] / vert[2][2] + d2 / side_len * vert_color[1][1] / vert[1][2]);
            blue_ex = r.end_z * (d1 / side_len * vert_color[2][2] / vert[2][2] + d2 / side_len * vert_color[1][2] / vert[1][2]);
        }
        else {
            float side_len = sqrt((vert[0][0] - vert[2][0]) * (vert[0][0] - vert[2][0]) + (vert[0][1] - vert[2][1]) * (vert[0][1] - vert[2][1]));
            float d0 = sqrt((vert[0][0] - ex) * (vert[0][0] - ex) + (vert[0][1] - y) * (vert[0][1] - y));
            float d2 = side_len - d0;
            red_ex = r.end_z * (d0 / side_len * vert_color[2][0] / vert[2][2] + d2 / side_len * vert_color[0][0] / vert[0][2]);
            green_ex = r.end_z * (d0 / side_len * vert_color[2][1] / vert[2][2] + d2 / side_len * vert_color[0][1] / vert[0][2]);
            blue_ex = r.end_z * (d0 / side_len * vert_color[2][2] / vert[2][2] + d2 / side_len * vert_color[0][2] / vert[0][2]);
        }
        rc.red_sx = red_sx;
        rc.green_sx = green_sx;
        rc.blue_sx = blue_sx;
        rc.red_ex = red_ex;
        rc.green_ex = green_ex;
        rc.blue_ex = blue_ex;
        rc.y = y;
    }
    if (std::abs(ex - sx) < .0000001) {
        int py = roundf(y + .5);
        if (vert[0][0] == ex && vert[0][1] == py) {
            int red = roundf(vert_color[0][0]);
            int green = roundf(vert_color[0][1]);
            int blue = roundf(vert_color[0][2]);
            return *(new vec4(red, green, blue, 0));
        }
        else if (vert[1][0] == ex && vert[1][1] == py) {
            int red = roundf(vert_color[1][0]);
            int green = roundf(vert_color[1][1]);
            int blue = roundf(vert_color[1][2]);
            return *(new vec4(red, green, blue, 0));
        }
        else {
            int red = roundf(vert_color[2][0]);
            int green = roundf(vert_color[2][1]);
            int blue = roundf(vert_color[2][2]);
            return *(new vec4(red, green, blue, 0));
        }
    }
    int redf = roundf(recent_z * (((float) ex-mx) / ((float) ex-sx) * red_sx / r.start_z + ((float) mx - sx) / ((float) ex-sx) * red_ex / r.end_z));
    int greenf = roundf(recent_z * (((float) ex-mx) / ((float) ex-sx) * green_sx / r.start_z + ((float) mx - sx) / ((float) ex-sx) * green_ex / r.end_z));
    int bluef = roundf(recent_z * (((float) ex-mx) / ((float) ex-sx) * blue_sx / r.start_z + ((float) mx - sx) / ((float) ex-sx) * blue_ex / r.end_z));
    return *(new vec4(redf, greenf, bluef, 0));
}

std::ostream &operator<<(std::ostream &o, const triangle &t) {
    o  << "vertex_1: "<< t[0] << std::endl << "vertex_2: "<< t[1] << std::endl << "vertex_3: "<< t[2] << std::endl << "norm_1: " << t.norm[0] << std::endl << "norm_2: " << t.norm[1] << std::endl << "norm_3: " << t.norm[2] << std::endl << "Box: " << t.getBoundingBox() << std::endl << "Diffuse: " << t.diffuse << std::endl;
    return o;
}
