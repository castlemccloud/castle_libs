#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "../matrix/matrix.h"
#include "raster.h"




int main() {
	
	
	unsigned long hor_res = 800;
	unsigned long ver_res = 600;
	
	
	SDL_Init(SDL_INIT_EVERYTHING);
	
	SDL_Window * main_window = SDL_CreateWindow(
		"Raster Demo", 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		hor_res, ver_res, 0);
	
	SDL_Renderer * main_renderer = SDL_CreateRenderer(
		main_window,
		-1, 
		(SDL_RENDERER_ACCELERATED, SDL_RENDERER_PRESENTVSYNC));
	
	SDL_Texture * tex_buf = SDL_CreateTexture(
		main_renderer, 
		SDL_PIXELFORMAT_ARGB8888, 
		SDL_TEXTUREACCESS_STREAMING,
		hor_res, ver_res);
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	SDL_SetTextureBlendMode(tex_buf, SDL_BLENDMODE_BLEND);
	
	
	
	
	model_t * teapot = load_obj("raster/teapot.obj", NULL);
	printf("Vec Count: %ld\n", teapot->vec_count);
	printf("Tri Count: %ld\n", teapot->tri_count);
	
	
	camera_t * cam = malloc(sizeof(camera_t));
	
	cam->pos =		(vec_t){ 0,	 0,	 -10.0};
	
	cam->right =	(vec_t){ 1.0,	0.0,	0.0};
	cam->up =		(vec_t){ 0.0,	1.0,	0.0};
	cam->look =		(vec_t){ 0.0,	0.0,	1.0};
	
	
	
	
	
	
	cam->hRes = hor_res;
	cam->vRes = ver_res;
	
	cam->FOV = M_PI / 2.0;
	
	cam->zNear = 0.01;
	cam->zFar = 10000.0;
	
	
	// Depth map
	double * depth = (double *) malloc(sizeof(double) * cam->hRes * cam->vRes);
	for(long i = 0; i < cam->hRes * cam->vRes; i++) {
		depth[i] = 1.0;
	}
	
	
	
	
	
	
	unsigned int * pix_buf;
	unsigned int pitch;
	char loop = 1;
	
	long frame_count = 0;
	
	#define MOVE_RIGHT		0x0001
	#define MOVE_LEFT		0x0002
	#define MOVE_UP			0x0004
	#define MOVE_DOWN		0x0008
	#define MOVE_FORWARD	0x0010
	#define MOVE_BACKWARD	0x0020
	#define LOOK_UP			0x0040
	#define LOOK_DOWN		0x0080
	#define LOOK_LEFT		0x0100
	#define LOOK_RIGHT		0x0200
	#define ROLL_RIGHT		0x0400
	#define ROLL_LEFT		0x0800
	
	
	
	
	
	int move_look = 0;
	double move_speed = 0.125;
	double look_speed = M_PI * (1.0 / 128.0);
	
	
	
	
	while(loop) {
		
		// Inputs
		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
				case SDL_QUIT:
					loop = 0;
					break;
					
				case SDL_KEYDOWN:
					switch (evt.key.keysym.sym) {
						
						case SDLK_d:
							move_look |= MOVE_RIGHT;
							break;
						
						case SDLK_a:
							move_look |= MOVE_LEFT;
							break;
						
						case SDLK_SPACE:
							move_look |= MOVE_UP;
							break;
						
						case SDLK_LSHIFT:
							move_look |= MOVE_DOWN;
							break;
						
						case SDLK_w:
							move_look |= MOVE_FORWARD;
							break;
						
						case SDLK_s:
							move_look |= MOVE_BACKWARD;
							break;
							
							
							
						case SDLK_UP:
							move_look |= LOOK_UP;
							break;
						
						case SDLK_DOWN:
							move_look |= LOOK_DOWN;
							break;
						
						case SDLK_LEFT:
							move_look |= LOOK_LEFT;
							break;
						
						case SDLK_RIGHT:
							move_look |= LOOK_RIGHT;
							break;
						
						case SDLK_e:
							move_look |= ROLL_RIGHT;
							break;
						
						case SDLK_q:
							move_look |= ROLL_LEFT;
							break;
						
						default:
							break;
					}
					break;
					
					
				case SDL_KEYUP:
					switch(evt.key.keysym.sym) {
							
						case SDLK_d:
							move_look &= ~MOVE_RIGHT;
							break;
						
						case SDLK_a:
							move_look &= ~MOVE_LEFT;
							break;
						
						case SDLK_SPACE:
							move_look &= ~MOVE_UP;
							break;
						
						case SDLK_LSHIFT:
							move_look &= ~MOVE_DOWN;
							break;
						
						case SDLK_w:
							move_look &= ~MOVE_FORWARD;
							break;
						
						case SDLK_s:
							move_look &= ~MOVE_BACKWARD;
							break;
						
						
						
						case SDLK_UP:
							move_look &= ~LOOK_UP;
							break;
						
						case SDLK_DOWN:
							move_look &= ~LOOK_DOWN;
							break;
						
						case SDLK_LEFT:
							move_look &= ~LOOK_LEFT;
							break;
						
						case SDLK_RIGHT:
							move_look &= ~LOOK_RIGHT;
							break;
						
						case SDLK_e:
							move_look &= ~ROLL_RIGHT;
							break;
						
						case SDLK_q:
							move_look &= ~ROLL_LEFT;
							break;
						
						default:
							break;
							
					}
					break;
					
				default:
					break;
					
			}
			
		}
		
		
		
		vec_t move = (vec_t) {0.0, 0.0, 0.0};
		
		if (move_look & MOVE_RIGHT) {
			move.x += cam->right.x;
			move.y += cam->right.y;
			move.z += cam->right.z;
		}
		
		if (move_look & MOVE_LEFT) {
			move.x -= cam->right.x;
			move.y -= cam->right.y;
			move.z -= cam->right.z;
		}
		
		if (move_look & MOVE_UP) {
			move.x += cam->up.x;
			move.y += cam->up.y;
			move.z += cam->up.z;
		}
		
		if (move_look & MOVE_DOWN) {
			move.x -= cam->up.x;
			move.y -= cam->up.y;
			move.z -= cam->up.z;
		}
		
		if (move_look & MOVE_FORWARD) {
			move.x += cam->look.x;
			move.y += cam->look.y;
			move.z += cam->look.z;
		}
		
		if (move_look & MOVE_BACKWARD) {
			move.x -= cam->look.x;
			move.y -= cam->look.y;
			move.z -= cam->look.z;
		}
		
		double magSq = (move.x * move.x) + (move.y * move.y) + (move.z * move.z);
		if (magSq > 0.0) {
			double scalar = move_speed / sqrt(magSq);
			
			cam->pos.x += move.x * scalar;
			cam->pos.y += move.y * scalar;
			cam->pos.z += move.z * scalar;
			
		}
		
		
		
		
		vec_t look = (vec_t) {0.0, 0.0, 0.0};
		
		if (move_look & LOOK_UP) {
			look.x += cam->right.x;
			look.z += cam->right.y;
			look.y += cam->right.z;
		}
		
		if (move_look & LOOK_DOWN) {
			look.x -= cam->right.x;
			look.z -= cam->right.y;
			look.y -= cam->right.z;
		}
		
		if (move_look & LOOK_LEFT) {
			look.x += cam->up.x;
			look.y += cam->up.y;
			look.z += cam->up.z;
		}
		
		if (move_look & LOOK_RIGHT) {
			look.x -= cam->up.x;
			look.y -= cam->up.y;
			look.z -= cam->up.z;
		}
		
		if (move_look & ROLL_RIGHT) {
			look.x += cam->look.x;
			look.y += cam->look.y;
			look.z += cam->look.z;
		}
		
		if (move_look & ROLL_LEFT) {
			look.x -= cam->look.x;
			look.y -= cam->look.y;
			look.z -= cam->look.z;
		}
		
		magSq = (look.x * look.x) + (look.y * look.y) + (look.z * look.z);
		if (magSq > 0.1) {
			double scalar = 1.0 / sqrt(magSq);
			
			look.x *= scalar;
			look.y *= scalar;
			look.z *= scalar;
			
			// rotate perspective around look
			
		}
		
		
		
		
		
		SDL_RenderClear(main_renderer);
		
		
		
		SDL_LockTexture(tex_buf, NULL, (void **) &pix_buf, &pitch);
		// Clear Texture
		for(long i = 0; i < hor_res * ver_res; i++) {
			pix_buf[i] = 0x0;
			depth[i] = 1.0;
		}
		
		
		
		
		render_model(teapot, cam, pix_buf, depth);
		
		
		
		SDL_UnlockTexture(tex_buf);
		
		
		SDL_RenderCopy(main_renderer, tex_buf, NULL, NULL);
		SDL_RenderPresent(main_renderer);
		
		
	}
	
	
	
	
	free(depth);
	
	free(cam);
	
	destroy_model(teapot);
	
	SDL_DestroyTexture(tex_buf);
	SDL_DestroyRenderer(main_renderer);
	SDL_DestroyWindow(main_window);
	
	SDL_Quit();
}
