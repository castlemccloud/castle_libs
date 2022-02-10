

#include "raster.h"

#include <stdio.h>
#include <string.h>


float vec3_dot(const vec3_t A, const vec3_t B) {
	return (A.x * B.x) + (A.y * B.y) + (A.z * B.z);
}

vec3_t vec3_cross(const vec3_t A, const vec3_t B) {
	return (vec3_t){((A.y * B.z) - (A.z * B.y)), 
				   ((A.z * B.x) - (A.x * B.z)), 
				   ((A.x * B.y) - (A.y * B.x))};
}

vec3_t vec3_scale(const vec3_t A, float s) {
	return (vec3_t){A.x * s, A.y * s, A.z * s};
}

vec3_t vec3_add(const vec3_t A, const vec3_t B) {
	return (vec3_t){A.x + B.x, A.y + B.y, A.z + B.z};
}

vec3_t vec3_rotate(const vec3_t A, const vec3_t B, float t) {
	// Ru(t)x = u * DOT(u, x) + cos(t) * CROSS(CROSS(u, x), u) + sin(t) * CROSS(u, x)
	
	vec3_t w = vec3_cross(B, A);
	vec3_t v = vec3_cross(w, B);
	
	vec3_t para = vec3_scale(B, vec3_dot(A, B));
	
	vec3_t vsin = vec3_scale(w, sin(t));
	
	vec3_t vcos = vec3_scale(v, cos(t));
	
	return vec3_add(para, vec3_add(vsin, vcos));
	
}




texture_t * load_texture(const char * texture_name) {
	
	texture_t * rtn = (texture_t *) malloc(sizeof(texture_t));
	
	int width, height, texChannels;
	
	unsigned char * pixels = stbi_load(texture_name, &(width), &(height), &(texChannels), STBI_rgb_alpha);
	
	rtn->width = width;
	rtn->height = height;
	rtn->pixels = (unsigned int *) pixels;
	
	for(unsigned long z = 0; z < (rtn->height * rtn->width); z++) {
		
		// Change from RGBA to ARGB
		
		unsigned int A = pixels[(z * 4) + 3];
		unsigned int R = pixels[(z * 4) + 0];
		unsigned int G = pixels[(z * 4) + 1];
		unsigned int B = pixels[(z * 4) + 2];
		
		rtn->pixels[z] = argb_to_uint(A, R, G, B);
	}
	
	return rtn;
}

void destroy_texture(texture_t * texture) {
	free(texture->pixels);
	free(texture);
}


unsigned int sample_texture(texture_t * texture, float u, float v) {
	
	v = 1.0 - v;
	
	float x = u * (float)texture->width;
	float y = v * (float)texture->height;
	
	unsigned long ind_x = (unsigned long) x;
	unsigned long ind_y = (unsigned long) y;
	
	float dx = x - ind_x;
	float dy = y - ind_y;
	
	unsigned int * A = texture->pixels + ind_x       + (ind_y * texture->width);
	unsigned int * B = texture->pixels + (ind_x + 1) + (ind_y * texture->width);
	unsigned int * C = texture->pixels + ind_x       + ((ind_y + 1) * texture->width);
	unsigned int * D = texture->pixels + (ind_x + 1) + ((ind_y + 1) * texture->width);
	
	unsigned char A_a = ((unsigned char *)A) [3];
	unsigned char A_r = ((unsigned char *)A) [2];
	unsigned char A_g = ((unsigned char *)A) [1];
	unsigned char A_b = ((unsigned char *)A) [0];
	
	unsigned char B_a = ((unsigned char *)B) [3];
	unsigned char B_r = ((unsigned char *)B) [2];
	unsigned char B_g = ((unsigned char *)B) [1];
	unsigned char B_b = ((unsigned char *)B) [0];
	
	unsigned char C_a = ((unsigned char *)C) [3];
	unsigned char C_r = ((unsigned char *)C) [2];
	unsigned char C_g = ((unsigned char *)C) [1];
	unsigned char C_b = ((unsigned char *)C) [0];
	
	unsigned char D_a = ((unsigned char *)D) [3];
	unsigned char D_r = ((unsigned char *)D) [2];
	unsigned char D_g = ((unsigned char *)D) [1];
	unsigned char D_b = ((unsigned char *)D) [0];
	
	unsigned int rtn_a = (unsigned int) ((A_a * (1.0f - dx) * (1.0f - dy)) + (B_a * (dx) * (1.0f - dy)) + (C_a * (1.0f - dx) * (dy)) + (D_a * (dx) * (dy)));
	unsigned int rtn_r = (unsigned int) ((A_r * (1.0f - dx) * (1.0f - dy)) + (B_r * (dx) * (1.0f - dy)) + (C_r * (1.0f - dx) * (dy)) + (D_r * (dx) * (dy)));
	unsigned int rtn_g = (unsigned int) ((A_g * (1.0f - dx) * (1.0f - dy)) + (B_g * (dx) * (1.0f - dy)) + (C_g * (1.0f - dx) * (dy)) + (D_g * (dx) * (dy)));
	unsigned int rtn_b = (unsigned int) ((A_b * (1.0f - dx) * (1.0f - dy)) + (B_b * (dx) * (1.0f - dy)) + (C_b * (1.0f - dx) * (dy)) + (D_b * (dx) * (dy)));
	
	return argb_to_uint(rtn_a, rtn_r, rtn_g, rtn_b);
	
}



