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

struct z_buffer {
    float *data;
    int w, h;
};

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

std::vector<triangle> visibility(std::vector<triangle> triangles, int width, int height) {
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
    z_buffer *z_buf = (z_buffer *) malloc(sizeof(z_buffer));
    int z_method = 0;
    int color_method = 0;
    bool gotDimension = false;
    bool readCam = false;
    FILE *output;
    for (int i = 1; i < argc; i = i + 1) {
        std::string str(argv[i]);
        errno = 0;
        char *end;
        strtof(argv[i], &end);
        if (str.find(".obj") != std::string::npos) {
          //Parses OBJ
            LoadObj(shapes, materials, argv[1], NULL);
            for (int j = 0; j < ((int) shapes.size()); j = j + 1) {
                for (int i = 0; i < ((int) shapes[j].mesh.indices.size()); i = i + 3) {
                  //Creates the triangles and stores them
                    int index1 = shapes[j].mesh.indices[i] * 3;
                    int index2 = shapes[j].mesh.indices[i + 1] * 3;
                    int index3 = shapes[j].mesh.indices[i + 2] * 3;
                    vec4 vertex1(shapes[j].mesh.positions[index1], shapes[j].mesh.positions[index1 + 1], shapes[j].mesh.positions[index1 + 2], 1);
                    vec4 vertex2(shapes[j].mesh.positions[index2], shapes[j].mesh.positions[index2 + 1], shapes[j].mesh.positions[index2 + 2], 1);
                    vec4 vertex3(shapes[j].mesh.positions[index3], shapes[j].mesh.positions[index3 + 1], shapes[j].mesh.positions[index3 + 2], 1);
                    vec4 n1(shapes[j].mesh.normals[index1], shapes[j].mesh.normals[index1 + 1], shapes[j].mesh.normals[index1 + 2], 0);
                    vec4 n2(shapes[j].mesh.normals[index2], shapes[j].mesh.normals[index2 + 1], shapes[j].mesh.normals[index2 + 2], 0);
                    vec4 n3(shapes[j].mesh.normals[index3], shapes[j].mesh.normals[index3 + 1], shapes[j].mesh.normals[index3 + 2], 0);
                    int mat_id = shapes[j].mesh.material_ids[i / 3];
                    vec4 diffuse(materials[mat_id].diffuse[0], materials[mat_id].diffuse[1], materials[mat_id].diffuse[2], 0);
                    triangle t(vertex1, vertex2, vertex3, n1, n2, n3, diffuse);
                    triangles.push_back(t);
                }
            }
        }
        else if (str.find("camera") != std::string::npos && !readCam) {
            std::vector<float> param;
            std::string num;
            std::ifstream camFile(argv[i]);
            while (camFile >> num) {
                param.push_back(stof(num));
            }
            camera c(param[0], param[1], param[2], param[3], param[4], param[5], param[6], param[7], param[8], param[9], param[10], param[11], param[12], param[13], param[14]);
            triangles = toNDC(c, triangles);
            for (int k = 0; k < ((int) triangles.size()); k = k + 1) {
                triangles[k].rotateNorms(c);
            }
            cam = c;
            readCam = true;
        }
        else if (errno == 0 && !gotDimension) {
            width = strtof(argv[i], &end);
            i = i + 1;
            height = strtof(argv[i], &end);
            gotDimension = true;
            pic->w = width;
            pic->h = height;
            pic->data = (pixel_t *) calloc(width * height, sizeof(pixel_t));
            z_buf->data = (float *) malloc(width * height * sizeof(float));
            z_buf->w = width;
            z_buf->h = height;
            for (int z = 0; z < z_buf->w * z_buf->h; z = z + 1) {
                z_buf->data[z] = 2;
            }
            triangles = toScreen(pic->w, pic->h, triangles);
            triangles = visibility(triangles, pic->w - 1, pic->h - 1);
            for (int k = 0; k < ((int) triangles.size()); k = k + 1) {
                triangles[k].getPixels();
            }
        }
        else if (str.find(".ppm") != std::string::npos) {
            assert(argv[i] != NULL);
            output = fopen(argv[i], "wb+");
            assert(output != NULL);
            fprintf(output, "P6\n%d %d 255\n", pic->w, pic->h);
        }
        else if (!strcmp(argv[i], "--norm_gouraud_z")) {
            z_method = 1;
            color_method = 5;
        }
        else if (!strcmp(argv[i], "--norm_bary_z")) {
            z_method = 2;
            color_method = 6;
        }
        else if (!strcmp(argv[i], "--norm_gouraud")) {
            color_method = 3;
        }
        else if (!strcmp(argv[i], "--norm_bary")) {
            color_method = 4;
        }
        else if (!strcmp(argv[i], "--norm_flat")) {
            color_method = 2;
        }
        else if (!strcmp(argv[i], "--white")) {
            color_method = 1;
        }
        else {
            break;
        }
    }
    for (int k = 0; k < ((int) triangles.size()); k = k + 1) {
        int range = triangles[k].max_y - triangles[k].min_y;
      // Iterate over the y height
        for (int r = 0; r < range; r = r + 1) {
            int sx = triangles[k].getStartx(r);
            int ex = triangles[k].getEndx(r);
            float y = triangles[k].getY(r);
            int py = y - .5;
          // Iterate over the x range
            for (int p = sx; p <= ex; p = p + 1) {
                int index = p + py * pic->w;
                float z_norm = triangles[k].getZ(sx, ex, y, p, z_method);
                if (z_norm >= 0 && z_norm <= 1) {
                    float zb = z_buf->data[index];
                    if (z_norm < zb) {
                        z_buf->data[index] = z_norm;
                        //color pixel
                        vec4 color = triangles[k].getColor(sx, ex, y, p, color_method);
                        pic->data[index].r = color[0];
                        pic->data[index].g = color[1];
                        pic->data[index].b = color[2];
                    }
                }
            }
        }
        free(triangles[k].pix);
    }
    for (int t = 0; t < pic->w * pic->h; t = t + 1) {
        fwrite(&(pic->data[t].r), sizeof(pic->data[t].r), 1, output);
        fwrite(&(pic->data[t].g), sizeof(pic->data[t].g), 1, output);
        fwrite(&(pic->data[t].b), sizeof(pic->data[t].b), 1, output);
    }
    free(pic->data);
    free(pic);
    free(z_buf->data);
    free(z_buf);
    return 0;
}
