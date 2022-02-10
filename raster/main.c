#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <time.h>


#include <SDL2/SDL.h>
#include <pthread.h>

#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
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
	
	clock_t total_start = clock();
	
	printf("Hello, World!\n");
	srand(time(NULL));
	
	unsigned long hor_res = 2048;
	unsigned long ver_res = 2048;
	
	vec3_t min = {0.0, 0.0, 0.0};
	vec3_t max = {0.0, 0.0, 0.0};
	
	model_t * model = load_model("viking_room.obj");

	for(unsigned long i = 0; i < model->vec_count; i++) {
//		model->vec_list[i].x *= 0.01f;
//		model->vec_list[i].y *= -0.01f;
//		model->vec_list[i].z *= 0.01f;
		
		if (min.x > model->vec_list[i].x) min.x = model->vec_list[i].x;
		if (min.y > model->vec_list[i].y) min.y = model->vec_list[i].y;
		if (min.z > model->vec_list[i].z) min.z = model->vec_list[i].z;
		
		if (max.x < model->vec_list[i].x) max.x = model->vec_list[i].x;
		if (max.y < model->vec_list[i].y) max.y = model->vec_list[i].y;
		if (max.z < model->vec_list[i].z) max.z = model->vec_list[i].z;
	}
	
	
	
	
	
	printf("Vec Count: %ld\n", model->vec_count);
	printf("Tex Count: %ld\n", model->tex_count);
	printf("Tri Count: %ld\n", model->tri_count);
	printf("Min: [%g, %g, %g]\n", min.x, min.y, min.z);
	printf("Max: [%g, %g, %g]\n", max.x, max.y, max.z);
	
	
	
	texture_t * texture = load_texture("viking_room.png");
	
	
	
	camera_t main_camera = (camera_t) {
		(vec3_t) { -1.0,  0.0,  0.0 },	// Position
		(vec3_t) {  1.0,  0.0,  0.0 },	// Look
		(vec3_t) {  0.0,  1.0,  0.0 },	// Up
		(vec3_t) {  0.0,  0.0,  1.0 },	// Right
		M_PI / 2.0,
		10.0f, 0.1f,
		hor_res, ver_res
	};
	
	float move_speed = 0.1f;
	float look_speed = M_PI / 64.0f;
	
	
	
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
		
		
		vec3_t move = (vec3_t) {0.0, 0.0, 0.0};
		if (user_input & move_forward) {
			move.x += main_camera.look.x;
			move.y += main_camera.look.y;
			move.z += main_camera.look.z;
		}
		if (user_input & move_back) {
			move.x -= main_camera.look.x;
			move.y -= main_camera.look.y;
			move.z -= main_camera.look.z;
		}
		if (user_input & move_up) {
			move.x += main_camera.up.x;
			move.y += main_camera.up.y;
			move.z += main_camera.up.z;
		}
		if (user_input & move_down) {
			move.x -= main_camera.up.x;
			move.y -= main_camera.up.y;
			move.z -= main_camera.up.z;
		}
		if (user_input & move_right) {
			move.x += main_camera.right.x;
			move.y += main_camera.right.y;
			move.z += main_camera.right.z;
		}
		if (user_input & move_left) {
			move.x -= main_camera.right.x;
			move.y -= main_camera.right.y;
			move.z -= main_camera.right.z;
		}
		
		float move_mag = sqrt((move.x * move.x) + (move.y * move.y) + (move.z * move.z));
		if (move_mag > 0.1f) {
			move.x *= move_speed / move_mag;
			move.y *= move_speed / move_mag;
			move.z *= move_speed / move_mag;
			
			main_camera.pos.x += move.x;
			main_camera.pos.y += move.y;
			main_camera.pos.z += move.z;
		}
		
		
		
		vec3_t look = (vec3_t) {0.0, 0.0, 0.0};
		if (user_input & look_up) {
			look.x += main_camera.right.x;
			look.y += main_camera.right.y;
			look.z += main_camera.right.z;
		}
		if (user_input & look_down) {
			look.x -= main_camera.right.x;
			look.y -= main_camera.right.y;
			look.z -= main_camera.right.z;
		}
		if (user_input & look_left) {
			look.x += main_camera.up.x;
			look.y += main_camera.up.y;
			look.z += main_camera.up.z;
		}
		if (user_input & look_right) {
			look.x -= main_camera.up.x;
			look.y -= main_camera.up.y;
			look.z -= main_camera.up.z;
		}
		if (user_input & roll_clock) {
			look.x += main_camera.look.x;
			look.y += main_camera.look.y;
			look.z += main_camera.look.z;
		}
		if (user_input & roll_counter) {
			look.x -= main_camera.look.x;
			look.y -= main_camera.look.y;
			look.z -= main_camera.look.z;
		}
		float look_mag = sqrt((look.x * look.x) + (look.y * look.y) + (look.z * look.z));
		if (look_mag > 0.1f) {
			
			look.x /= look_mag;
			look.y /= look_mag;
			look.z /= look_mag;
			
			main_camera.look = vec3_rotate(main_camera.look, look, look_speed);
			main_camera.up = vec3_rotate(main_camera.up, look, look_speed);
			main_camera.right = vec3_cross(main_camera.look, main_camera.up);
			
		}
		
		printf("CAMERA:\n");
		printf("POS  : [%g, %g, %g]\n", main_camera.pos.x, main_camera.pos.y, main_camera.pos.z);
		printf("LOOK : [%g, %g, %g]\n", main_camera.look.x, main_camera.look.y, main_camera.look.z);
		printf("UP   : [%g, %g, %g]\n", main_camera.up.x, main_camera.up.y, main_camera.up.z);
		printf("RIGHT: [%g, %g, %g]\n", main_camera.right.x, main_camera.right.y, main_camera.right.z);
		
		
		
		// Before rendering each frame:
		SDL_LockTexture(frame_buffer, NULL, (void **)&pixel_buffer, &pitch);
		
		
		
		// Clear frame:
		for(unsigned long i = 0; i < hor_res * ver_res; i++) {
			
			pixel_buffer[i] = 0x0;
			depth_buffer[i] = NAN;
			
		}
		
		
		// Render Frame
		render_model(pixel_buffer, depth_buffer, main_camera, model, texture);
		
		
		// After rendering frame
		
		SDL_UnlockTexture(frame_buffer);
		SDL_RenderCopy(main_renderer, frame_buffer, NULL, NULL);
		SDL_RenderPresent(main_renderer);
		frame_count++;
		
		clock_t end = clock();
		
		printf("Frame %d: %g\n", frame_count, (float)(end - start) / (float) CLOCKS_PER_SEC);
		
		
		//if (frame_count > 60) loop = 0;

	}


	// Cleanup
	free(depth_buffer);
	SDL_DestroyTexture(frame_buffer);
	SDL_DestroyRenderer(main_renderer);
	SDL_DestroyWindow(main_window);
	SDL_Quit();
	
	
	destroy_texture(texture);
	destroy_model(model);
	
	clock_t total_end = clock();
	
	printf("Total Time: %g\n", (float)(total_end - total_start) / (float)CLOCKS_PER_SEC);
	
	return 0;
}