model_t * load_model(const char * model_name) {
	
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_name, NULL, true)) {
		throw std::runtime_error(warn + err);
	}
	
	unsigned long vec_count = attrib.vertices.size() / 3;
	vec3_t * vec_list = (vec3_t *) malloc(sizeof(vec3_t) * vec_count);
	memcpy(vec_list, attrib.vertices.data(), sizeof(vec3_t) * vec_count);
	
	unsigned long tex_count = attrib.texcoords.size() / 2;
	vec2_t * tex_list = (vec2_t *) malloc(sizeof(vec2_t) * tex_count);
	memcpy(tex_list, attrib.texcoords.data(), sizeof(vec2_t) * tex_count);
	
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
			
			tri_list[tri_index] = (triangle_t){
				(unsigned long)iA.vertex_index, (unsigned long)iB.vertex_index, (unsigned long)iC.vertex_index, 
				(unsigned long)iA.texcoord_index, (unsigned long)iB.texcoord_index, (unsigned long)iC.texcoord_index};
			
			tri_index++;
		}
		
	}
	
	
	
	
	model_t * rtn = (model_t *) malloc(sizeof(model_t));
	rtn->vec_count = vec_count;
	rtn->vec_list = vec_list;
	rtn->tex_count = tex_count;
	rtn->tex_list = tex_list;
	rtn->tri_count = tri_count;
	rtn->tri_list = tri_list;
	
	return rtn;
	
}

void destroy_model(model_t * model) {
	
	free(model->vec_list);
	free(model->tex_list);
	free(model->tri_list);
	free(model);
	
}









typedef struct {
	
	float x, y, z;
	float u, v;
	
} vertex_t;


typedef struct {
	
	vertex_t A, B, C;
	
} render_target_triangle_t;

