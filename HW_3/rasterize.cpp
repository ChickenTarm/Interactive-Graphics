#include <iostream>
#include <assert.h>
#include "tiny_obj_loader.h"
#include "mat4.h"
#include "triangle.h"
#include "vec4.h"
#include "camera.h"
#include <fstream>
#include <string>
#include <cmath>
#include <stdlib.h>

#define CLAMP(x) (x> 255 ? 255: ((x) < 0 ? 0 : (x)))
#if 1
#define HERE printf("Arrived here: %s: %d\n", __FILE__, __LINE__)
#else
#define HERE
#endif

using namespace std;

struct pixel_struct {
  unsigned char r, g, b;
};

typedef struct pixel_struct pixel_t;

typedef struct img_struct {
  pixel_t *data;
  int w, h;
} img_t;

typedef struct z_buffer {
    float *data;
    int w, h;
} z_buf;

std::vector<triangle> toNDC(camera &c, std::vector<triangle> triangles) {
    std::vector<triangle> temp;
    for (int k = 0; k < ((int) triangles.size()); k = k + 1) {
        triangle &tri = triangles[k];
        tri[0] = c.transform(tri[0]);
        tri[0] = tri[0] / tri[0][3];
        tri[1] = c.transform(tri[1]);
        tri[1] = tri[1] / tri[1][3];
        tri[2] = c.transform(tri[2]);
        tri[2] = tri[2] / tri[2][3];
        temp.push_back(tri);
    }
    return temp;
}

std::vector<triangle> toScreen(float width, float height, std::vector<triangle> triangles) {
    std::vector<triangle> temp;
    for (int k = 0; k < ((int) triangles.size()); k = k + 1) {
        triangle &tri = triangles[k];
        tri[0][0] = roundf((tri[0][0] + 1) * width / 2);
        tri[0][1] = roundf((1 - tri[0][1]) * height / 2);
        tri[1][0] = roundf((tri[1][0] + 1) * width / 2);
        tri[1][1] = roundf((1 - tri[1][1]) * height / 2);
        tri[2][0] = roundf((tri[2][0] + 1) * width / 2);
        tri[2][1] = roundf((1 - tri[2][1]) * height / 2);
        temp.push_back(tri);
    }
    return temp;
}

void boundBox(triangle &t, int width, int height) {
    int minx = min(min(t[0][0], t[1][0]), t[2][0]);
    minx = max(0, minx);
    int maxx = max(max(t[0][0], t[1][0]), t[2][0]);
    maxx = min(width, maxx);
    int miny = min(min(t[0][1], t[1][1]), t[2][1]);
    miny = max(0, miny);
    int maxy = max(max(t[0][1], t[1][1]), t[2][1]);
    maxy = min(height, maxy);
    t.setBounds(minx, maxx, miny, maxy);
}

std::vector<triangle> visibility(camera &c, std::vector<triangle> triangles, int width, int height) {
    std::vector<triangle> temp;
    for (int k = 0; k < ((int) triangles.size()); k = k + 1) {
        float z1 = triangles[k][0][2];
        float z2 = triangles[k][1][2];
        float z3 = triangles[k][2][2];
        if ((z1 < 0 || z1 > 1) && (z2 < 0 || z2 > 1) && (z3 < 0 || z3 > 1)) {
            continue;
        }
        else {
            float x1 = triangles[k][0][0];
            float x2 = triangles[k][1][0];
            float x3 = triangles[k][2][0];
            float y1 = triangles[k][0][1];
            float y2 = triangles[k][1][1];
            float y3 = triangles[k][2][1];
            bool out_x = (x1 < 0 || x1 > width) && (x2 < 0 || x2 > width) && (x3 < 0 || x3 > width);
            bool out_y = (y1 < 0 || y1 > height) && (y2 < 0 || y2 > height) && (y3 < 0 || y3 > height);
            if (out_x || out_y) {
                continue;
            }
            else {
                boundBox(triangles[k], width, height);
                temp.push_back(triangles[k]);
            }
        }
    }
    return temp;
}

int main(int argc, char *argv[])
{
    assert(argv[1] != NULL);
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::vector<triangle> triangles;
    camera cam(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    float width;
    float height;
    img_t *pic = (img_t *) malloc(sizeof(img_t));
    for (int i = 1; i < argc; i = i + 1) {
        std::string str(argv[i]);
        errno = 0;
        char *end;
        strtof(argv[i], &end);
        if (str.find(".obj") != std::string::npos) {
            LoadObj(shapes, materials, argv[1], NULL);
//            cout << "LoadObj: "<< LoadObj(shapes, materials, argv[1], NULL) <<  endl;
            for (int j = 0; j < ((int) shapes.size()); j = j + 1) {
                for (int i = 0; i < ((int) shapes[j].mesh.indices.size()); i = i + 3) {
                    int index1 = shapes[j].mesh.indices[i];
                    int index2 = shapes[j].mesh.indices[i + 1];
                    int index3 = shapes[j].mesh.indices[i + 2];
                    vec4 vertex1(shapes[j].mesh.positions[index1 * 3], shapes[j].mesh.positions[index1 * 3 + 1], shapes[j].mesh.positions[index1 * 3 + 2], 1);
                    vec4 vertex2(shapes[j].mesh.positions[index2 * 3], shapes[j].mesh.positions[index2 * 3 + 1], shapes[j].mesh.positions[index2 * 3 + 2], 1);
                    vec4 vertex3(shapes[j].mesh.positions[index3 * 3], shapes[j].mesh.positions[index3 * 3 + 1], shapes[j].mesh.positions[index3 * 3 + 2], 1);
                    triangle t(vertex1, vertex2, vertex3);
                    triangles.push_back(t);
                }
            }
//            cout << "Num Positions: " << shapes[0].mesh.positions.size() << endl;
//            cout << "Num Indices: " << shapes[0].mesh.indices.size() << endl;
//            cout << "Num Triangles: " << triangles.size() << endl;
            cout << triangles[0] << endl;
            cout << triangles[1] << endl;
        }
        else if (!strcmp(argv[i], "camera.txt")) {
            std::vector<float> param;
            std::string num;
            std::ifstream camFile("camera.txt");
            while (camFile >> num) {
                param.push_back(stof(num));
            }
//            for (int j = 0; j < param.size(); j = j + 1) {
//                cout << j << ": " << param[j] << endl;
//            }
            camera c(param[0], param[1], param[2], param[3], param[4], param[5], param[6], param[7], param[8], param[9], param[10], param[11], param[12], param[13], param[14]);
            triangles = toNDC(c, triangles);
            cam = c;
        }
        else if (errno == 0) {
//            printf("width: %s/n height: %s\n", argv[i], argv[i + 1]);
            width = strtof(argv[i], &end);
            i = i + 1;
            height = strtof(argv[i], &end);
            pic->w = width;
            pic->h = height;
            pic->data = (pixel_t *) calloc(width * height, sizeof(pixel_t));
            triangles = toScreen(pic->w, pic->h, triangles);
            triangles = visibility(cam, triangles, pic->w - 1, pic->h - 1);
            cout << triangles[0] << endl;
            cout << triangles[1] << endl;
            for (int k = 0; k < ((int) triangles.size()); k = k + 1) {
                printf("triangle %d:\n", k);
                triangles[k].getPixels();
            }
        }
        else if (str.find(".ppm") != std::string::npos) {

        }
    }
    return 0;
}
