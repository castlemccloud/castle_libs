#include "../matrix/matrix.h"
#include "raster.h"






model_t * load_obj(const char * file_name, const char * tex_name) {
	
	// Read obj into new model.
	
	// Read Texture into model's texture surface.
	
	if (file_name == NULL) return NULL;
	
	
	
	vec_t * V; long size_V; long capacity_V;
	triangle_t * F; long size_F; long capacity_F;
	
	
	capacity_V = 128;
	size_V = 0;
	V = (vec_t *) malloc(sizeof(vec_t) * capacity_V);
	
	capacity_F = 128;
	size_F = 0;
	F = (triangle_t *) malloc(sizeof(triangle_t) * capacity_F);
	
	
	
	FILE * fp = fopen(file_name, "r");
	if(fp == NULL) return NULL;
	int status;
	
	
	while(1) {
		
		char type;
		status = fscanf(fp, "%c", &type);
		
		if (status == EOF) break;
		
		if (type == 'v') {
			double x, y, z;
			fscanf(fp, "%lf %lf %lf\n", &x, &y, &z);
			
			V[size_V] = (vec_t) {x, y, z};
			size_V++;
			
			if (size_V == capacity_V) {
				vec_t * new_V = (vec_t *) malloc(sizeof(vec_t) * 2 * capacity_V);
				for(long i = 0; i < size_V; i++) {
					new_V[i] = V[i];
				}
				free(V);
				capacity_V *= 2;
				V = new_V;
			}
			
		} else if (type == 'f') {
			long x, y, z;
			fscanf(fp, "%ld %ld %ld\n", &x, &y, &z);
			
			F[size_F] = (triangle_t) {x, y, z, -1, -1, -1, -1, -1, -1};
			size_F++;
			
			if(size_F == capacity_F) {
				triangle_t * new_F = (triangle_t *) malloc(sizeof(triangle_t) * 2 * capacity_F);
				for(long i = 0; i < size_F; i++) {
					new_F[i] = F[i];
				}
				free(F);
				capacity_F *= 2;
				F = new_F;
			}
		} else {
			continue;
		}
		
		
	}
	
	fclose(fp);
	
	
	
	model_t * rtn = (model_t *) malloc(sizeof(model_t));
	
	rtn->vec_arr = (vec_t *) malloc(sizeof(vec_t) * size_V);
	rtn->vec_count = size_V;
	for(long i = 0; i < size_V; i++) {
		rtn->vec_arr[i] = V[i];
	}
	free(V);
	
	
	rtn->tex_arr = NULL;
	rtn->tex_count = 0;
	
	
	rtn->nor_arr = NULL;
	rtn->nor_count = 0;
	
	
	rtn->tri_arr = (triangle_t *) malloc(sizeof(triangle_t) * size_F);
	rtn->tri_count = size_F;
	for(long i = 0; i < size_F; i++) {
		rtn->tri_arr[i] = F[i];
	}
	free(F);
	
	
	
	if (tex_name) {
		rtn->tex = SDL_LoadBMP(tex_name);
	} else {
		rtn->tex = NULL;
	}
	
	return rtn;
}

void destroy_model(model_t * mod) {
	if (mod->vec_arr) free(mod->vec_arr);
	if (mod->tex_arr) free(mod->tex_arr);
	if (mod->nor_arr) free(mod->nor_arr);
	
	if (mod->tri_arr) free(mod->tri_arr);
	
	if (mod->tex) SDL_FreeSurface(mod->tex);
	
	free(mod);
}


void lerp_vec(vec_t * A, vec_t * B, double u, vec_t * C) {
	double v = 1.0 - u;
	
	
	C->x = (A->x * u) + (B->x * v);
	C->y = (A->y * u) + (B->y * v);
	C->z = (A->z * u) + (B->z * v);
	
}

void lerp_tex(tex_t * A, tex_t * B, double u, tex_t * C) {
	double v = 1.0 - u;
	
	C->x = (A->x * u) + (B->x * v);
	C->y = (A->y * u) + (B->y * v);
	C->z = (A->z * u) + (B->z * v);
	
}



