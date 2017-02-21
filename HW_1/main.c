#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#define CLAMP(x) (x> 255 ? 255: ((x) < 0 ? 0 : (x)))

#if 0
#define HERE fprintf(stdeer, "Arrived here: %s: %d", __FILE__, __LINE__)
#else
#define HERE
#endif


struct pixel_struct {
  unsigned char r, g, b; // a pixel contains three bytes, name r, g, and b
};  // don't forget the ; at the end of the struct declaration!

/* To declare a variable of type "pixel" in C, you have to write
 *   struct pixel_struct pixel;
 * which is clunky.  It's handy to "rename" this type to get around having
 * to write "struct" all over the place.  Do this with a typedef:
 */
typedef struct pixel_struct pixel_t; // from now on "pixel_t p" is a synonym for "struct pixel_struct p"

/* You can also combine the struct declaration with a typedef:
 */
typedef struct img_struct {
  pixel_t *data; // img is a pointer to a block of memory containing pixels, i.e. an array of pixels
  // see the discussion of argv below for an explanation of pointers
  int w, h; // image width and height
} img_t;

typedef struct pixel_double {
  double r, g, b; // a pixel contains three bytes, name r, g, and b
} dub_p;

typedef struct between {
  dub_p *data; // img is a pointer to a block of memory containing pixels, i.e. an array of pixels
  // see the discussion of argv below for an explanation of pointers
  int w, h; // image width and height
} dub_i;

unsigned char getMed(unsigned char *lst, int num) {
    for (int q = 0; q < num - 1; q = q + 1) {
        int min_index = q;
        for (int u = q + 1; u < num; u = u + 1) {
            if (lst[u] < lst[min_index]) {
                min_index = u;
            }
        }
        if (q != min_index) {
            unsigned char hold = lst[q];
            lst[q] = lst[min_index];
            lst[min_index] = hold;
        }
    }
    if (num % 2 == 0) {
        double combine = lst[num / 2] + lst[num / 2 + 1];
        return CLAMP(combine / 2.0);
    }
    else {
        return lst[num / 2];
    }
}