void draw_triangle(unsigned int * pix_buf, float * depth_buf, long hor_res, long ver_res, render_target_triangle_t tri, texture_t * texture) {
	
	vertex_t A = tri.A;
	vertex_t B = tri.B;
	vertex_t C = tri.C;
	
	A.x += 1.0f;
	A.x *= (0.5f * hor_res);
	
	A.y += 1.0f;
	A.y *= (0.5f * ver_res);
	
	
	
	B.x += 1.0f;
	B.x *= (0.5f * hor_res);
	
	B.y += 1.0f;
	B.y *= (0.5f * ver_res);
	
	
	
	C.x += 1.0f;
	C.x *= (0.5f * hor_res);
	
	C.y += 1.0f;
	C.y *= (0.5f * ver_res);
	
	
	
	if (A.y > B.y) {
		vertex_t tmp = A;
		A = B;
		B = tmp;
	}
	
	if (A.y > C.y) {
		vertex_t tmp = A;
		A = C;
		C = tmp;
	}
	
	if (B.y > C.y) {
		vertex_t tmp = B;
		B = C;
		C = tmp;
	}
	
	float AB_dx = B.x - A.x;
	float AB_dy = B.y - A.y;
	float AB_dz = B.z - A.z;
	
	float AC_dx = C.x - A.x;
	float AC_dy = C.y - A.y;
	float AC_dz = C.z - A.z;
	
	float BC_dx = C.x - B.x;
	float BC_dy = C.y - B.y;
//	float BC_dz = C.z - A.z;
	
	float Nx = (AB_dy * AC_dz) - (AB_dz * AC_dy);
	float Ny = (AB_dz * AC_dx) - (AB_dx * AC_dz);
	float Nz = (AB_dx * AC_dy) - (AB_dy * AC_dx);
	
	float AB_m = AB_dx / AB_dy;
	float AC_m = AC_dx / AC_dy;
	float BC_m = BC_dx / BC_dy;
	
	float AB_b = A.x - A.y * AB_m;
	float AC_b = A.x - A.y * AC_m;
	float BC_b = B.x - B.y * BC_m;
	
	float P = (Nx * A.x) + (Ny * A.y) + (Nz * A.z);
	
	float PNz = P / Nz;
	float NxNz = Nx / Nz;
	float NyNz = Ny / Nz;
	
	
	
	float idenom = 1.0f / ((AB_dy * AC_dx) - (AB_dx * AC_dy));
	
	float u_x_factor = - AC_dy * idenom;
	float u_y_factor = AC_dx * idenom;
	
	float v_x_factor = AB_dy * idenom;
	float v_y_factor = - AB_dx * idenom;
	
	float u_factor = ((A.x * AC_dy) - (A.y * AC_dx)) * idenom;
	float v_factor = ((A.y * AB_dx) - (A.x * AB_dy)) * idenom;
	
	
	
	unsigned char bend_left = (Nz < 0.0f);
	
	float start_y = A.y;
	float end_y = C.y;
	
	
	for(float y = start_y; y < end_y; y += 1.0f) {
		
		float start_x;
		float end_x;
		
		if (y < B.y) {
			
			if (bend_left) {
				
				start_x = (y * AB_m) + AB_b;
				end_x = (y * AC_m) + AC_b;
				
			} else {
				
				start_x = (y * AC_m) + AC_b;
				end_x = (y * AB_m) + AB_b;
				
			}
			
		} else {
			
			if (bend_left) {
				
				start_x = (y * BC_m) + BC_b;
				end_x = (y * AC_m) + AC_b;
				
			} else {
				
				start_x = (y * AC_m) + AC_b;
				end_x = (y * BC_m) + BC_b;
				
			}
		}
		
		for(float x = start_x; x < end_x; x += 1.0f) {
			
			float z = (PNz - ((NxNz * x) + (NyNz * y)));
			
			long ind_x = (long) x;
			long ind_y = (long) y;
			long index = ind_x + (ind_y * hor_res);
			
			if (depth_buf[index] != NAN && z > depth_buf[index]) continue;
			
			float u = (y * u_y_factor) + (x * u_x_factor) + u_factor;
			float v = (x * v_x_factor) + (y * v_y_factor) + v_factor;
			
			depth_buf[index] = z;
			
			float tex_u = ((1.0f - (u + v)) * A.u) + (u * B.u) + (v * C.u);
			float tex_v = ((1.0f - (u + v)) * A.v) + (u * B.v) + (v * C.v);
			
			pix_buf[index] = sample_texture(texture, tex_u, tex_v);
			
		}
		
	}
	
}



vertex_t lerp_vertex(vertex_t A, vertex_t B, float u) {
	
	float v = 1.0 - u;
	
	return (vertex_t) {
		(A.x * v) + (B.x * u),
		(A.y * v) + (B.y * u),
		(A.z * v) + (B.z * u),
		(A.u * v) + (B.u * u),
		(A.v * v) + (B.v * u)
	};
}