void draw_triangle(unsigned int * dest, unsigned long hRes, unsigned long vRes,
				   double * depth, 
				   vec_t * vA, vec_t * vB, vec_t * vC, 
				   tex_t * tA, tex_t * tB, tex_t * tC, 
				   SDL_Surface * src) {
	
	
	/*
	printf("Triangle:\n");
	printf("\t[%lf, %lf, %lf]\n", vA->x, vA->y, vA->z);
	printf("\t[%lf, %lf, %lf]\n", vB->x, vB->y, vB->z);
	printf("\t[%lf, %lf, %lf]\n", vC->x, vC->y, vC->z);
	*/
	
	if (
		(vA->x < 1.0 && vA->x > -1.0)
		&& (vA->y < 1.0 && vA->y > -1.0)
		&& (vA->z > -1.0 && vA->z < 1.0)
	) {
		long Ax = (vA->x + 1.0) * (hRes/2);
		long Ay = (1.0 - vA->y) * (vRes/2);
		dest[Ax + Ay*hRes] = 0xFFFFFFFF;
	}
	
	
	if (
		(vB->x < 1.0 && vB->x > -1.0)
		&& (vB->y < 1.0 && vB->y > -1.0)
		&& (vB->z > -1.0 && vB->z < 1.0)
	) {
		long Bx = (vB->x + 1.0) * (hRes/2);
		long By = (1.0 - vB->y) * (vRes/2);
		dest[Bx + By*hRes] = 0xFFFFFFFF;
	}
	
	
	if (
		(vC->x < 1.0 && vC->x > -1.0)
		&& (vC->y < 1.0 && vC->y > -1.0)
		&& (vC->z > -1.0 && vC->z < 1.0)
	) {
		long Bx = (vC->x + 1.0) * (hRes/2);
		long By = (1.0 - vC->y) * (vRes/2);
		dest[Bx + By*hRes] = 0xFFFFFFFF;
	}
	
	
	
	
	// Check Normal
	// +-Z
	
	
	// Clipping
	// If All points out, return. nothing to draw
	// If Two points out, move them in, and continue
	// If One point out, cut triangle in two, 
	//   recursive call on second, then continue on first
	
	
	// z < -1
	// z >  1
	
	// y < -1
	// y >  1
	
	// x < -1
	// x >  1
	
	
	
	// Draw Triangle
	// reorder points in order of Y
	// Least to greatest
	//
	// lines, AB, AC, BC
	// 
	// Determine if bend is left or right
	// IF left
	//      A
	//   B
	//         C
	//   Draw lines from AB to AC
	//   Then lines from BC to AC
	// ELSE
	//      A
	//         B
	//   C
	//   Draw line from AC to AB
	//   Then lines from AC to BC
	
	
	
	
	
	
	
	
	
	
}





