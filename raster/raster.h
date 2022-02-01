#ifndef __RASTER_H
#define __RASTER_H

typedef struct {
	
	float x, y, z;
	
} vertex_t;


typedef struct {
	
	vertex_t A, B, C;
	
} triangle_t;




// Triangle must be in image space: [[0, hor_res],[0, ver_res]]
void draw_triangle(unsigned int * pix_buf, float * depth_buf, long hor_res, long ver_res, triangle_t tri, unsigned int col) {
	
	vertex_t A = tri.A;
	vertex_t B = tri.B;
	vertex_t C = tri.C;
	
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
	
	
	// to calculate Z:
	// Nx * x + Ny * y + Nz * z = P; Equation of a plane
	// Nx, Ny, Nz are components of the normal vector.
	// P can be solved for by using any point on the plane, and the normal vector.
	
	float P = (Nx * A.x) + (Ny * A.y) + (Nz * A.z);
	
	// Z = (P - Nx * x - Ny * y) / Nz
	// Z = (P / Nz) - (((Nx / Nz) * x) + ((Ny / Nz) * y));
	
	float PNz = P / Nz;
	float NxNz = Nx / Nz;
	float NyNz = Ny / Nz;
	
	
	
	unsigned char bend_left = (Nz < 0.0f);
	
	float start_y = A.y;
	float end_y = C.y;
	
	
	for(float y = start_y; y < end_y; y += 1.0f) {
		
//		if (y < 0.0f || y >= ver_res) continue;
		
		// X = Y * (dx / dy) + b
		
		float start_x;
		float end_x;
		
		if (y < B.y) {
			
			if (bend_left) {
				// From line AB to line AC
				start_x = (y * AB_m) + AB_b;
				end_x = (y * AC_m) + AC_b;
				
			} else {
				// From line AC to line AB
				start_x = (y * AC_m) + AC_b;
				end_x = (y * AB_m) + AB_b;
				
			}
			
		} else {
			
			if (bend_left) {
				// From line BC to line AC
				start_x = (y * BC_m) + BC_b;
				end_x = (y * AC_m) + AC_b;
				
			} else {
				// From line AC to line BC
				start_x = (y * AC_m) + AC_b;
				end_x = (y * BC_m) + BC_b;
				
			}
		}
		
		for(float x = start_x; x < end_x; x += 1.0f) {
			
//			if (x < 0.0f || x >= hor_res) continue;
			
			float z = (PNz - ((NxNz * x) + (NyNz * y)));
			
			long ind_x = (long) floor(x);
			long ind_y = (long) floor(y);
			long index = ind_x + (ind_y * hor_res);
			
			if (depth_buf[index] != NAN && z > depth_buf[index]) continue;
			
			depth_buf[index] = z;
			pix_buf[index] = col;
			
		}
		
	}
	
	
	
	
	
	
}







#endif