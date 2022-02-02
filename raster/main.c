#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>


#include <SDL2/SDL.h>
#include <pthread.h>

#include "raster.h"


#define to_color(a, r, g, b) ((unsigned int)(((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | ((b & 0xFF))))


int main() {
	
	printf("Hello, World!\n");
	srand(time(NULL));
	
	long hor_res = 1440;
	long ver_res = 960;
	
	long tri_count = 256;
	
	unsigned int tri_colors[tri_count];
	for(int i = 0; i < tri_count; i++) {
		unsigned int a = 0xFF;
		unsigned int r = rand() % 0xFF;
		unsigned int g = rand() % 0xFF;
		unsigned int b = rand() % 0xFF;
		tri_colors[i] = to_color(a, r, g, b);
	}
	
	
	long vert_count = tri_count * 3;
	vertex_t test_vert_list_pos[vert_count];
	vertex_t test_vert_list_vel[vert_count];
	
	for(long i = 0; i < vert_count; i++) {
		test_vert_list_pos[i] = {
			((rand() % (hor_res * 1024)) / 1024.0f),
			((rand() % (ver_res * 1024)) / 1024.0f),
			((rand() % (ver_res * 1024)) / 1024.0f)
		};

		test_vert_list_vel[i] = {
			((rand() % 20000) / 10000.0f) - 1.0f,
			((rand() % 20000) / 10000.0f) - 1.0f,
			((rand() % 20000) / 10000.0f) - 1.0f
		};
	}
	
	
	
	
	
	
	printf("Init SDL!\n");
	
	int window_width = 720;
	int window_height = 480;
	
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
		
		for(long i = 0; i < vert_count; i++) {
			
			vertex_t * vert_pos = test_vert_list_pos + i;
			vertex_t * vert_vel = test_vert_list_vel + i;
			
			vert_pos->x += vert_vel->x;
			if (vert_pos->x < 0.0f && vert_vel->x < 0.0f) {
					vert_vel->x *= -1.0f;
			}
			if (vert_pos->x > hor_res && vert_vel->x > 0.0f) {
					vert_vel->x *= -1.0f;
			}
			
			vert_pos->y += vert_vel->y;
			if (vert_pos->y < 0.0f && vert_vel->y < 0.0f) {
					vert_vel->y *= -1.0f;
			}
			if (vert_pos->y > ver_res && vert_vel->y > 0.0f) {
					vert_vel->y *= -1.0f;
			}
			
			
			vert_pos->z += vert_vel->z;
			if (vert_pos->z < 0.0f && vert_vel->z < 0.0f) {
					vert_vel->z *= -1.0f;
			}
			if (vert_pos->z > ver_res && vert_vel->z > 0.0f) {
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
		for(long i = 0; i < tri_count; i++) {
			
			triangle_t tmp = (triangle_t) {
				
				test_vert_list_pos[(i * 3) + 0],
				test_vert_list_pos[(i * 3) + 1],
				test_vert_list_pos[(i * 3) + 2]
				
			};
			
			draw_triangle(pixel_buffer, depth_buffer, hor_res, ver_res, tmp, tri_colors[i]);
			
		}
		
		
		
		
		// After rendering frame
		
		SDL_UnlockTexture(frame_buffer);
		SDL_RenderCopy(main_renderer, frame_buffer, NULL, NULL);
		SDL_RenderPresent(main_renderer);
		frame_count++;
		
		clock_t end = clock();
		
		printf("Frame %d: %g\n", frame_count, (float)(end - start) / (float) CLOCKS_PER_SEC);
		
		
		if (frame_count > 360) loop = 0;

	}


	// Cleanup
	free(depth_buffer);
	SDL_DestroyTexture(frame_buffer);
	SDL_DestroyRenderer(main_renderer);
	SDL_DestroyWindow(main_window);
	SDL_Quit();
	
	return 0;
}