void render_model(const model_t * mod, const camera_t * cam, unsigned int * dst, double * depth) {
	/* *
	 * Function takes a model, and camera, then draws to dst and depth map.
	 *
	 * Render Pipeline:
	 * 
	 * Translation 
	 * 		Translate Model's vec_arr by cam's position to get position relative to camera
	 * 
	 * Rotation
	 * 		Rotate Model's vec_arr and nor_arr by cam's orientation 
	 *		such that +x goes right, +y goes up, and +z goes away from the camera
	 *
	 *	Projection
	 *		divide by distance, and accout for aspect ratio
	 *
	 *	Clipping
	 *		Clip triangles points to inside of image space
	 *
	 * 	Scaling
	 * 		Scale into screen space
	 * 
	 * 	Raster
	 * 		Raster the triangles onto the SDL_Texture.
	 */
	
	if (mod == NULL || cam == NULL) return;	// Invalid inputs.
	
	
	
	// Make Rotation Matrix
	
	double TA = -((cam->right.x * cam->pos.x) + (cam->right.y * cam->pos.y) + (cam->right.z * cam->pos.z));
	double TB = -((cam->up.x * cam->pos.x) + (cam->up.y * cam->pos.y) + (cam->up.z * cam->pos.z));
	double TC = -((cam->look.x * cam->pos.x) + (cam->look.y * cam->pos.y) + (cam->look.z * cam->pos.z));
	
	matrix_t * rotation = make_matrix(4, 4);
	mpc_set_d(get_matrix(rotation, 0, 0), cam->right.x, MPC_RNDDD);
	mpc_set_d(get_matrix(rotation, 1, 0), cam->right.y, MPC_RNDDD);
	mpc_set_d(get_matrix(rotation, 2, 0), cam->right.z, MPC_RNDDD);
	mpc_set_d(get_matrix(rotation, 3, 0), TA, MPC_RNDDD);
	
	mpc_set_d(get_matrix(rotation, 0, 1), cam->up.x, MPC_RNDDD);
	mpc_set_d(get_matrix(rotation, 1, 1), cam->up.y, MPC_RNDDD);
	mpc_set_d(get_matrix(rotation, 2, 1), cam->up.z, MPC_RNDDD);
	mpc_set_d(get_matrix(rotation, 3, 1), TB, MPC_RNDDD);
	
	mpc_set_d(get_matrix(rotation, 0, 2), cam->look.x, MPC_RNDDD);
	mpc_set_d(get_matrix(rotation, 1, 2), cam->look.y, MPC_RNDDD);
	mpc_set_d(get_matrix(rotation, 2, 2), cam->look.z, MPC_RNDDD);
	mpc_set_d(get_matrix(rotation, 3, 2), TC, MPC_RNDDD);
	
	mpc_set_d(get_matrix(rotation, 3, 3), 1.0, MPC_RNDDD);
	
	
	// Make Projection Matrix
	
	double ar = (double)cam->vRes / (double)cam->hRes;
	
	double tF = 1.0 / tan(cam->FOV / 2.0);
	
	double zNear = cam->zNear;
	double zFar = cam->zFar;
	double izRange = 1.0 / (zFar - zNear);
	
	
	matrix_t * projection = make_matrix(4, 4);
	mpc_set_d(get_matrix(projection, 0, 0), (ar * tF), MPC_RNDDD);
	mpc_set_d(get_matrix(projection, 1, 1), (tF), MPC_RNDDD);
	mpc_set_d(get_matrix(projection, 2, 2), (2.0) * izRange, MPC_RNDDD);
	mpc_set_d(get_matrix(projection, 3, 2), (-(zNear + zFar)) * izRange, MPC_RNDDD);
	mpc_set_d(get_matrix(projection, 2, 3), 1.0, MPC_RNDDD);
	mpc_set_d(get_matrix(projection, 3, 3), 1.0, MPC_RNDDD);
	
	
	matrix_t * Complete = mult_matrix(rotation, projection);
	
	destroy_matrix(rotation);
	destroy_matrix(projection);
	
	
	matrix_t * vec_matrix = make_matrix(mod->vec_count, 4);
	for(long j = 0; j < (mod->vec_count); j++) {
		
		mpc_set_d(get_matrix(vec_matrix, j, 0), mod->vec_arr[j].x, MPC_RNDDD);
		mpc_set_d(get_matrix(vec_matrix, j, 1), mod->vec_arr[j].y, MPC_RNDDD);
		mpc_set_d(get_matrix(vec_matrix, j, 2), mod->vec_arr[j].z, MPC_RNDDD);
		mpc_set_d(get_matrix(vec_matrix, j, 3), 1.0, MPC_RNDDD);

	}
	
	
	matrix_t * image_space = mult_matrix(vec_matrix, Complete);
	
	destroy_matrix(Complete);
	destroy_matrix(vec_matrix);
	
	vec_t * vec_arr = (vec_t *) malloc(sizeof(vec_t) * mod->vec_count);
	
	for(long j = 0; j < (mod->vec_count); j++) {
		
		double X = creal(mpc_get_dc(get_matrix(image_space, j, 0), MPC_RNDDD));
		double Y = creal(mpc_get_dc(get_matrix(image_space, j, 1), MPC_RNDDD));
		double Z = creal(mpc_get_dc(get_matrix(image_space, j, 2), MPC_RNDDD));
		double W = creal(mpc_get_dc(get_matrix(image_space, j, 3), MPC_RNDDD));
		
		if (W != 0.0) {
			X /= W;
			Y /= W;
		}
		
		vec_arr[j] = (vec_t){X, Y, Z};
		
	}
	
	destroy_matrix(image_space);
	
	
	for(long i = 0; i < mod->tri_count; i++) {
		
		long vA; long vB; long vC;
		long tX; long tY; long tZ;
		
		vA = mod->tri_arr[i].vA;
		vB = mod->tri_arr[i].vB;
		vC = mod->tri_arr[i].vC;
		
		tX = mod->tri_arr[i].tX;
		tY = mod->tri_arr[i].tY;
		tZ = mod->tri_arr[i].tZ;
		
		draw_triangle(dst, cam->hRes, cam->vRes, depth, 
					  vec_arr + vA, vec_arr + vB, vec_arr + vC,
					  mod->tex_arr + tX, mod->tex_arr + tY, mod->tex_arr + tZ,
					  mod->tex);
	}
	
	free(vec_arr);
	
}
