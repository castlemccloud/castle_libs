#ifndef __RASTER_H
#define __RASTER_H

#include <math.h>

#include "stb_image.h"



#define argb_to_uint(a, r, g, b) ((unsigned int)(((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | ((b & 0xFF))))

typedef struct {
	float x, y, z;
} vec3_t;

typedef struct {
	float x, y, z;
	float u, v;
} vertex_t;

typedef struct {
	unsigned long A, B, C;
} triangle_t;

typedef struct {
	unsigned long vertex_count;
	vertex_t * vertex_list;
	
	unsigned long triangle_count;
	triangle_t * triangle_list;
} model_t;


typedef struct {
	unsigned long width;
	unsigned long height;
	unsigned int * pixels;
} texture_t;

texture_t * load_texture(const char * texture_name);
void destroy_texture(texture_t * texture);

unsigned int sample_texture(texture_t * texture, float u, float v);



void render_model(unsigned int * pix_buf, float * depth_buf, long hor_res, long ver_res, model_t * model, texture_t * texture);


#endif