#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>


#include <SDL2/SDL.h>
#include <pthread.h>

#define STB_IMAGE_IMPLEMENTATION
#include "raster.h"





/*





typedef struct {
	unsigned int vertex_count;
	vertex_t * vertex_list;
	
	unsigned int triangle_count;
	triangle_t * triangle_list;
} model_t;



enum transform_type {TRANSFORM_TYPE_TRANSLATE, TRANSFORM_TYPE_ROTATE, TRANSFORM_TYPE_SCALE};

typedef struct {
	enum transform_type type;
	unsigned int start_index;
	unsigned int end_index;
	vec3_t data;
} transform_t;

typedef struct {
	model_t * model;
	texture_t * texture;
	
	unsigned int transform_count;
	transform_t * transform_list;
} model_instance_t;

model_instance_t * create_model_instance(...);
void destroy_model_instance(model_instance_t * instance);
void render_model_instance(pixel_buffer, depth_buffer, width, height, camera, model_instance_t);



// Render model instance:
//   Copy all model vertices
//   Apply all instance transforms
//   Camera transforms / projections
//   Triangle Assembly / Culling
//   Plane clipping
//   Draw triangles



*/

int main() {
	
	printf("Hello, World!\n");
	srand(time(NULL));
	
	long hor_res = 2048;
	long ver_res = 2048;
	
	unsigned long vertex_count = 4;
	vertex_t vertex_list[] = {
		(vertex_t) { 
			(((rand() % 65536) / 32768.0f) - 1.0f), 
			(((rand() % 65536) / 32768.0f) - 1.0f), 
			(((rand() % 65536) / 65536.0f)), 
			(0.25f), (0.25f) 
		},
		
		(vertex_t) { 
			(((rand() % 65536) / 32768.0f) - 1.0f), 
			(((rand() % 65536) / 32768.0f) - 1.0f), 
			(((rand() % 65536) / 65536.0f)), 
			(1.0f), (0.0f) 
		},
		
		(vertex_t) { 
			(((rand() % 65536) / 32768.0f) - 1.0f), 
			(((rand() % 65536) / 32768.0f) - 1.0f), 
			(((rand() % 65536) / 65536.0f)), 
			(0.0f), (1.0f) 
		},
		
		(vertex_t) { 
			(((rand() % 65536) / 32768.0f) - 1.0f), 
			(((rand() % 65536) / 32768.0f) - 1.0f), 
			(((rand() % 65536) / 65536.0f)), 
			(0.75f), (0.75f) 
		}
	};
	
	vec3_t vertex_list_vel[] = {
		(vec3_t) {
			(((rand() % 65536) / 32768.0f) - 1.0f) * 0.01f,
			(((rand() % 65536) / 32768.0f) - 1.0f) * 0.01f,
			(((rand() % 65536) / 32768.0f) - 1.0f) * 0.01f
		},
		
		(vec3_t) {
			(((rand() % 65536) / 32768.0f) - 1.0f) * 0.01f,
			(((rand() % 65536) / 32768.0f) - 1.0f) * 0.01f,
			(((rand() % 65536) / 32768.0f) - 1.0f) * 0.01f
		},
		
		(vec3_t) {
			(((rand() % 65536) / 32768.0f) - 1.0f) * 0.01f,
			(((rand() % 65536) / 32768.0f) - 1.0f) * 0.01f,
			(((rand() % 65536) / 32768.0f) - 1.0f) * 0.01f
		},
		
		(vec3_t) {
			(((rand() % 65536) / 32768.0f) - 1.0f) * 0.01f,
			(((rand() % 65536) / 32768.0f) - 1.0f) * 0.01f,
			(((rand() % 65536) / 32768.0f) - 1.0f) * 0.01f
		}
		
	};
	
	
	unsigned long triangle_count = 2;
	triangle_t triangle_list[] = {
		(triangle_t) {0, 1, 2},
		(triangle_t) {1, 3, 2}
	};
	
	
	model_t render_target = {
		
		vertex_count,
		vertex_list,
		
		triangle_count,
		triangle_list
		
	};
	
	texture_t * texture = load_texture("Cropped_Image.png");
	
	
	
	
	
	printf("Init SDL!\n");
	
	int window_width = 512;
	int window_height = 512;
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Failed to init SDL: %s\n", SDL_GetError());
		return 1;
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

	SDL_Window * main_window = SDL_CreateWindow(
		"Raster Demo",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		window_width, window_height, 0);
	if(!main_window) {
		printf("Failed to create SDL window: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer * main_renderer = SDL_CreateRenderer(main_window, -1, (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
	if (!main_renderer) {
		printf("Failed to create SDL Renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(main_window);
		SDL_Quit();
		return 1;
	}

	SDL_Texture * frame_buffer = SDL_CreateTexture(
		main_renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		hor_res, ver_res);
	SDL_SetTextureBlendMode(frame_buffer, SDL_BLENDMODE_NONE);
	float * depth_buffer = (float *) malloc(sizeof(float) * hor_res * ver_res);
	unsigned int * pixel_buffer;
	int pitch;
	
	/*
	// Before rendering each frame:
	
	SDL_LockTexture(frame_buffer, NULL, (void **)&pixel_buffer, &pitch);
	
	
	
	// Render frame here into pixel_buffer
	
	
	
	// After rendering each frame:
	
	SDL_UnlockTexture(frame_buffer);
	SDL_RenderCopy(main_renderer, frame_buffer, NULL, NULL);
	SDL_RenderPresent(main_renderer);
	
	*/
	
	
	


	
	const unsigned long look_up			(1 << 0);
	const unsigned long look_down		(1 << 1);
	const unsigned long look_right		(1 << 2);
	const unsigned long look_left		(1 << 3);
	const unsigned long roll_clock		(1 << 4);
	const unsigned long roll_counter	(1 << 5);
	
	const unsigned long move_up			(1 << 6);
	const unsigned long move_down		(1 << 7);
	const unsigned long move_forward	(1 << 8);
	const unsigned long move_back		(1 << 9);
	const unsigned long move_left		(1 << 10);
	const unsigned long move_right		(1 << 11);
	
	
	unsigned char loop = 1;
	int frame_count = 0;
	unsigned long user_input = 0;
	

	while(loop) {
		
		clock_t start = clock();
		
		//printf("Frame!\n");
		
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
		
		for(unsigned long i = 0; i < vertex_count; i++) {
			
			vertex_t * vert_pos = vertex_list + i;
			vec3_t * vert_vel = vertex_list_vel + i;
			
			vert_pos->x += vert_vel->x;
			if (vert_pos->x < -1.0f && vert_vel->x < 0.0f) {
				vert_pos->x = -1.0f;
				vert_vel->x *= -1.0f;
			}
			if (vert_pos->x > 1.0f && vert_vel->x > 0.0f) {
				vert_pos->x = 1.0f;
				vert_vel->x *= -1.0f;
			}
			
			vert_pos->y += vert_vel->y;
			if (vert_pos->y < -1.0f && vert_vel->y < 0.0f) {
				vert_pos->y = -1.0f;
				vert_vel->y *= -1.0f;
			}
			if (vert_pos->y > 1.0f && vert_vel->y > 0.0f) {
				vert_pos->y = 1.0f;
				vert_vel->y *= -1.0f;
			}
			
			
			vert_pos->z += vert_vel->z;
			if (vert_pos->z < 0.0f && vert_vel->z < 0.0f) {
				vert_pos->z = 0.0f;
				vert_vel->z *= -1.0f;
			}
			if (vert_pos->z > 1.0f && vert_vel->z > 0.0f) {
				vert_pos->z = 1.0f;
				vert_vel->z *= -1.0f;
			}
		}
		
		
		
		
		
		
		
		
		
		
		
		
		
		// Before rendering each frame:
		SDL_LockTexture(frame_buffer, NULL, (void **)&pixel_buffer, &pitch);
		
		
		
		// Clear frame:
		for(long i = 0; i < hor_res * ver_res; i++) {
			
			pixel_buffer[i] = 0x0;
			depth_buffer[i] = NAN;
			
		}
		
		
		
		// Render Frame
		render_model(pixel_buffer, depth_buffer, hor_res, ver_res, &render_target, texture);
		
		
		// After rendering frame
		
		SDL_UnlockTexture(frame_buffer);
		SDL_RenderCopy(main_renderer, frame_buffer, NULL, NULL);
		SDL_RenderPresent(main_renderer);
		frame_count++;
		
		clock_t end = clock();
		
		printf("Frame %d: %g\n", frame_count, (float)(end - start) / (float) CLOCKS_PER_SEC);
		
		
		//if (frame_count > 30) loop = 0;

	}


	// Cleanup
	free(depth_buffer);
	SDL_DestroyTexture(frame_buffer);
	SDL_DestroyRenderer(main_renderer);
	SDL_DestroyWindow(main_window);
	SDL_Quit();
	
	destroy_texture(texture);
	
	return 0;
}
