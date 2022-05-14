#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers = NULL;
png_bytep *gray_pointers = NULL;

void read_png_file(char *filename) {
    FILE *fp = fopen(filename, "rb");

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) abort();

    png_infop info = png_create_info_struct(png);
    if (!info) abort();

    if (setjmp(png_jmpbuf(png))) abort();

    png_init_io(png, fp);

    png_read_info(png, info);

    width = png_get_image_width(png, info);
    height = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if (bit_depth == 16)
        png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    if (row_pointers) abort();

    row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * height);
    gray_pointers = (png_bytep *) malloc(sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte *) malloc(png_get_rowbytes(png, info));
        gray_pointers[y] = (png_byte *) malloc(png_get_rowbytes(png, info));
    }

    png_read_image(png, row_pointers);

    fclose(fp);

    png_destroy_read_struct(&png, &info, NULL);
}

void write_png_file(char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) abort();

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) abort();

    png_infop info = png_create_info_struct(png);
    if (!info) abort();

    if (setjmp(png_jmpbuf(png))) abort();

    png_init_io(png, fp);

    png_set_IHDR(
            png,
            info,
            width, height,
            8,
            PNG_COLOR_TYPE_RGBA,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);

    if (!row_pointers) abort();

    png_write_image(png, row_pointers);
    png_write_end(png, NULL);

    for (int y = 0; y < height; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);

    fclose(fp);

    png_destroy_write_struct(&png, &info);
}

void sobel(int x, int y) {
    png_byte matrix[3][3];

    if (x == 0 || y == 0) {
        matrix[0][0] = 0;
    } else {
        matrix[0][0] = row_pointers[y - 1][(x - 1) * 4];
    }


    if (x == 0) {
        matrix[1][0] = 0;
    } else {
        matrix[1][0] = row_pointers[y][(x - 1) * 4];
    }


    if (x == 0 || y == height - 1) {
        matrix[2][0] = 0;
    } else {
        matrix[2][0] = row_pointers[y + 1][(x - 1) * 4];
    }


    if (y == 0) {
        matrix[0][1] = 0;
    } else {
        matrix[0][1] = row_pointers[y - 1][x * 4];
    }


    if (y == height - 1) {
        matrix[2][1] = 0;
    } else {
        matrix[2][1] = row_pointers[y + 1][x * 4];
    }

    matrix[1][1] = row_pointers[y][x * 4];

    if (y == 0 || x == width - 1) {
        matrix[0][2] = 0;
    } else {
        matrix[0][2] = row_pointers[y - 1][(x + 1) * 4];
    }

    if (x == width - 1) {
        matrix[1][2] = 0;
    } else {
        matrix[1][2] = row_pointers[y][(x + 1) * 4];
    }

    if (x == width - 1 || y == height - 1) {
        matrix[2][2] = 0;
    } else {
        matrix[2][2] = row_pointers[y + 1][(x + 1) * 4];
    }

    double xvalue = (matrix[2][0] + 2*matrix[2][1] + matrix[2][2]) - (matrix[0][0] + 2*matrix[0][1] + matrix[0][2]);
    double yvalue = (matrix[0][2] + 2*matrix[1][2] + matrix[2][2]) - (matrix[0][0] + 2*matrix[1][0] + matrix[0][2]);

    png_byte result = sqrt(xvalue*xvalue + yvalue*yvalue);

    gray_pointers[y][x*4] = result;
    gray_pointers[y][x*4 + 1] = result;
    gray_pointers[y][x*4 + 2] = result;
    gray_pointers[y][x*4 + 3] = 255;
}

void* sobel_row(void* row_ptr) {
    int row = *(int*)row_ptr;

    for (int x = 0; x < width; ++x) {
        sobel(x, row);
    }
}

void process_png_file() {
    for (int y = 0; y < height; y++) {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < width; x++) {
            png_bytep px = &(row[x * 4]);
            png_byte grayColor = 0.30 * px[0] + 0.59 * px[1] + 0.11 * px[2];

            px[0] = grayColor;
            px[1] = grayColor;
            px[2] = grayColor;
            px[3] = 255;
            //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
        }
    }

    struct timeval tv1,tv2,dtv;
    struct timezone tz;

    gettimeofday(&tv1, &tz);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            sobel(x, y);
        }
    }
    gettimeofday(&tv2, &tz);
    dtv.tv_sec= tv2.tv_sec -tv1.tv_sec;
    dtv.tv_usec=tv2.tv_usec-tv1.tv_usec;
    if(dtv.tv_usec<0) { dtv.tv_sec--; dtv.tv_usec+=1000000; }

    printf("time for single thread = %lu\n", dtv.tv_sec*1000+dtv.tv_usec/1000);

    int row_pointer[height];
    pthread_t threads[height];

    gettimeofday(&tv1, &tz);
    for (int y = 0; y < height; ++y) {
        row_pointer[y] = y;
        pthread_create(&threads[y], NULL, sobel_row, &row_pointer[y]);
    }

    for (int i = 0; i < height; ++i) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&tv2, &tz);
    dtv.tv_sec= tv2.tv_sec -tv1.tv_sec;
    dtv.tv_usec=tv2.tv_usec-tv1.tv_usec;
    if(dtv.tv_usec<0) { dtv.tv_sec--; dtv.tv_usec+=1000000; }

    printf("time for multiple thread = %lu", dtv.tv_sec*1000+dtv.tv_usec/1000);

    for (int y = 0; y < height; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);

    row_pointers = gray_pointers;
}

int main() {
    read_png_file("input.png");
    process_png_file();
    write_png_file("output.png");
}