unsigned char clip_triangle(render_target_triangle_t * tri, vec3_t O, vec3_t N) {
	
	vertex_t A = tri[0].A;
	vertex_t B = tri[0].B;
	vertex_t C = tri[0].C;
	
	vec3_t OA = (vec3_t) {O.x - A.x, O.y - A.y, O.z - A.z};
	vec3_t OB = (vec3_t) {O.x - B.x, O.y - B.y, O.z - B.z};
	vec3_t OC = (vec3_t) {O.x - C.x, O.y - C.y, O.z - C.z};
	
	float OA_N = ((OA.x * N.x) + (OA.y * N.y) + (OA.z * N.z));
	float OB_N = ((OB.x * N.x) + (OB.y * N.y) + (OB.z * N.z));
	float OC_N = ((OC.x * N.x) + (OC.y * N.y) + (OC.z * N.z));
	
	unsigned char state = ((OA_N <= 0.0) << 0) | 
		((OB_N <= 0.0) << 1) | 
		((OC_N <= 0.0) << 2);
	
	float u = OA_N / (((B.x - A.x) * N.x) + ((B.y - A.y) * N.y) + ((B.z - A.z) * N.z));
	float v = OA_N / (((C.x - A.x) * N.x) + ((C.y - A.y) * N.y) + ((C.z - A.z) * N.z));
	float w = OB_N / (((C.x - B.x) * N.x) + ((C.y - B.y) * N.y) + ((C.z - B.z) * N.z));
	
	vertex_t v_U = lerp_vertex(A, B, u);
	vertex_t v_V = lerp_vertex(A, C, v);
	vertex_t v_W = lerp_vertex(B, C, w);
	
	
	switch (state) {
	
		case 0:
			return 0;
		
		case 1:
			tri[0].B = v_U;
			tri[0].C = v_V;
			return 1;
		
		case 2:
			tri[0].A = v_U;
			tri[0].C = v_W;
			return 1;
		
		case 3:
			tri[0].C = v_W;
			tri[1].B = v_W;
			tri[1].C = v_V;
			return 2;
		
		case 4:
			tri[0].A = v_V;
			tri[0].B = v_W;
			return 1;
		
		case 5:
			tri[0].B = v_W;
			tri[1].B = v_U;
			tri[1].C = v_W;
			return 2;
		
		case 6:
			tri[0].A = v_U;
			tri[1].A = v_V;
			tri[1].B = v_U;
			return 2;
		
		case 7:
			return 1;
		
		default:
			return 0;

		}
	
	return 0;
}



