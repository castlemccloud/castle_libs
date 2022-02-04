#ifndef __RASTER_H
#define __RASTER_H

#include <math.h>

#define argb_to_uint(a, r, g, b) ((unsigned int)(((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | ((b & 0xFF))))

typedef struct {
	float x, y, z;
} vec3_t;

typedef struct {
	
	float x, y, z;
	float a, r, g, b;
	
} vertex_t;


typedef struct {
	
	vertex_t A, B, C;
	
} triangle_t;


/*
typedef struct {
	
	unsigned long vertex_count;
	vertex_t * vertex_list;
	
	unsigned long triangle_count;
	unsigned long * triangle_list;
	
} model_t;

void render_model(unsigned int * pix_buf, float * depth_buf, long hor_res, long ver_res, model_t * model);
*/



// Triangle must be in image space: [[-1, 1], [-1, 1], [0, 1]]
void draw_triangle(unsigned int * pix_buf, float * depth_buf, long hor_res, long ver_res, triangle_t tri);



#endif