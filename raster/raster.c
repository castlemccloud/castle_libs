

#include "raster.h"

#include <stdio.h>


// Triangle must be in image space: [[-1, 1], [-1, 1], [0, 1]]
void draw_triangle(unsigned int * pix_buf, float * depth_buf, long hor_res, long ver_res, triangle_t tri) {
	
	vertex_t A = tri.A;
	vertex_t B = tri.B;
	vertex_t C = tri.C;
	
	// From image space [[-1, 1], [-1, 1], [0, 1]] to screen space [[0, hor_res], [0, ver_res], [0, 1]]
	
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
		
		// If pixel is off screen, skip
		// if (y < 0.0f || y >= ver_res) continue;
		
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
			
			// If pixel is off screen, skip
			// if (x < 0.0f || x >= hor_res) continue;
			
			float z = (PNz - ((NxNz * x) + (NyNz * y)));
			
			long ind_x = (long) x;
			long ind_y = (long) y;
			long index = ind_x + (ind_y * hor_res);
			
			// If pixel is behind something already, skip
			if (depth_buf[index] != NAN && z > depth_buf[index]) continue;
			
			// Determine U, and V values
			// to sample texture, and colors:
			
			// P[X, Y, Z] - A[x,y,z] = ( U * AB[x,y,z] ) + ( V * AC[x,y,z] );
			
			// Two equations needed to solve for two unknowns
			// X - Ax =  U * AB_dx + V * AC_dx
			// Y - Ay =  U * AB_dy + V * AC_dy
			
			// ( ( X - A.x ) - U * AB_dx ) / AC_dx = V
			// ( ( Y - A.y ) - U * AB_dy ) / AC_dy = V
			
			// ( ( X - A.x ) - U * AB_dx ) / AC_dx = ( ( Y - A.y ) - U * AB_dy ) / AC_dy
			// ( ( X - A.x ) * AC_dy ) - U * ( AB_dx * AC_dy ) = ( ( Y - A.y ) * AC_dx ) - U * ( AB_dy * AC_dx )
			
			// U * ( ( AB_dy * AC_dx ) - ( AB_dx * AC_dy ) ) = ( ( Y - A.y ) * AC_dx ) - ( ( X - A.x ) * AC_dy )
			// U = ( ( ( Y - A.y ) * AB_dx ) - ( ( X - A.x ) * AC_dy ) ) / ( ( AB_dy * AC_dx ) - ( AB_dx * AC_dy ) )
			
			float u = (((y - A.y) * AC_dx) - ((x - A.x) * AC_dy)) / ((AB_dy * AC_dx) - (AB_dx * AC_dy));
			
			
			// ( ( X - A.x ) - V * AC_dx ) / AB_dx = U
			// ( ( Y - A.y ) - V * AC_dy ) / AB_dy = U
			
			// ( ( X - A.x ) - V * AC_dx ) / AB_dx = ( ( Y - A.y ) - V * AC_dy ) / AB_dy
			// ( ( X - A.x ) * AB_dy ) - V * ( AB_dy * AC_dx ) = ( ( Y - A.y ) * AB_dx ) - V * ( AB_dx * AC_dy )

			// V * ( ( AB_dy * AC_dx ) - ( AB_dx * AC_dy ) ) = ( ( X - A.x ) * AB_dy ) - ( ( Y - A.y ) * AB_dx )
			// V = ( ( ( X - A.x ) * AB_dy ) - ( ( Y - A.y ) * AB_dx ) ) / ( ( AB_dy * AC_dx ) - ( AB_dx * AC_dy ) )
			
			float v = (((x - A.x) * AB_dy) - ((y - A.y) * AB_dx)) / ((AB_dy * AC_dx) - (AB_dx * AC_dy));
			
			// If pixel is outside the bounds of the triangle, skip
			// Allowing for a little bit of wiggle room, for floating point error.
			// if (u < -0.001f || v < -0.001f || (u + v) > 1.001f) continue;
			
			
			
			
			depth_buf[index] = z;
			
			// float tex_u = ((1.0f - (u + v)) * A.u) + (u * B.u) + (v * C.u);
			// float tex_v = ((1.0f - (u + v)) * A.u) + (u * B.u) + (v * C.u);
			// unsigned int col = texture_sampler(texture, tex_u, tex_v);
			
			
			
			unsigned int a = 0xFF;
			unsigned int r = (unsigned int) floor(255.0f * (((1.0f - (u + v)) * A.r) + (u * B.r) + (v * C.r)));
			unsigned int g = (unsigned int) floor(255.0f * (((1.0f - (u + v)) * A.g) + (u * B.g) + (v * C.g)));
			unsigned int b = (unsigned int) floor(255.0f * (((1.0f - (u + v)) * A.b) + (u * B.b) + (v * C.b)));
			
			
			pix_buf[index] = argb_to_uint(a, r, g, b);
			
		}
		
	}
	
}