int main(int argc, char *argv[])
{
    assert(argv[1] != NULL); // crash if fname is NULL
    FILE *in = fopen(argv[1], "rb");
    assert(in != NULL); // crash if the file didn't open
    int w;
    int h;
    fscanf(in, "P6 %d %d 255%*c", &w, &h);
    img_t *pixies = malloc(sizeof(img_t));
    pixies->w = w;
    pixies->h = h;
    pixies->data = calloc(w * h, sizeof(pixel_t));
    fread(pixies->data, 3, w * h, in);

    assert(argv[argc - 1] != NULL); // crash if fname is NULL
    FILE *out = fopen(argv[argc - 1], "wb+");
    assert(out != NULL); // crash if file did not open

    for (int i = 2; i < argc - 1; i = i + 1) {
        if (!strcmp(argv[i], "-grayscale")) {
            img_t *alt = malloc(sizeof(img_t));
            alt->w = w;
            alt->h = h;
            alt->data = calloc(w * h, sizeof(pixel_t));
            for (int j = 0; j < w * h; j = j + 1) {
                unsigned char y = (unsigned char) CLAMP(.299 * pixies->data[j].r + .587 * pixies->data[j].g + .114 * pixies->data[j].b);
                alt->data[j].r = y;
                alt->data[j].g = y;
                alt->data[j].b = y;
            }
            img_t *temp = pixies;
            temp->data = pixies->data;
            pixies = alt;
            pixies->data = alt->data;
            free(temp->data);
            free(temp);
        }
        if (!strcmp(argv[i], "-flop")) {
            img_t *alt = malloc(sizeof(img_t));
            alt->w = w;
            alt->h = h;
            alt->data = calloc(w * h, sizeof(pixel_t));
            int row;
            int col;
            int flip;
            for (int j = 0; j < w * h; j = j + 1) {
                row  = j / w;
                col = j % w;
                flip = w * (h - 1 - row) + col;
                alt->data[flip] = pixies->data[j];
            }
            img_t *temp = pixies;
            temp->data = pixies->data;
            pixies = alt;
            pixies->data = alt->data;
            free(temp->data);
            free(temp);
        }
        if (!strcmp(argv[i], "-flip")) {
            img_t *alt = malloc(sizeof(img_t));
            alt->w = w;
            alt->h = h;
            alt->data = calloc(w * h, sizeof(pixel_t));
            int row;
            int col;
            int flip;
            for (int j = 0; j < w * h; j = j + 1) {
                row  = j / w;
                col = j % w;
                flip = (w - 1 - col) + w * row;
                alt->data[flip] = pixies->data[j];
            }
            img_t *temp = pixies;
            temp->data = pixies->data;
            pixies = alt;
            pixies->data = alt->data;
            free(temp->data);
            free(temp);
        }
        if (!strcmp(argv[i], "-transpose")) {
            img_t *alt = malloc(sizeof(img_t));
            alt->w = h;
            alt->h = w;
            alt->data = calloc(w * h, sizeof(pixel_t));
            int row;
            int col;
            int flip;
            for (int j = 0; j < w * h; j = j + 1) {
                row  = j / w;
                col = j % w;
                flip = h * col + row;
                alt->data[flip] = pixies->data[j];
            }
            h = alt->w;
            w = alt->h;
            img_t *temp = pixies;
            temp->data = pixies->data;
            pixies = alt;
            pixies->data = alt->data;
            free(temp->data);
            free(temp);
        }
        if (!strcmp(argv[i], "-boxblur")) {
             i = i + 1;
             int rad = atoi(argv[i]);
             img_t *alt = malloc(sizeof(img_t));
             alt->w = w;
             alt->h = h;
             alt->data = calloc(w * h, sizeof(pixel_t));
             int row;
             int col;
             for (int j = 0; j < w * h; j = j + 1) {
                 row  = j / w;
                 col = j % w;
                 if (row < rad || (h - 1) - row < rad || col < rad || (w - 1) - col < rad) {
                     alt->data[j].b = 0;
                     alt->data[j].r = 0;
                     alt->data[j].g = 255;
                 }
                 else {
                     int all_red = 0;
                     int all_green = 0;
                     int all_blue = 0;
                     for (int r = row - rad; r < row + rad + 1; r = r + 1) {
                         for (int c = col - rad; c < col + rad + 1; c = c + 1) {
                             int index = w * r + c;
                             all_red = all_red + pixies->data[index].r;
                             all_green = all_green + pixies->data[index].g;
                             all_blue = all_blue + pixies->data[index].b;
                         }
                     }
                     float num = (2 * rad + 1) * (2 * rad + 1);
                     unsigned char red = (unsigned char) CLAMP(all_red / num);
                     unsigned char green = (unsigned char) CLAMP(all_green / num);
                     unsigned char blue = (unsigned char) CLAMP(all_blue / num);
                     alt->data[j].b = blue;
                     alt->data[j].r = red;
                     alt->data[j].g = green;
                 }
             }
             img_t *temp = pixies;
             temp->data = pixies->data;
             pixies = alt;
             pixies->data = alt->data;
             free(temp->data);
             free(temp);
        }
        if (!strcmp(argv[i], "-median")) {
            i = i + 1;
            int rad = atoi(argv[i]);
            img_t *alt = malloc(sizeof(img_t));
            alt->w = w;
            alt->h = h;
            alt->data = calloc(w * h, sizeof(pixel_t));
            int row;
            int col;
            int area = (2 * rad + 1) * (2 * rad + 1);
            unsigned char reds[area];
            unsigned char greens[area];
            unsigned char blues[area];
            for (int j = 0; j < w * h; j = j + 1) {
                row  = j / w;
                col = j % w;
                if (row < rad || (h - 1) - row < rad || col < rad || (w - 1) - col < rad) {
                    alt->data[j].b = 0;
                    alt->data[j].r = 0;
                    alt->data[j].g = 255;
                }
                else {
                    int count = 0;
                    for (int r = row - rad; r < row + rad + 1; r = r + 1) {
                        for (int c = col - rad; c < col + rad + 1; c = c + 1) {
                            int index = w * r + c;
                            reds[count] =  pixies->data[index].r;
                            greens[count] = pixies->data[index].g;
                            blues[count] = pixies->data[index].b;
                            count = count + 1;
                        }
                    }
                    alt->data[j].b = getMed(blues, area);
                    alt->data[j].r = getMed(reds, area);
                    alt->data[j].g = getMed(greens, area);
                }
            }
            img_t *temp = pixies;
            temp->data = pixies->data;
            pixies = alt;
            pixies->data = alt->data;
            free(temp->data);
            free(temp);
        }
        if (!strcmp(argv[i], "-gaussian")) {
            i = i + 1;
            int rad = atoi(argv[i]);
            i = i + 1;
            double s = atof(argv[i]);
            img_t *alt = malloc(sizeof(img_t));
            alt->w = w;
            alt->h = h;
            alt->data = calloc(w * h, sizeof(pixel_t));
            dub_i *mid = malloc(sizeof(dub_i));
            mid->w = w;
            mid->h = h;
            mid->data = calloc(w * h, sizeof(dub_p));
            int row;
            int col;
            for (int j = 0; j < w * h; j = j + 1) {
                row  = j / w;
                col = j % w;
                if (col < rad || (w - 1) - col < rad) {
                    alt->data[j].b = 0;
                    alt->data[j].g = 255;
                    alt->data[j].r = 0;
                }
                else {
                    double all_red = 0;
                    double all_green = 0;
                    double all_blue = 0;
                    double num = 0;
                    for (int p = col - rad; p < col + rad + 1; p = p + 1) {
                        int index = row * w + p;
                        double x = p - col;
                        double weight = exp(-1 * x * x / (2 * s * s));
                        num = num + weight;
                        all_red = all_red + pixies->data[index].r * weight;
                        all_green = all_green + pixies->data[index].g * weight;
                        all_blue = all_blue + pixies->data[index].b * weight;
                    }
                    double red = all_red / num;
                    double green = all_green / num;
                    double blue = all_blue / num;
                    mid->data[j].b = blue;
                    mid->data[j].r = red;
                    mid->data[j].g = green;
                }
            }
            for (int j = 0; j < w * h; j = j + 1) {
                row  = j / w;
                col = j % w;
                if (row < rad || (h - 1) - row < rad || col < rad || (w - 1) - col < rad) {
                    alt->data[j].b = 0;
                    alt->data[j].g = 255;
                    alt->data[j].r = 0;
                }
                else {
                    double all_red = 0;
                    double all_green = 0;
                    double all_blue = 0;
                    double num = 0;
                    for (int p = row - rad; p < row + rad + 1; p = p + 1) {
                        int index = p * w + col;
                        double x = p - row;
                        double weight = exp(-1 * x * x / (2 * s * s));
                        num = num + weight;
                        all_red = all_red + mid->data[index].r * weight;
                        all_green = all_green + mid->data[index].g * weight;
                        all_blue = all_blue + mid->data[index].b * weight;
                    }
                    unsigned char red = (unsigned char) CLAMP(all_red / num);
                    unsigned char green = (unsigned char) CLAMP(all_green / num);
                    unsigned char blue = (unsigned char) CLAMP(all_blue / num);
                    alt->data[j].b = blue;
                    alt->data[j].r = red;
                    alt->data[j].g = green;
                }
            }
            free(mid->data);
            free(mid);
            img_t *temp = pixies;
            temp->data = pixies->data;
            pixies = alt;
            pixies->data = alt->data;
            free(temp->data);
            free(temp);
        }
        if (!strcmp(argv[i], "-sobel")) {
            int gx[9];
            gx[0] = gx[6]= -1;
            gx[1] = gx[4] = gx[7] = 0;
            gx[2] = gx[8] = 1;
            gx[3] = -2;
            gx[5] = 2;
            int gy[9];
            gy[0] = gy[2] = 1;
            gy[3] = gy[4] = gy[5] = 0;
            gy[6] = gy[8] = -1;
            gy[1] = 2;
            gy[7] = -2;
            img_t *alt = malloc(sizeof(img_t));
            alt->w = w;
            alt->h = h;
            alt->data = calloc(w * h, sizeof(pixel_t));
            img_t *gray = malloc(sizeof(img_t));
            gray->h = h;
            gray->w = w;
            gray->data = calloc(w * h, sizeof(pixel_t));
            for (int j = 0; j < w * h; j = j + 1) {
                unsigned char y = (unsigned char) CLAMP(.299 * pixies->data[j].r + .587 * pixies->data[j].g + .114 * pixies->data[j].b);
                gray->data[j].r = y;
                gray->data[j].g = y;
                gray->data[j].b = y;
            }
            int row;
            int col;
            for (int j = 0; j < w * h; j = j + 1) {
                row  = j / w;
                col = j % w;
                if (row < 1 || (h - 1) - row < 1 || col < 1 || (w - 1) - col < 1) {
                    alt->data[j].b = 0;
                    alt->data[j].r = 0;
                    alt->data[j].g = 255;
                }
                else {
                    int x_grad = 0;
                    int y_grad = 0;
                    int count = 0;
                    for (int r = row - 1; r < row + 2; r = r + 1) {
                        for (int c = col - 1; c < col + 2; c = c + 1) {
                            int index = w * r + c;
                            x_grad = x_grad + gx[count] * gray->data[index].r;
                            y_grad = y_grad + gy[count] * gray->data[index].r;
                            count = count + 1;
                        }
                    }
                    unsigned char g = (unsigned char) CLAMP(sqrt((double) (x_grad * x_grad + y_grad * y_grad)));
                    alt->data[j].b = alt->data[j].r = alt->data[j].g = g;
                }
            }
            free(gray->data);
            free(gray);
            img_t *temp = pixies;
            temp->data = pixies->data;
            pixies = alt;
            pixies->data = alt->data;
            free(temp->data);
            free(temp);
        }
        if (!strcmp(argv[i], "-rotate")) {
            i = i + 1;
            double cc_angle = atof(argv[i]);
            double to_rad = M_PI / 180.0;
            cc_angle = cc_angle * to_rad;
            double cw_angle = -1 * cc_angle;
            double pos_x = 0;
            double neg_x = 0;
            double pos_y = 0;
            double neg_y = 0;
            int mid_x = (w - 1) / 2;
            int mid_y = (h - 1) / 2;
            for (int j = 0; j < w * h; j = j + 1) {
                int y  = mid_y - j / w;
                int x = j % w - mid_x;
                double nx = x * cos(cc_angle) - y * sin(cc_angle);
                double ny = x * sin(cc_angle) + y * cos(cc_angle);
                if (nx > 0) {
                    if (pos_x < nx) {
                        pos_x = nx;
                    }
                }
                else {
                    if (neg_x > nx) {
                        neg_x = nx;
                    }
                }
                if (ny > 0) {
                    if (pos_y < ny) {
                        pos_y = ny;
                    }
                }
                else {
                    if (neg_y > ny) {
                        neg_y = ny;
                    }
                }
            }
            int nw = round(pos_x - neg_x);
            int nh = round(pos_y - neg_y);
            img_t *alt = malloc(sizeof(img_t));
            alt->w = nw;
            alt->h = nh;
            alt->data = calloc(nw * nh, sizeof(pixel_t));
            int nmid_x = nw / 2;
            int nmid_y = nh / 2;
            for (int j = 0; j < nw * nh; j = j + 1) {
                int x = j % nw - nmid_x;
                int y = nmid_y - j / nw;
                double nx = x * cos(cw_angle) - y * sin(cw_angle);
                double ny = x * sin(cw_angle) + y * cos(cw_angle);
                if (nx >= 0) {
                    if (round(nx) > mid_x) {
                        alt->data[j].r = alt->data[j].b = 0;
                        alt->data[j].g = 225;
                        continue;
                    }
                }
                else {
                    if (round(nx) < -1 * mid_x) {
                        alt->data[j].r = alt->data[j].b = 0;
                        alt->data[j].g = 225;
                        continue;
                    }
                }
                if (ny >= 0) {
                    if (round(ny) > mid_y) {
                        alt->data[j].r = alt->data[j].b = 0;
                        alt->data[j].g = 225;
                        continue;
                    }
                }
                else {
                    if (round(ny) < -1 * mid_y) {
                        alt->data[j].r = alt->data[j].b = 0;
                        alt->data[j].g = 225;
                        continue;
                    }
                }
                int col = round(nx) + mid_x;
                int row = mid_y - round(ny);
                double r_top = 0;
                double g_top = 0;
                double b_top = 0;
                if (nx >= 0 && ny >= 0) {
                    int col_bord;
                    if (w - 1 > col + 1) {
                        col_bord = col + 1;
                    }
                    else {
                        col_bord = w - 1;
                    }
                    int row_bord;
                    if (0 < row - 1) {
                        row_bord = row - 1;
                    }
                    else {
                        row_bord = 0;
                    }
                    if (row == row_bord && col == col_bord) {
                        alt->data[j] = pixies->data[row * w + col];
                        continue;
                    }
                    if (row == row_bord) {
                        int index = w * row + col;
                        r_top = pixies->data[index].r + (nx - (int) nx) / 2.0 * (pixies->data[index + 1].r - pixies->data[index].r);
                        g_top = pixies->data[index].g + (nx - (int) nx) / 2.0 * (pixies->data[index + 1].g - pixies->data[index].g);
                        b_top = pixies->data[index].b + (nx - (int) nx) / 2.0 * (pixies->data[index + 1].b - pixies->data[index].b);
                        alt->data[j].r = (unsigned char) CLAMP(r_top);
                        alt->data[j].g = (unsigned char) CLAMP(g_top);
                        alt->data[j].b = (unsigned char) CLAMP(b_top);
                        continue;
                    }
                    if (col == col_bord) {
                        int index = w * row + col;
                        int index2 = w * (row_bord) + col;
                        r_top = pixies->data[index].r + (ny - (int) ny) / 2.0 * (pixies->data[index2].r - pixies->data[index].r);
                        g_top = pixies->data[index].g + (ny - (int) ny) / 2.0 * (pixies->data[index2].g - pixies->data[index].g);
                        b_top = pixies->data[index].b + (ny - (int) ny) / 2.0 * (pixies->data[index2].b - pixies->data[index].b);
                        alt->data[j].r = (unsigned char) CLAMP(r_top);
                        alt->data[j].g = (unsigned char) CLAMP(g_top);
                        alt->data[j].b = (unsigned char) CLAMP(b_top);
                        continue;
                    }
                    double x_dist = fabs(round(nx) - nx);
                    double y_dist = fabs(round(ny) - ny);
                    r_top = (1 - x_dist) * (1 - y_dist) * pixies->data[w * row + col].r + x_dist * (1 - y_dist) * pixies->data[w * row + col + 1].r + y_dist * (1 - x_dist) * pixies->data[w * (row + 1) + col].r + x_dist * y_dist * pixies->data[w * (row + 1) + col + 1].r;
                    g_top = (1 - x_dist) * (1 - y_dist) * pixies->data[w * row + col].g + x_dist * (1 - y_dist) * pixies->data[w * row + col + 1].g + y_dist * (1 - x_dist) * pixies->data[w * (row + 1) + col].g + x_dist * y_dist * pixies->data[w * (row + 1) + col + 1].g;
                    b_top = (1 - x_dist) * (1 - y_dist) * pixies->data[w * row + col].b + x_dist * (1 - y_dist) * pixies->data[w * row + col + 1].b + y_dist * (1 - x_dist) * pixies->data[w * (row + 1) + col].b + x_dist * y_dist * pixies->data[w * (row + 1) + col + 1].b;
                    alt->data[j].r = (unsigned char) CLAMP(r_top);
                    alt->data[j].g = (unsigned char) CLAMP(g_top);
                    alt->data[j].b = (unsigned char) CLAMP(b_top);
                    continue;
                }
                if (nx < 0  && ny > 0) {
                    int col_bord;
                    if (0 < col - 1) {
                        col_bord = col - 1;
                    }
                    else {
                        col_bord = 0;
                    }
                    int row_bord;
                    if (0 < row - 1) {
                        row_bord = row - 1;
                    }
                    else {
                        row_bord = 0;
                    }
                    if (row == row_bord && col == col_bord) {
                        alt->data[j] = pixies->data[row * w + col];
                        continue;
                    }
                    if (row == row_bord) {
                        int index = w * row + col;
                        r_top = pixies->data[index + 1].r + (nx - (int) nx) / 2.0 * (pixies->data[index].r - pixies->data[index + 1].r);
                        g_top = pixies->data[index + 1].g + (nx - (int) nx) / 2.0 * (pixies->data[index].g - pixies->data[index + 1].g);
                        b_top = pixies->data[index + 1].b + (nx - (int) nx) / 2.0 * (pixies->data[index].b - pixies->data[index + 1].b);
                        alt->data[j].r = (unsigned char) CLAMP(r_top);
                        alt->data[j].g = (unsigned char) CLAMP(g_top);
                        alt->data[j].b = (unsigned char) CLAMP(b_top);
                        continue;
                    }
                    if (col == col_bord) {
                        int index = w * row + col;
                        int index2 = w * (row_bord) + col;
                        r_top = pixies->data[index].r + (ny - (int) ny) / 2.0 * (pixies->data[index2].r - pixies->data[index].r);
                        g_top = pixies->data[index].g + (ny - (int) ny) / 2.0 * (pixies->data[index2].g - pixies->data[index].g);
                        b_top = pixies->data[index].b + (ny - (int) ny) / 2.0 * (pixies->data[index2].b - pixies->data[index].b);
                        alt->data[j].r = (unsigned char) CLAMP(r_top);
                        alt->data[j].g = (unsigned char) CLAMP(g_top);
                        alt->data[j].b = (unsigned char) CLAMP(b_top);
                        continue;
                    }
                    double x_dist = fabs(round(nx) - nx);
                    double y_dist = fabs(round(ny) - ny);
                    r_top = (1 - x_dist) * (1 - y_dist) * pixies->data[w * row + col].r + x_dist * (1 - y_dist) * pixies->data[w * row + col + 1].r + y_dist * (1 - x_dist) * pixies->data[w * (row + 1) + col].r + x_dist * y_dist * pixies->data[w * (row + 1) + col + 1].r;
                    g_top = (1 - x_dist) * (1 - y_dist) * pixies->data[w * row + col].g + x_dist * (1 - y_dist) * pixies->data[w * row + col + 1].g + y_dist * (1 - x_dist) * pixies->data[w * (row + 1) + col].g + x_dist * y_dist * pixies->data[w * (row + 1) + col + 1].g;
                    b_top = (1 - x_dist) * (1 - y_dist) * pixies->data[w * row + col].b + x_dist * (1 - y_dist) * pixies->data[w * row + col + 1].b + y_dist * (1 - x_dist) * pixies->data[w * (row + 1) + col].b + x_dist * y_dist * pixies->data[w * (row + 1) + col + 1].b;
                    alt->data[j].r = (unsigned char) CLAMP(r_top);
                    alt->data[j].g = (unsigned char) CLAMP(g_top);
                    alt->data[j].b = (unsigned char) CLAMP(b_top);
                    continue;
                }
                if (nx < 0  && ny < 0) {
                    int col_bord;
                    if (0 < col - 1) {
                        col_bord = col - 1;
                    }
                    else {
                        col_bord = 0;
                    }
                    int row_bord;
                    if (h - 1 > row + 1) {
                        row_bord = row + 1;
                    }
                    else {
                        row_bord = h - 1;
                    }
                    if (row == row_bord && col == col_bord) {
                        alt->data[j] = pixies->data[row * w + col];
                        continue;
                    }
                    if (row == row_bord) {
                        int index = w * row + col;
                        r_top = pixies->data[index + 1].r + (nx - (int) nx) / 2.0 * (pixies->data[index].r - pixies->data[index + 1].r);
                        g_top = pixies->data[index + 1].g + (nx - (int) nx) / 2.0 * (pixies->data[index].g - pixies->data[index + 1].g);
                        b_top = pixies->data[index + 1].b + (nx - (int) nx) / 2.0 * (pixies->data[index].b - pixies->data[index + 1].b);
                        alt->data[j].r = (unsigned char) CLAMP(r_top);
                        alt->data[j].g = (unsigned char) CLAMP(g_top);
                        alt->data[j].b = (unsigned char) CLAMP(b_top);
                        continue;
                    }
                    if (col == col_bord) {
                        int index = w * row + col;
                        int index2 = w * (row_bord) + col;
                        r_top = pixies->data[index].r + (ny - (int) ny) / 2.0 * (pixies->data[index2].r - pixies->data[index].r);
                        g_top = pixies->data[index].g + (ny - (int) ny) / 2.0 * (pixies->data[index2].g - pixies->data[index].g);
                        b_top = pixies->data[index].b + (ny - (int) ny) / 2.0 * (pixies->data[index2].b - pixies->data[index].b);
                        alt->data[j].r = (unsigned char) CLAMP(r_top);
                        alt->data[j].g = (unsigned char) CLAMP(g_top);
                        alt->data[j].b = (unsigned char) CLAMP(b_top);
                        continue;
                    }
                    double x_dist = fabs(round(nx) - nx);
                    double y_dist = fabs(round(ny) - ny);
                    r_top = (1 - x_dist) * (1 - y_dist) * pixies->data[w * row + col].r + x_dist * (1 - y_dist) * pixies->data[w * row + col + 1].r + y_dist * (1 - x_dist) * pixies->data[w * (row + 1) + col].r + x_dist * y_dist * pixies->data[w * (row + 1) + col + 1].r;
                    g_top = (1 - x_dist) * (1 - y_dist) * pixies->data[w * row + col].g + x_dist * (1 - y_dist) * pixies->data[w * row + col + 1].g + y_dist * (1 - x_dist) * pixies->data[w * (row + 1) + col].g + x_dist * y_dist * pixies->data[w * (row + 1) + col + 1].g;
                    b_top = (1 - x_dist) * (1 - y_dist) * pixies->data[w * row + col].b + x_dist * (1 - y_dist) * pixies->data[w * row + col + 1].b + y_dist * (1 - x_dist) * pixies->data[w * (row + 1) + col].b + x_dist * y_dist * pixies->data[w * (row + 1) + col + 1].b;
                    alt->data[j].r = (unsigned char) CLAMP(r_top);
                    alt->data[j].g = (unsigned char) CLAMP(g_top);
                    alt->data[j].b = (unsigned char) CLAMP(b_top);
                    continue;
                }
                if (nx > 0  && ny < 0) {
                    int col_bord;
                    if (w - 1 > col + 1) {
                        col_bord = col + 1;
                    }
                    else {
                        col_bord = w - 1;
                    }
                    int row_bord;
                    if (h - 1 > row + 1) {
                        row_bord = row + 1;
                    }
                    else {
                        row_bord = h - 1;
                    }
                    if (row == row_bord && col == col_bord) {
                        alt->data[j] = pixies->data[row * w + col];
                        continue;
                    }
                    if (row == row_bord) {
                        int index = w * row + col;
                        r_top = pixies->data[index].r + (nx - (int) nx) / 2.0 * (pixies->data[index + 1].r - pixies->data[index].r);
                        g_top = pixies->data[index].g + (nx - (int) nx) / 2.0 * (pixies->data[index + 1].g - pixies->data[index].g);
                        b_top = pixies->data[index].b + (nx - (int) nx) / 2.0 * (pixies->data[index + 1].b - pixies->data[index].b);
                        alt->data[j].r = (unsigned char) CLAMP(r_top);
                        alt->data[j].g = (unsigned char) CLAMP(g_top);
                        alt->data[j].b = (unsigned char) CLAMP(b_top);
                        continue;
                    }
                    if (col == col_bord) {
                        int index = w * row + col;
                        int index2 = w * (row_bord) + col;
                        r_top = pixies->data[index].r + (ny - (int) ny) / 2.0 * (pixies->data[index2].r - pixies->data[index].r);
                        g_top = pixies->data[index].g + (ny - (int) ny) / 2.0 * (pixies->data[index2].g - pixies->data[index].g);
                        b_top = pixies->data[index].b + (ny - (int) ny) / 2.0 * (pixies->data[index2].b - pixies->data[index].b);
                        alt->data[j].r = (unsigned char) CLAMP(r_top);
                        alt->data[j].g = (unsigned char) CLAMP(g_top);
                        alt->data[j].b = (unsigned char) CLAMP(b_top);
                        continue;
                    }
                    double x_dist = fabs(round(nx) - nx);
                    double y_dist = fabs(round(ny) - ny);
                    r_top = (1 - x_dist) * (1 - y_dist) * pixies->data[w * row + col].r + x_dist * (1 - y_dist) * pixies->data[w * row + col + 1].r + y_dist * (1 - x_dist) * pixies->data[w * (row + 1) + col].r + x_dist * y_dist * pixies->data[w * (row + 1) + col + 1].r;
                    g_top = (1 - x_dist) * (1 - y_dist) * pixies->data[w * row + col].g + x_dist * (1 - y_dist) * pixies->data[w * row + col + 1].g + y_dist * (1 - x_dist) * pixies->data[w * (row + 1) + col].g + x_dist * y_dist * pixies->data[w * (row + 1) + col + 1].g;
                    b_top = (1 - x_dist) * (1 - y_dist) * pixies->data[w * row + col].b + x_dist * (1 - y_dist) * pixies->data[w * row + col + 1].b + y_dist * (1 - x_dist) * pixies->data[w * (row + 1) + col].b + x_dist * y_dist * pixies->data[w * (row + 1) + col + 1].b;
                    alt->data[j].r = (unsigned char) CLAMP(r_top);
                    alt->data[j].g = (unsigned char) CLAMP(g_top);
                    alt->data[j].b = (unsigned char) CLAMP(b_top);
                    continue;
                }
            }
            img_t *temp = pixies;
            temp->data = pixies->data;
            pixies = alt;
            pixies->data = alt->data;
            free(temp->data);
            free(temp);
        }
    }

    fprintf(out, "P6\n%d %d 255\n", pixies->w, pixies->h); // write the image header

    for (int p = 0; p < pixies->w * pixies->h; p = p + 1) {
        fwrite(&(pixies->data[p].r), sizeof(pixies->data[p].r), 1, out);
        fwrite(&(pixies->data[p].g), sizeof(pixies->data[p].g), 1, out);
        fwrite(&(pixies->data[p].b), sizeof(pixies->data[p].b), 1, out);
    }
    free(pixies->data);
    free(pixies);
    return 0;
}
