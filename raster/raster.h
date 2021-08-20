

#ifndef __RASTER_H
#define __RASTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <SDL2/SDL.h>


typedef struct {
	double x, y, z;
} vec_t;


typedef struct {
	long x, y, z;
} tex_t;


typedef struct {
	long vA, vB, vC; // Geometric
	long tX, tY, tZ; // Texture
	long nI, nJ, nK; // Normal
} triangle_t;


typedef struct {
	
	vec_t * vec_arr; long vec_count;
	tex_t * tex_arr; long tex_count;
	vec_t * nor_arr; long nor_count;
	
	triangle_t * tri_arr; long tri_count;	
	
	SDL_Surface * tex;
	
} model_t;


typedef struct {
	
	// Position & Orientation
	vec_t pos;
	vec_t look;
	vec_t up;
	vec_t right;
	
	long hRes, vRes;
	double FOV;		// Field of View in radians
	double zNear;	// Near Clipping Plane
	double zFar;	// Far Clipping Plane
} camera_t;


// Loads .obj and .bmp file(s) into a model
// If tex is NULL, modle will default to grey.
// See https://en.wikipedia.org/wiki/Wavefront_.obj_file for information about file format
model_t * load_obj(const char * obj_name, const char * tex_name);
void destroy_model(model_t * mod);

// Renders model from cam and writes to surface, with depth mapping
void render_model(const model_t * mod, const camera_t * cam, unsigned int * pix_buf, double * depth);


/* End of cplusplus */
#ifdef __cplusplus
}
#endif

/* End of __RASTER_H */
#endif