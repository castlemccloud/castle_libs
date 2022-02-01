



/*

Goal:
	
	Camera + Environment => Image
	
	How?
	
	Make Camera
	
		Basic information
	
	Make Environment
		
		Load model from file
		
		Load Texture from file
		
	Make Image
	
		How?
		
		Camera makes lots of rays
			
			How?
			
			Number of rays are dependant on resolution of Camera
			
			Ray Origin is camera position
			
				Ray Direction is dependant on camera Look, Up, Right, and FOV
		
		Rays intersect with environment
			
			How?
			
			Model makes a Bounded Volume Heirarchy
				
			Ray intersect with BVH, down untill it hits primatives at bottom
				
				Closest first search
				
			Recursive for reflections, refractions, Specular lighting etc
		
		Intersections determine color of image
		
			How?
			
			Average results of all rays cast for each pixel
		
		
		
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <omp.h>
#include <SDL2/SDL.h>

#include "btree.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"




typedef struct {
	float x, y, z;
} vec_t;

float vec_dot(const vec_t A, const vec_t B) {
	return (A.x * B.x) + (A.y * B.y) + (A.z * B.z);
}

vec_t vec_cross(const vec_t A, const vec_t B) {
	return (vec_t){((A.y * B.z) - (A.z * B.y)), 
				   ((A.z * B.x) - (A.x * B.z)), 
				   ((A.x * B.y) - (A.y * B.x))};
}

vec_t vec_scale(const vec_t A, float s) {
	return (vec_t){A.x * s, A.y * s, A.z * s};
}

vec_t vec_add(const vec_t A, const vec_t B) {
	return (vec_t){A.x + B.x, A.y + B.y, A.z + B.z};
}

vec_t vec_rotate(const vec_t A, const vec_t B, float t) {
	// Ru(t)x = u * DOT(u, x) + cos(t) * CROSS(CROSS(u, x), u) + sin(t) * CROSS(u, x)
	
	vec_t w = vec_cross(B, A);
	vec_t v = vec_cross(w, B);
	
	vec_t para = vec_scale(B, vec_dot(A, B));
	
	vec_t vsin = vec_scale(w, sin(t));
	
	vec_t vcos = vec_scale(v, cos(t));
	
	return vec_add(para, vec_add(vsin, vcos));
	
}



typedef struct {
	float r, g, b, a;
} color_t;

typedef struct {
	vec_t pos;
	vec_t look;
	vec_t up;
	vec_t right;
	
	float hFOV;
	float vFOV;
	
	unsigned long hRES;
	unsigned long vRES;
	
} Camera_t;

typedef struct {
	float u, v;
} texel_t;

typedef struct {
	// Indices for each position
	int vA, vB, vC;
	
	// Indices for each texel
	int tA, tB, tC;
	
} triangle_t;

typedef struct _bounded_volume {
	vec_t min;
	vec_t mid;
	vec_t max;
	
	struct _bounded_volume ** children;
	
	unsigned long primative_count;
	unsigned long * primative_list;
	
} BVH_t;

typedef struct {
	vec_t pos;
	color_t color;
	
} light_t;

typedef struct {
	
	//   Positions
	vec_t * vec_list; unsigned long vec_count;
	
	//   Texture Coordinates
	texel_t * tex_list; unsigned long tex_count;
	
	// List of Triangles
	triangle_t * tri_list; unsigned long tri_count;
	
} model_t;

model_t * load_model(const char * fname) {
	
	
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fname, NULL, true)) {
		throw std::runtime_error(warn + err);
	}
	
	
	unsigned long vec_count = attrib.vertices.size() / 3;
	vec_t * vec_list = (vec_t *) malloc(sizeof(vec_t) * vec_count);
	memcpy(vec_list, attrib.vertices.data(), sizeof(vec_t) * vec_count);
//	for(unsigned long i = 0; i < vec_count; i++) {
//		printf("V %ld: {%f, %f, %f}\n", i, vec_list[i].x, vec_list[i].y, vec_list[i].z);
//	}
	
	unsigned long tex_count = attrib.texcoords.size() / 2;
	texel_t * tex_list = (texel_t *) malloc(sizeof(texel_t) * tex_count);
	memcpy(tex_list, attrib.texcoords.data(), sizeof(texel_t) * tex_count);
//	for(unsigned long i = 0; i < tex_count; i++) {
//		printf("T %ld of %ld: {%f, %f}\n", i, tex_count, tex_list[i].u, tex_list[i].v);
//	}
	
	unsigned long tri_count = 0;
	for(unsigned long i = 0; i < shapes.size(); i++) {
		tri_count += shapes[i].mesh.indices.size() / 3;
	}
	
	triangle_t * tri_list = (triangle_t *) malloc(sizeof(triangle_t) * tri_count);
	unsigned long tri_index = 0;
	for(unsigned long i = 0; i < shapes.size(); i++) {
		
		for(unsigned long j = 0; j < shapes[i].mesh.indices.size(); j += 3) {
			
			tinyobj::index_t iA = shapes[i].mesh.indices[j];
			tinyobj::index_t iB = shapes[i].mesh.indices[j+1];
			tinyobj::index_t iC = shapes[i].mesh.indices[j+2];
			
			tri_list[tri_index] = (triangle_t){iA.vertex_index, iB.vertex_index, iC.vertex_index, 
											   iA.texcoord_index, iB.texcoord_index, iC.texcoord_index};
			
			tri_index++;
		}
		
	}
	
	
	model_t * rtn = (model_t *) malloc(sizeof(model_t));
	
	rtn->vec_list = vec_list;
	rtn->vec_count = vec_count;
	
	rtn->tex_list = tex_list;
	rtn->tex_count = tex_count;
	
	rtn->tri_list = tri_list;
	rtn->tri_count = tri_count;
	
	return rtn;
	
}

void destroy_model(model_t * model) {
	free(model->vec_list);
	free(model->tex_list);
	free(model->tri_list);
	free(model);
}



BVH_t * create_bvh(unsigned long * primative_list, unsigned long primative_count, triangle_t * tri_list, vec_t * vec_list, vec_t * smin, vec_t * smax, unsigned long limit, unsigned long depth) {
	
	if (primative_list == NULL || primative_count <= 0 || tri_list == NULL || vec_list == NULL) return NULL;
	
	BVH_t * rtn = (BVH_t *) malloc(sizeof(BVH_t));
	
	unsigned char bSetMinMax = 0;
	
	if (smin == NULL || smax == NULL) {
		bSetMinMax = 1;
		rtn->min = (vec_t){INFINITY, INFINITY, INFINITY};
		rtn->max = (vec_t){-INFINITY, -INFINITY, -INFINITY};
	} else {
		bSetMinMax = 0;
		rtn->min = *smin;
		rtn->max = *smax;
	}
	
	rtn->mid = (vec_t){0.0, 0.0, 0.0};
	unsigned long vec_count = 0;
	btree_t * vec_tree = NULL;
	
	for(unsigned long i = 0; i < primative_count; i++) {
		
		triangle_t * tri = tri_list + primative_list[i];
		
		vec_t * A = vec_list + tri->vA;
		if(!contains_btree(vec_tree, tri->vA)) {
			insert_btree(&vec_tree, tri->vA);
			
			rtn->mid.x += A->x; rtn->mid.y += A->y; rtn->mid.z += A->z;
			vec_count++;
				
			if (bSetMinMax) {
				if(rtn->min.x > A->x) rtn->min.x = A->x;
				if(rtn->min.y > A->y) rtn->min.y = A->y;
				if(rtn->min.z > A->z) rtn->min.z = A->z;
				
				if(rtn->max.x < A->x) rtn->max.x = A->x;
				if(rtn->max.y < A->y) rtn->max.y = A->y;
				if(rtn->max.z < A->z) rtn->max.z = A->z;
				
			}
		}
		
		vec_t * B = vec_list + tri->vB;
		if(!contains_btree(vec_tree, tri->vB)) {
			insert_btree(&vec_tree, tri->vB);
			
			rtn->mid.x += B->x; rtn->mid.y += B->y; rtn->mid.z += B->z;
			vec_count++;
			
			if (bSetMinMax) {
				if(rtn->min.x > B->x) rtn->min.x = B->x;
				if(rtn->min.y > B->y) rtn->min.y = B->y;
				if(rtn->min.z > B->z) rtn->min.z = B->z;
				
				if(rtn->max.x < B->x) rtn->max.x = B->x;
				if(rtn->max.y < B->y) rtn->max.y = B->y;
				if(rtn->max.z < B->z) rtn->max.z = B->z;
				
			}
		}
		
		
		vec_t * C = vec_list + tri->vC;
		if(!contains_btree(vec_tree, tri->vC)) {
			insert_btree(&vec_tree, tri->vC);
			
			rtn->mid.x += C->x; rtn->mid.y += C->y; rtn->mid.z += C->z;
			vec_count++;
			
			if (bSetMinMax) {
				if(rtn->min.x > C->x) rtn->min.x = C->x;
				if(rtn->min.y > C->y) rtn->min.y = C->y;
				if(rtn->min.z > C->z) rtn->min.z = C->z;
				
				if(rtn->max.x < C->x) rtn->max.x = C->x;
				if(rtn->max.y < C->y) rtn->max.y = C->y;
				if(rtn->max.z < C->z) rtn->max.z = C->z;
				
			}
		}
	}
	
	free_btree(vec_tree);
	
	if (vec_count != 0) {
		
		rtn->mid.x /= (double)vec_count;
		rtn->mid.y /= (double)vec_count;
		rtn->mid.z /= (double)vec_count;
		
	} else {
		
		rtn->mid.x = rtn->min.x + ((rtn->max.x - rtn->min.x) / 2.0f);
		rtn->mid.y = rtn->min.y + ((rtn->max.y - rtn->min.y) / 2.0f);
		rtn->mid.z = rtn->min.z + ((rtn->max.z - rtn->min.z) / 2.0f);
		
	}
	
	
	if (rtn->mid.x < rtn->min.x || rtn->mid.y < rtn->min.y || rtn->mid.z < rtn->min.z || 
	    rtn->mid.x > rtn->max.x || rtn->mid.y > rtn->max.y || rtn->mid.z > rtn->max.z) {
		
		rtn->mid.x = rtn->min.x + ((rtn->max.x - rtn->min.x) / 2.0f);
		rtn->mid.y = rtn->min.y + ((rtn->max.y - rtn->min.y) / 2.0f);
		rtn->mid.z = rtn->min.z + ((rtn->max.z - rtn->min.z) / 2.0f);
	}
	
	
	
	if (primative_count > limit && depth > 0) {
		
		rtn->children = (BVH_t **) malloc(sizeof(BVH_t *) * 8);
		
		#pragma omp parallel for
		for(unsigned char i = 0; i < 8; i++) {
			
			vec_t temp_min = (vec_t){(i & 0x1) ? rtn->mid.x : rtn->min.x, 
									 (i & 0x2) ? rtn->mid.y : rtn->min.y, 
									 (i & 0x4) ? rtn->mid.z : rtn->min.z};
			vec_t temp_max = (vec_t){(i & 0x1) ? rtn->max.x : rtn->mid.x, 
									 (i & 0x2) ? rtn->max.y : rtn->mid.y, 
									 (i & 0x4) ? rtn->max.z : rtn->mid.z};
			
			unsigned long * temp_primative_list = (unsigned long *) malloc(sizeof(unsigned long) * primative_count);
			
			#pragma omp parallel for
			for(unsigned long j = 0; j < primative_count; j++) {
				
				triangle_t * tri = tri_list + primative_list[j];
				
				vec_t v0 = vec_list[tri->vA];
				vec_t v1 = vec_list[tri->vB];
				vec_t v2 = vec_list[tri->vC];
				
				vec_t e = (vec_t){(temp_max.x - temp_min.x) / 2.0f, (temp_max.y - temp_min.y) / 2.0f, (temp_max.z - temp_min.z) / 2.0f};
				vec_t c = (vec_t){temp_min.x + e.x, temp_min.y + e.y, temp_min.z + e.z};
				
				v0.x -= c.x; v0.y -= c.y; v0.z -= c.z;
				v1.x -= c.x; v1.y -= c.y; v1.z -= c.z;
				v2.x -= c.x; v2.y -= c.y; v2.z -= c.z;
				
				vec_t f0 = (vec_t){v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
				vec_t f1 = (vec_t){v2.x - v1.x, v2.y - v1.y, v2.z - v1.z};
				vec_t f2 = (vec_t){v0.x - v2.x, v0.y - v2.y, v0.z - v2.z};
				
				vec_t u0 = (vec_t){1.0f, 0.0f, 0.0f};
				vec_t u1 = (vec_t){0.0f, 1.0f, 0.0f};
				vec_t u2 = (vec_t){0.0f, 0.0f, 1.0f};
				
				
				
				float p0, p1, p2, r;
				float u, v;
				
				vec_t a_u0_f0 = vec_cross(u0, f0);
				
				p0 = vec_dot(v0, a_u0_f0);
				p1 = vec_dot(v1, a_u0_f0);
				p2 = vec_dot(v2, a_u0_f0);
				
				r = e.x * abs(vec_dot(u0, a_u0_f0)) + 
					e.y * abs(vec_dot(u1, a_u0_f0)) + 
					e.z * abs(vec_dot(u2, a_u0_f0));
				
				u = p0;				
				if (p1 > u) u = p1;	
				if (p2 > u) u = p2;
				
				v = p0;
				if (p1 < v) v = p1;
				if (p2 < v) v = p2;
				
				if (fmax(-u, v) > r) {
					temp_primative_list[j] = 0;
					continue;
				}
				
				
				vec_t a_u0_f1 = vec_cross(u0, f1);
				
				p0 = vec_dot(v0, a_u0_f1);
				p1 = vec_dot(v1, a_u0_f1);
				p2 = vec_dot(v2, a_u0_f1);
				
				r = e.x * abs(vec_dot(u0, a_u0_f1)) + 
					e.y * abs(vec_dot(u1, a_u0_f1)) + 
					e.z * abs(vec_dot(u2, a_u0_f1));
				
				u = p0;				
				if (p1 > u) u = p1;	
				if (p2 > u) u = p2;
				
				v = p0;
				if (p1 < v) v = p1;
				if (p2 < v) v = p2;
				
				if (fmax(-u, v) > r) {
					temp_primative_list[j] = 0;
					continue;
				}
				
				
				vec_t a_u0_f2 = vec_cross(u0, f2);
				
				p0 = vec_dot(v0, a_u0_f2);
				p1 = vec_dot(v1, a_u0_f2);
				p2 = vec_dot(v2, a_u0_f2);
				
				r = e.x * abs(vec_dot(u0, a_u0_f2)) + 
					e.y * abs(vec_dot(u1, a_u0_f2)) + 
					e.z * abs(vec_dot(u2, a_u0_f2));
				
				u = p0;				
				if (p1 > u) u = p1;	
				if (p2 > u) u = p2;
				
				v = p0;
				if (p1 < v) v = p1;
				if (p2 < v) v = p2;
				
				if (fmax(-u, v) > r) {
					temp_primative_list[j] = 0;
					continue;
				}
				
				
				vec_t a_u1_f0 = vec_cross(u1, f0);
				
				p0 = vec_dot(v0, a_u1_f0);
				p1 = vec_dot(v1, a_u1_f0);
				p2 = vec_dot(v2, a_u1_f0);
				
				r = e.x * abs(vec_dot(u0, a_u1_f0)) + 
					e.y * abs(vec_dot(u1, a_u1_f0)) + 
					e.z * abs(vec_dot(u2, a_u1_f0));
				
				u = p0;				
				if (p1 > u) u = p1;	
				if (p2 > u) u = p2;
				
				v = p0;
				if (p1 < v) v = p1;
				if (p2 < v) v = p2;
				
				if (fmax(-u, v) > r) {
					temp_primative_list[j] = 0;
					continue;
				}
				
				
				vec_t a_u1_f1 = vec_cross(u1, f1);
				
				p0 = vec_dot(v0, a_u1_f1);
				p1 = vec_dot(v1, a_u1_f1);
				p2 = vec_dot(v2, a_u1_f1);
				
				r = e.x * abs(vec_dot(u0, a_u1_f1)) + 
					e.y * abs(vec_dot(u1, a_u1_f1)) + 
					e.z * abs(vec_dot(u2, a_u1_f1));
				
				u = p0;				
				if (p1 > u) u = p1;	
				if (p2 > u) u = p2;
				
				v = p0;
				if (p1 < v) v = p1;
				if (p2 < v) v = p2;
				
				if (fmax(-u, v) > r) {
					temp_primative_list[j] = 0;
					continue;
				}
				
				
				vec_t a_u1_f2 = vec_cross(u1, f2);
				
				p0 = vec_dot(v0, a_u1_f2);
				p1 = vec_dot(v1, a_u1_f2);
				p2 = vec_dot(v2, a_u1_f2);
				
				r = e.x * abs(vec_dot(u0, a_u1_f2)) + 
					e.y * abs(vec_dot(u1, a_u1_f2)) + 
					e.z * abs(vec_dot(u2, a_u1_f2));
				
				u = p0;				
				if (p1 > u) u = p1;	
				if (p2 > u) u = p2;
				
				v = p0;
				if (p1 < v) v = p1;
				if (p2 < v) v = p2;
				
				if (fmax(-u, v) > r) {
					temp_primative_list[j] = 0;
					continue;
				}
				
				
				vec_t a_u2_f0 = vec_cross(u2, f0);
				
				p0 = vec_dot(v0, a_u2_f0);
				p1 = vec_dot(v1, a_u2_f0);
				p2 = vec_dot(v2, a_u2_f0);
				
				r = e.x * abs(vec_dot(u0, a_u2_f0)) + 
					e.y * abs(vec_dot(u1, a_u2_f0)) + 
					e.z * abs(vec_dot(u2, a_u2_f0));
				
				u = p0;				
				if (p1 > u) u = p1;	
				if (p2 > u) u = p2;
				
				v = p0;
				if (p1 < v) v = p1;
				if (p2 < v) v = p2;
				
				if (fmax(-u, v) > r) {
					temp_primative_list[j] = 0;
					continue;
				}
				
				
				vec_t a_u2_f1 = vec_cross(u2, f1);
				
				p0 = vec_dot(v0, a_u2_f1);
				p1 = vec_dot(v1, a_u2_f1);
				p2 = vec_dot(v2, a_u2_f1);
				
				r = e.x * abs(vec_dot(u0, a_u2_f1)) + 
					e.y * abs(vec_dot(u1, a_u2_f1)) + 
					e.z * abs(vec_dot(u2, a_u2_f1));
				
				u = p0;				
				if (p1 > u) u = p1;	
				if (p2 > u) u = p2;
				
				v = p0;
				if (p1 < v) v = p1;
				if (p2 < v) v = p2;
				
				if (fmax(-u, v) > r) {
					temp_primative_list[j] = 0;
					continue;
				}
				
				
				vec_t a_u2_f2 = vec_cross(u2, f2);
				
				p0 = vec_dot(v0, a_u2_f2);
				p1 = vec_dot(v1, a_u2_f2);
				p2 = vec_dot(v2, a_u2_f2);
				
				r = e.x * abs(vec_dot(u0, a_u2_f2)) + 
					e.y * abs(vec_dot(u1, a_u2_f2)) + 
					e.z * abs(vec_dot(u2, a_u2_f2));
				
				u = p0;				
				if (p1 > u) u = p1;	
				if (p2 > u) u = p2;
				
				v = p0;
				if (p1 < v) v = p1;
				if (p2 < v) v = p2;
				
				if (fmax(-u, v) > r) {
					temp_primative_list[j] = 0;
					continue;
				}
				
				
				
				p0 = v0.x;
				p1 = v1.x;
				p2 = v2.x;
				
				r = e.x;
				
				u = p0;				
				if (p1 > u) u = p1;	
				if (p2 > u) u = p2;
				
				v = p0;
				if (p1 < v) v = p1;
				if (p2 < v) v = p2;
				
				if (fmax(-u, v) > r) {
					temp_primative_list[j] = 0;
					continue;
				}
				
				
				
				p0 = v0.y;
				p1 = v1.y;
				p2 = v2.y;
				
				r = e.y;
				
				u = p0;				
				if (p1 > u) u = p1;	
				if (p2 > u) u = p2;
				
				v = p0;
				if (p1 < v) v = p1;
				if (p2 < v) v = p2;
				
				if (fmax(-u, v) > r) {
					temp_primative_list[j] = 0;
					continue;
				}
				
				
				
				p0 = v0.z;
				p1 = v1.z;
				p2 = v2.z;
				
				r = e.z;
				
				u = p0;				
				if (p1 > u) u = p1;	
				if (p2 > u) u = p2;
				
				v = p0;
				if (p1 < v) v = p1;
				if (p2 < v) v = p2;
				
				if (fmax(-u, v) > r) {
					temp_primative_list[j] = 0;
					continue;
				}
				
				
				
				vec_t t_norm = vec_cross(f0, f1);
				
				p0 = vec_dot(v0, t_norm);
				p1 = vec_dot(v1, t_norm);
				p2 = vec_dot(v2, t_norm);
				
				r = e.x * abs(vec_dot(u0, t_norm)) + 
					e.y * abs(vec_dot(u1, t_norm)) + 
					e.z * abs(vec_dot(u2, t_norm));
				
				u = p0;				
				if (p1 > u) u = p1;	
				if (p2 > u) u = p2;
				
				v = p0;
				if (p1 < v) v = p1;
				if (p2 < v) v = p2;
				
				if (fmax(-u, v) > r) {
					temp_primative_list[j] = 0;
					continue;
				}
				
				temp_primative_list[j] = primative_list[j];
				
			}
			
			
			unsigned long count = 0;
			for(unsigned long j = 0; j < primative_count; j++) {
				if (temp_primative_list[j] == 0) {
					continue;
				} else {
					temp_primative_list[count] = temp_primative_list[j];
					count++;
				}
			}
			
			rtn->children[i] = create_bvh(temp_primative_list, count, tri_list, vec_list, &temp_min, &temp_max, limit, depth - 1);
			
			free(temp_primative_list);
			
		}
		
		char children = 0;
		for(unsigned long i = 0; i < 8; i++) {
			children |= rtn->children != NULL;
			if (children) break;
		}
		if (!children) {
			free(rtn->children);
			free(rtn);
			return NULL;
		}
		
		rtn->primative_list = NULL;
		rtn->primative_count = 0;
		
	} else {
		
		rtn->children = NULL;
		
		rtn->primative_list = (unsigned long *) malloc(sizeof(unsigned long) * primative_count);
		rtn->primative_count = primative_count;
		for(unsigned long i = 0; i < primative_count; i++) {
			rtn->primative_list[i] = primative_list[i];
		}
		
		
	}
	
	return rtn;
}

void destroy_bvh(BVH_t * bvh) {
	
	if (bvh == NULL) return;
	
	if(bvh->children != NULL) {
		for(unsigned long i = 0; i < 8; i++) {
			if (bvh->children[i] != NULL) destroy_bvh(bvh->children[i]);
		}
		free(bvh->children);
	}
	
	if (bvh->primative_list != NULL) free(bvh->primative_list);
	
	free(bvh);
}



// Returns Closest Hit Intersection
// Returns 1 on hit, 0 on miss

typedef struct {
	
	float dist;
	
	vec_t intersect;
	vec_t normal;
	
	triangle_t * tri;
	
	float u;
	float v;
	
} intersect_t;

char cast_ray_triangle(const vec_t * RayOrigin, const vec_t * RayDir, triangle_t * triangle, vec_t * vec_list, float t_close, float t_far, intersect_t * rtn) {
	
	/*
	
	Ray Triangle intersection:
	
	X = o + d*t
	
	(X - Ta) * N = 0
	
	((o + d*t) - Ta) *  N = 0
	
	((o - Ta) * N) + t*(d * N) = 0
	
	t = ((Ta - o) * N) / (d * N)
	
	*/
	
	vec_t * A = vec_list + triangle->vA;
	vec_t * B = vec_list + triangle->vB;
	vec_t * C = vec_list + triangle->vC;
	
	vec_t AB = (vec_t){(B->x - A->x),(B->y - A->y),(B->z - A->z)};
	vec_t AC = (vec_t){(C->x - A->x),(C->y - A->y),(C->z - A->z)};
	
	vec_t norm = (vec_t){
		(AB.y * AC.z) - (AB.z * AC.y), 
		(AB.z * AC.x) - (AB.x * AC.z), 
		(AB.x * AC.y) - (AB.y * AC.x)
		};
	float norm_mag = 1.0f / sqrt((norm.x * norm.x) + (norm.y * norm.y) + (norm.z * norm.z));
	norm.x *= norm_mag;
	norm.y *= norm_mag;
	norm.z *= norm_mag;
	
	float i = (RayDir->x * norm.x) + (RayDir->y * norm.y) + (RayDir->z * norm.z);
	
	// No hit, Ray is parallel to triangle.
	if (i == 0.0) return 0;
	
	// Wrong side of the Triangle
	if (i > 0.0) return 0; 
	
	float t = (((A->x - RayOrigin->x) * norm.x) + ((A->y - RayOrigin->y) * norm.y) + ((A->z - RayOrigin->z) * norm.z)) / i;
	
	if (t < t_close || t > t_far || t > rtn->dist) {
		return 0;
	}
	
	// Determine U and V values
	
	/*
	
	X = Ta + Tab * u + Tac * v
	
	X - Ta = Tab * u + Tac * v
	
	(X - Ta) * Tab = (Tab * Tab) * u + (Tab * Tac) * v	
	(X - Ta) * Tac = (Tab * Tac) * u + (Tac * Tac) * v
	
	d00 = Tab * Tab
	d01 = Tab * Tac
	d02 = (X - Ta) * Tab
	
	d11 = Tac * Tac
	d12 = (X - Ta) * Tac
	
	
	d02 = d00 * u + d01 * v			d12 = d01 * u + d11 * v
	
	u = (d02 - d01 * v) / d00		u = (d12 - d11 * v) / d01
	v = (d02 - d00 * u) / d01		v = (d12 - d01 * u) / d11
	
	u = u
	
	(d02 - d01 * v) * d01 = (d12 - d11 * v) * d00
	
	(d02 * d01) - (d01 * d01) * v = (d00 * d12) - (d00 * d11) * v
	
	v * ((d00 * d11) - (d01 * d01)) = (d00 * d12) - (d02 * d01)
	
	v = ((d00 * d12) - (d02 * d01)) / ((d00 * d11) - (d01 * d01))
	
	
	v = v
	
	(d02 - d00 * u) * d11 = (d12 - d01 * u) * d01
	
	(d02 * d11) - (d00 * d11) * u = (d01 * d12) - (d01 * d01) * u
	
	u * ((d01 * d01) - (d00 * d11)) = (d01 * d12) - (d02 * d11)
	
	u = ((d01 * d12) - (d02 * d11)) / ((d01 * d01) - (d00 * d11))
	
	u = ((d02 * d11) - (d01 * d12)) / ((d00 * d11) - (d01 * d01))
	
	
	*/
	
	vec_t X = (vec_t){
		RayOrigin->x + (RayDir->x * t), 
		RayOrigin->y + (RayDir->y * t), 
		RayOrigin->z + (RayDir->z * t)
		};
	vec_t dX = (vec_t){
		X.x - A->x, 
		X.y - A->y, 
		X.z - A->z
		};
	
	float d00 = (AB.x * AB.x) + (AB.y * AB.y) + (AB.z * AB.z);
	float d01 = (AB.x * AC.x) + (AB.y * AC.y) + (AB.z * AC.z);
	float d02 = (AB.x * dX.x) + (AB.y * dX.y) + (AB.z * dX.z);
	float d11 = (AC.x * AC.x) + (AC.y * AC.y) + (AC.z * AC.z);
	float d12 = (AC.x * dX.x) + (AC.y * dX.y) + (AC.z * dX.z);
	
	float invDenom = 1.0 / ((d01 * d01) - (d00 * d11));
	
	float u = ((d12 * d01) - (d02 * d11)) * invDenom;
	float v = ((d02 * d01) - (d12 * d00)) * invDenom;
	
	// If plane intersection is outside triangle: no hit.
	if (u < 0.0f || v < 0.0f || u + v > 1.0f) {
		return 0;
	}
	
	
	// Hit triangle! Quick check to prevent overwrite a better intersection
	if (rtn->dist > t) {
		rtn->dist = t;
		rtn->intersect = X;
		rtn->normal = norm;
		rtn->tri = triangle;
		rtn->u = u;
		rtn->v = v;
		
		return 1;
	}
	
	return 0;
}