void render_model(unsigned int * pix_buf, float * depth_buf, camera_t camera, model_t * model, texture_t * texture) {
	
	
	vec3_t * vec_list = (vec3_t *) malloc(sizeof(vec3_t) * model->vec_count);
	memcpy(vec_list, model->vec_list, sizeof(vec3_t) * model->vec_count);
	
	
	float AR = (float) camera.hor_res / (float) camera.ver_res;
	float FOV = camera.fov;
	float FAR = camera.far;
	float NEAR = camera.near;
	
	for(unsigned long i = 0; i < model->vec_count; i++) {
		
		float x = vec_list[i].x;
		float y = vec_list[i].y;
		float z = vec_list[i].z;
		
		// Translate Based on Camera Position:
		
		float x_1 = x - camera.pos.x;
		float y_1 = y - camera.pos.y;
		float z_1 = z - camera.pos.z;
		
		// Rotate based on camera orientation:
		
		float x_2 = (x_1 * camera.right.x) + (y_1 * camera.right.y) + (z_1 * camera.right.z);
		float y_2 = (x_1 * camera.up.x) + (y_1 * camera.up.y) + (z_1 * camera.up.z);
		float z_2 = (x_1 * camera.look.x) + (y_1 * camera.look.y) + (z_1 * camera.look.z);
		
		// Perform Perspective Projection
		
		float x_3 = AR * FOV * x_2;
		float y_3 = FOV * y_2;
		float z_3 = (z_2 - NEAR) / (FAR - NEAR);
		
		float x_4 = x_3 / abs(z_2);
		float y_4 = y_3 / abs(z_2);
		float z_4 = z_3 / abs(z_2);
		
		
		
		// Flip Y Axis
		
		y_4 *= -1.0;
		
		vec_list[i] = (vec3_t){x_4, y_4, z_4};
		
	}
	
	
	
	
	
	
	// Triangle Assembly
	
	unsigned long triangle_count = 0;
	render_target_triangle_t * target_triangle_list = (render_target_triangle_t *) malloc(sizeof(render_target_triangle_t) * model->tri_count);
	
	for(unsigned long i = 0; i < model->tri_count; i++) {
		
		triangle_t target = model->tri_list[i];
		
		render_target_triangle_t tmp;
		
		
		vec3_t vA = vec_list[target.vA];
		vec2_t tA = model->tex_list[target.tA];
		tmp.A = (vertex_t) {
			vA.x,
			vA.y,
			vA.z,
			tA.u,
			tA.v
		};
		
		vec3_t vB = vec_list[target.vB];
		vec2_t tB = model->tex_list[target.tB];
		tmp.B = (vertex_t) {
			vB.x,
			vB.y,
			vB.z,
			tB.u,
			tB.v
		};
		
		vec3_t vC = vec_list[target.vC];
		vec2_t tC = model->tex_list[target.tC];
		tmp.C = (vertex_t) {
			vC.x,
			vC.y,
			vC.z,
			tC.u,
			tC.v
		};
		
		// Culling
		
		
		if (((vB.x - vA.x) * (vC.y - vA.y)) < ((vB.y - vA.y) * (vC.x - vA.x))) {
			target_triangle_list[triangle_count++] = tmp;
		}
		
	}
	
	free(vec_list);
	
	
	
	// Plane Clipping
	
	const unsigned long plane_count = 6;
	const vec3_t plane_ori[] = {
		(vec3_t){  0.0,   0.0,   0.0 },
		(vec3_t){  0.0,   0.0,   1.0 },
		(vec3_t){  0.0,  -1.0,   0.0 },
		(vec3_t){  0.0,   1.0,   0.0 },
		(vec3_t){ -1.0,   0.0,   0.0 },
		(vec3_t){  1.0,   0.0,   0.0 }
	};
	const vec3_t plane_dir[] = {
		(vec3_t){  0.0,   0.0,   1.0 },
		(vec3_t){  0.0,   0.0,  -1.0 },
		(vec3_t){  0.0,   1.0,   0.0 },
		(vec3_t){  0.0,  -1.0,   0.0 },
		(vec3_t){  1.0,   0.0,   0.0 },
		(vec3_t){ -1.0,   0.0,   0.0 }
	};
	
	for(unsigned long i = 0; i < plane_count; i++) {
		render_target_triangle_t * new_target_triangle_list = (render_target_triangle_t *) malloc(sizeof(render_target_triangle_t) * triangle_count * 2);
		unsigned long new_triangle_count = 0;
		
		for(unsigned long j = 0; j < triangle_count; j++) {
			new_target_triangle_list[new_triangle_count] = target_triangle_list[j];
			new_target_triangle_list[new_triangle_count + 1] = target_triangle_list[j];
			new_triangle_count += clip_triangle(new_target_triangle_list + new_triangle_count, plane_ori[i], plane_dir[i]);
		}

		free(target_triangle_list);
		target_triangle_list = new_target_triangle_list;
		triangle_count = new_triangle_count;
		
	}
	
	render_target_triangle_t * new_target_triangle_list = (render_target_triangle_t *) malloc(sizeof(render_target_triangle_t) * triangle_count);
	memcpy(new_target_triangle_list, target_triangle_list, sizeof(render_target_triangle_t) * triangle_count);
	free(target_triangle_list);
	target_triangle_list = new_target_triangle_list;
	
	
	
	// Drawing Triangles
	for(unsigned long i = 0; i < triangle_count; i++) {
		draw_triangle(pix_buf, depth_buf, camera.hor_res, camera.ver_res, target_triangle_list[i], texture);
	}
	
	
	
	free(target_triangle_list);
}




