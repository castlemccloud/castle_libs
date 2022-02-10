#ifndef __RASTER_H
#define __RASTER_H

#include <math.h>


#include "stb_image.h"
#include "tiny_obj_loader.h"


#define argb_to_uint(a, r, g, b) ((unsigned int)(((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | ((b & 0xFF))))

typedef struct {
	float x, y, z;
} vec3_t;

typedef struct {
	float u, v;
} vec2_t;

typedef struct {
	unsigned long vA, vB, vC;
	unsigned long tA, tB, tC;
} triangle_t;



typedef struct {
	vec3_t pos; // X, Y ,Z
	vec3_t look;
	vec3_t up;
	vec3_t right;
	float fov;
	float far;
	float near;
	unsigned long hor_res;
	unsigned long ver_res;
} camera_t;



typedef struct {
	unsigned long width;
	unsigned long height;
	unsigned int * pixels;
} texture_t;

texture_t * load_texture(const char * texture_name);
void destroy_texture(texture_t * texture);

unsigned int sample_texture(texture_t * texture, float u, float v);



typedef struct {
	unsigned long vec_count;
	vec3_t * vec_list;
	
	unsigned long tex_count;
	vec2_t * tex_list;
	
	unsigned long tri_count;
	triangle_t * tri_list;
} model_t;

model_t * load_model(const char * model_name);
void destroy_model(model_t * model);

void render_model(unsigned int * pix_buf, float * depth_buf, camera_t cam, model_t * model, texture_t * texture);





#endif