int cast_ray_bvh(const vec_t * RayOrigin, const vec_t * RayDir, const BVH_t * bvh, triangle_t * tri_list, vec_t * vec_list, float t_close, float t_far, intersect_t * rtn) {
	
	if (bvh == NULL) return 0;
	
	float tx1 = (bvh->min.x - RayOrigin->x) / RayDir->x;
	float tx2 = (bvh->max.x - RayOrigin->x) / RayDir->x;
	
	float tx3 = (bvh->mid.x - RayOrigin->x) / RayDir->x;
	
	float ty1 = (bvh->min.y - RayOrigin->y) / RayDir->y;
	float ty2 = (bvh->max.y - RayOrigin->y) / RayDir->y;
	
	float ty3 = (bvh->mid.y - RayOrigin->y) / RayDir->y;
	
	float tz1 = (bvh->min.z - RayOrigin->z) / RayDir->z;
	float tz2 = (bvh->max.z - RayOrigin->z) / RayDir->z;
	
	float tz3 = (bvh->mid.z - RayOrigin->z) / RayDir->z;
	
	
	if (tx1 > tx2) {
		float tmp = tx1;
		tx1 = tx2;
		tx2 = tmp;
	}
	
	if (ty1 > ty2) {
		float tmp = ty1;
		ty1 = ty2;
		ty2 = tmp;
	}
	
	if (tz1 > tz2) {
		float tmp = tz1;
		tz1 = tz2;
		tz2 = tmp;
	}
	
	
	float tmp_min = tx1;
	if (tmp_min < ty1) tmp_min = ty1;
	if (tmp_min < tz1) tmp_min = tz1;
	
	float tmp_max = tx2;
	if (tmp_max > ty2) tmp_max = ty2;
	if (tmp_max > tz2) tmp_max = tz2;
	
	if (tmp_min > tmp_max || tmp_max < 0.0f || t_close > tmp_max || t_far < tmp_min) return 0;
	
	if (bvh->children != NULL) {
		
		unsigned char index = (((RayDir->z > 0.0f) ^ (tz3 > tmp_min))<<2) | 
							  (((RayDir->y > 0.0f) ^ (ty3 > tmp_min))<<1) | 
							  (((RayDir->x > 0.0f) ^ (tx3 > tmp_min))<<0);
		
		int depth;
		
		depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
		if (depth > 0) return 1 + depth;
		
		
		unsigned char order = ((ty3 < tz3) << 2) | ((tx3 < tz3) << 1) | (tx3 < ty3);
		
		switch (order) {
			case 0:	// Z > Y > X
				if (tz3 > tmp_max) break;
				if (tz3 > tmp_min) {
					index ^= 4;
					depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}

				if (ty3 > tmp_max) break;
				if (ty3 > tmp_min) {
					index ^= 2;
					depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}

				if (tx3 > tmp_max) break;
				if (tx3 > tmp_min) {
					index ^= 1;
					depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				break;
				
			case 1: // Z > X > Y
				if (tz3 > tmp_max) break;
				if (tz3 > tmp_min) {
					index ^= 4;
					depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				
				if (tx3 > tmp_max) break;
				if (tx3 > tmp_min) {
					index ^= 1;
					depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				
				if (ty3 > tmp_max) break;
				if (ty3 > tmp_min) {
					index ^= 2;
					depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				break;
				
			case 2: // Invalid
				break;
				
			case 3: // X > Z > Y
				if (tx3 > tmp_max) break;
				if (tx3 > tmp_min) {
					index ^= 1;
					depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				
				if (tz3 > tmp_max) break;
				if (tz3 > tmp_min) {
					index ^= 4;
					depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				
				if (ty3 > tmp_max) break;
				if (ty3 > tmp_min) {
					index ^= 2;
					depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				
				break;
				
			case 4: // Y > Z > X
				if (ty3 > tmp_max) break;
				if (ty3 > tmp_min) {
					index ^= 2;
					depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				
				if (tz3 > tmp_max) break;
				if (tz3 > tmp_min) {
					index ^= 4;
					depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				
				if (tx3 > tmp_max) break;
				if (tx3 > tmp_min) {
					index ^= 1;
					depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				break;
				
			case 5: // Invalid
				break;
				
			case 6: // Y > X > Z
				if (ty3 > tmp_max) break;
				if (ty3 > tmp_min) {
					index ^= 2;
					depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				
				if (tx3 > tmp_max) break;
				if (tx3 > tmp_min) {
					index ^= 1;
					depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				
				if (tz3 > tmp_max) break;
				if (tz3 > tmp_min) {
					index ^= 4;
					depth = cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				
				break;
				
			case 7: // X > Y > Z
				if (tx3 > tmp_max) break;
				if (tx3 > tmp_min) {
					index ^= 1;
					depth += cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				
				if (ty3 > tmp_max) break;
				if (ty3 > tmp_min) {
					index ^= 2;
					depth += cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				
				if (tz3 > tmp_max) break;
				if (tz3 > tmp_min) {
					index ^= 4;
					depth += cast_ray_bvh(RayOrigin, RayDir, bvh->children[index], tri_list, vec_list, t_close, t_far, rtn);
					if (depth > 0) return 1 + depth;
				}
				
				break;
				
			default:
				break;
		}
		
	} else {
		
		int hit = 0;
		
		for(unsigned long i = 0; i < bvh->primative_count; i++) {
			
			hit |= cast_ray_triangle(RayOrigin, RayDir, tri_list + bvh->primative_list[i], vec_list, t_close, t_far, rtn);
			
		}
		
		return hit;
		
	} 
	
	return 0;
}


// Returns a color. Recursive to call reflections and refractions.
void trace_ray(const vec_t * RayOrigin, const vec_t * RayDir, const BVH_t * BVH, triangle_t * tri_list, vec_t * vec_list, texel_t * tex_list, const light_t * light_list, const unsigned long light_count, const long ttl, color_t * rtn_color) {
	
	if (ttl <= 0) {
		*rtn_color = (color_t){0.0f, 0.0f, 0.0f, 0.0f};
		return;
	} else {
		
		intersect_t I;
		I.dist = INFINITY;
		
		int depth = cast_ray_bvh(RayOrigin, RayDir, BVH, tri_list, vec_list, 0.0, INFINITY, &I);
		
		if (depth <= 0) {
			// No Hit
			*rtn_color = (color_t){0.0f, 0.0f, 0.0f, 0.0f};
			return;
		} else {
			// Determine color, along with reflect and refract.
			// Return the color
			
			// Light Amount * Surface Color Values = rtn Color
			// Light Amount = (Sum Direct Light) + Reflect + Refraction
			// (Sum Direct Light + Reflect + Refraction) * Surface Color = rtn Color
			
			
			// TODO::THIS //
			
			//vec_t ReflectDir;
			//vec_t RefractDir;
			
			//color_t Reflect;
			//color_t Refract;
			
			//trace_ray(&I.intersect, &ReflectDir, BVH, tri_list, vec_list, tex_list, light_list, light_count, ttl - 1, &Reflect);
			//trace_ray(&I.intersect, &RefractDir, BVH, tri_list, vec_list, tex_list, light_list, light_count, ttl - 1, &Refract);
			
			
			
			// Sum of all direct lighting
			//color_t DirectLight;
			
			// For every light:
			// Cast ray from intersect point to light
			// If it misses everything
			// Add Light based on Light intensity, Distance, and angle to surface.
			
			
			
			// Color of triangle surface
			//color_t surfaceColor;
			// texel_t * tA = texel_list + triangle.tA
			// texel_t * tB = texel_list + triangle.tA
			// texel_t * tC = texel_list + triangle.tA
			// texel_t dAB = tB - tA;
			// texel_t dAC = tB - tA;
			// texel_t final = tA + I.u * tAB + I.v * tAC
			// poll Texture at final
			
			
			
			//*rtn_color = (color_t){
			//	(DirectLight.r + Reflect.r + Refract.r) * surfaceColor.r, 
			//	(DirectLight.g + Reflect.g + Refract.g) * surfaceColor.g, 
			//	(DirectLight.b + Reflect.b + Refract.b) * surfaceColor.b,
			//  1.0f // Alpha
			//	};
			
			
			float s = vec_dot(I.normal, *RayDir);
			s = abs(s);
			
			*rtn_color = (color_t){s, s, s, 1.0f};
		}
		
	}
	
}



unsigned int color_to_int(color_t color) {
	
	// A B G R
	
	return (((unsigned int)floor(color.a * 255.0f) & 0xFF) << 24) | 
		   (((unsigned int)floor(color.b * 255.0f) & 0xFF) << 16) | 
		   (((unsigned int)floor(color.g * 255.0f) & 0xFF) << 8) | 
		   (((unsigned int)floor(color.r * 255.0f) & 0xFF) << 0);
	
}

void render_image(const Camera_t * Camera, const model_t * model, unsigned int * image) {
	
	unsigned long ttl = 12;
	unsigned long limit = 8;
	unsigned long depth = 12;
	
	// Make a BVH using model
	unsigned long temp_primative_count = model->tri_count;
	unsigned long * temp_primative_list = (unsigned long *) malloc(sizeof(unsigned long) * temp_primative_count);
	for(unsigned long i = 0; i < temp_primative_count; i++) {
		temp_primative_list[i] = i;
	}
	BVH_t * bvh = create_bvh(temp_primative_list, temp_primative_count, model->tri_list, model->vec_list, NULL, NULL, limit, depth);
	free(temp_primative_list);
	
	
	#pragma omp parallel for collapse(2)
	for(unsigned long y = 0; y < Camera->vRES; y++) {
		for(unsigned long x = 0; x < Camera->hRES; x++) {
			
			float Pitch = (Camera->vFOV / 2.0f) - (Camera->vFOV / (float)Camera->vRES) * y;
			float Yaw = (Camera->hFOV / 2.0f) - (Camera->hFOV / (float)Camera->hRES) * x;
			
			vec_t axis = vec_add(vec_scale(Camera->up, Yaw), vec_scale(Camera->right, Pitch));
			float im = 1.0f / sqrt(vec_dot(axis, axis));
			axis = vec_scale(axis, im);
			
			vec_t DIR = vec_rotate(Camera->look, axis, sqrt(Yaw*Yaw + Pitch*Pitch));
			
			color_t color = (color_t){0.0, 0.0, 0.0, 0.0};
			trace_ray(&Camera->pos, &DIR, bvh, model->tri_list, model->vec_list, model->tex_list, NULL, 0, ttl, &color);
			
			image[x + y * Camera->hRES] = color_to_int(color);
			
		}
	}
	
	
	destroy_bvh(bvh);
}







int main() {
	
	printf("Hello, World!\n");
	
	
	// Make Camera:
	Camera_t Camera;
	Camera.pos =	(vec_t){ -2.0f,  0.5f,   0.0f};
	Camera.look =	(vec_t){  1.0f,  0.0f,   0.0f};
	Camera.up =		(vec_t){  0.0f,  1.0f,   0.0f};
	Camera.right = 	(vec_t){  0.0f,  0.0f,   1.0f};
	
	Camera.hFOV = M_PI * 0.5f;
	Camera.vFOV = M_PI * 0.5f;
	
	Camera.hRES = 256;
	Camera.vRES = 256;
	
	float lookSpeed = M_PI / 32.0f;
	float moveSpeed = 0.1f;
	
	
	// Make Environment:
	model_t * E = load_model("bunny.obj");
	
	
	
	
	
	
	// Initilize SDL
	
	unsigned long window_width = 512;
	unsigned long window_height = 512;
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Failed to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
	
	
	SDL_Window * MainWindow = SDL_CreateWindow("Ray Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, 0);
	if (!MainWindow) {
		printf("Failed to create SDL Window: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	
	unsigned long render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	SDL_Renderer * MainRenderer = SDL_CreateRenderer(MainWindow, -1, render_flags);
	if (!MainRenderer) {
		printf("Failed to create Renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(MainWindow);
		SDL_Quit();
		return 1;
	}
	
	unsigned int * PIX_BUF;
	int PITCH;
	SDL_Texture * TexBuff = SDL_CreateTexture(MainRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, Camera.hRES, Camera.vRES);
	SDL_SetTextureBlendMode(TexBuff, SDL_BLENDMODE_NONE);
	SDL_LockTexture(TexBuff, NULL, (void**)&PIX_BUF, &PITCH);
	
	// Done SDL init
	
	
	
	
	// Main Loop variables
	char loop = 1;
	
	unsigned long look_up		(1 << 0);
	unsigned long look_down		(1 << 1);
	unsigned long look_right	(1 << 2);
	unsigned long look_left		(1 << 3);
	unsigned long roll_clock	(1 << 4);
	unsigned long roll_counter	(1 << 5);
	
	unsigned long move_up		(1 << 6);
	unsigned long move_down		(1 << 7);
	unsigned long move_forward	(1 << 8);
	unsigned long move_back		(1 << 9);
	unsigned long move_left		(1 << 10);
	unsigned long move_right	(1 << 11);
	
	unsigned long user_input = 0;
	
	unsigned long frame_count = 0;
	
	while (loop) {
		// Input Handling
		
		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
				case SDL_QUIT:
					loop = 0;
					break;
				case SDL_KEYDOWN:
					
					switch (evt.key.keysym.sym) {
						case SDLK_LEFT:
							user_input |= look_left;
							break;
						case SDLK_RIGHT:
							user_input |= look_right;
							break;
						case SDLK_UP:
							user_input |= look_up;
							break;
						case SDLK_DOWN:
							user_input |= look_down;
							break;
						case SDLK_e:
							user_input |= roll_clock;
							break;
						case SDLK_q:
							user_input |= roll_counter;
							break;
							
						case SDLK_w:
							user_input |= move_forward;
							break;
						case SDLK_a:
							user_input |= move_left;
							break;
						case SDLK_s:
							user_input |= move_back;
							break;
						case SDLK_d:
							user_input |= move_right;
							break;
						case SDLK_SPACE:
							user_input |= move_up;
							break;
						case SDLK_LSHIFT:
							user_input |= move_down;
							break;
						
						default:
							break;
					}
					
					break;
				case SDL_KEYUP:
					
					switch(evt.key.keysym.sym) {
						case SDLK_LEFT:
							user_input &= !look_left;
							break;
						case SDLK_RIGHT:
							user_input &= !look_right;
							break;
						case SDLK_UP:
							user_input &= !look_up;
							break;
						case SDLK_DOWN:
							user_input &= !look_down;
							break;
						case SDLK_e:
							user_input &= !roll_clock;
							break;
						case SDLK_q:
							user_input &= !roll_counter;
							break;
							
						case SDLK_w:
							user_input &= !move_forward;
							break;
						case SDLK_a:
							user_input &= !move_left;
							break;
						case SDLK_s:
							user_input &= !move_back;
							break;
						case SDLK_d:
							user_input &= !move_right;
							break;
						case SDLK_SPACE:
							user_input &= !move_up;
							break;
						case SDLK_LSHIFT:
							user_input &= !move_down;
							break;
							
						default:
							break;
					}
					
					break;
				default:
					break;
			}
		}
		
		// Look Around
		
		unsigned int tmp;
		
		vec_t axis = (vec_t){0.0f, 0.0f, 0.0f};
		tmp = user_input & look_up;
		if (tmp) {
			axis.x += Camera.right.x;
			axis.y += Camera.right.y;
			axis.z += Camera.right.z;
		}
		tmp = user_input & look_down;
		if (tmp) {
			axis.x -= Camera.right.x;
			axis.y -= Camera.right.y;
			axis.z -= Camera.right.z;
		}
		tmp = user_input & look_left;
		if (tmp) {
			axis.x += Camera.up.x;
			axis.y += Camera.up.y;
			axis.z += Camera.up.z;
		}
		tmp = user_input & look_right;
		if (tmp) {
			axis.x -= Camera.up.x;
			axis.y -= Camera.up.y;
			axis.z -= Camera.up.z;
		}
		tmp = user_input & roll_clock;
		if (tmp) {
			axis.x += Camera.look.x;
			axis.y += Camera.look.y;
			axis.z += Camera.look.z;
		}
		tmp = user_input & roll_counter;
		if (tmp) {
			axis.x -= Camera.look.x;
			axis.y -= Camera.look.y;
			axis.z -= Camera.look.z;
		}
		float axis_mag = vec_dot(axis, axis);
		if (axis_mag > 0.0f) {
			axis = vec_scale(axis, 1.0f / sqrt(axis_mag));
			
			Camera.look = vec_rotate(Camera.look, axis, lookSpeed);
			Camera.up= vec_rotate(Camera.up, axis, lookSpeed);
			Camera.right = vec_rotate(Camera.right, axis, lookSpeed);
			
		}
		
		// Move
		vec_t move = (vec_t) {0.0f, 0.0f, 0.0f};
		tmp = user_input & move_forward;
		if (tmp) {
			move.x += Camera.look.x;
			move.y += Camera.look.y;
			move.z += Camera.look.z;
		}
		tmp = user_input & move_back;
		if (tmp) {
			move.x -= Camera.look.x;
			move.y -= Camera.look.y;
			move.z -= Camera.look.z;
		}
		tmp = user_input & move_right;
		if (tmp) {
			move.x += Camera.right.x;
			move.y += Camera.right.y;
			move.z += Camera.right.z;
		}
		tmp = user_input & move_left;
		if (tmp) {
			move.x -= Camera.right.x;
			move.y -= Camera.right.y;
			move.z -= Camera.right.z;
		}
		tmp = user_input & move_up;
		if (tmp) {
			move.x += Camera.up.x;
			move.y += Camera.up.y;
			move.z += Camera.up.z;
		}
		tmp = user_input & move_down;
		if (tmp) {
			move.x -= Camera.up.x;
			move.y -= Camera.up.y;
			move.z -= Camera.up.z;
		}
		float move_mag = vec_dot(move, move);
		if (move_mag > 0.0f) {
			move = vec_scale(move, moveSpeed / sqrt(move_mag));
			
			Camera.pos.x += move.x;
			Camera.pos.y += move.y;
			Camera.pos.z += move.z;
		}
		
		
		/*
		// Clear Screen
		#pragma omp parallel for
		for(unsigned long z = 0; z < Camera.hRES * Camera.vRES; z++) {
			PIX_BUF[z] = 0x00000000;
		}
		*/
		
		// render
		render_image(&Camera, E, PIX_BUF);
		printf("Frame!\n");
		
		
		// Update Screen
		SDL_UnlockTexture(TexBuff);
		SDL_RenderCopy(MainRenderer, TexBuff, NULL, NULL);
		SDL_RenderPresent(MainRenderer);
		SDL_LockTexture(TexBuff, NULL, (void **)&PIX_BUF, &PITCH);
		
		frame_count++;
//		if (frame_count >= 10) loop = 0;
		
	}
	
	// Cleanup
	
	SDL_UnlockTexture(TexBuff);
	SDL_DestroyTexture(TexBuff);
	SDL_DestroyRenderer(MainRenderer);
	SDL_DestroyWindow(MainWindow);
	SDL_Quit();
	
	// Destroy Environment
	destroy_model(E);
	
	// Destroy Camera
	// Nothing to do.
	
	
	
	
	return 0;	
}
