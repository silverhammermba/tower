#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/ext.hpp>

using std::cerr;
using std::endl;

// read entire contents of file into string
std::string read_file(const std::string& filename)
{
	std::string content;

	std::ifstream in(filename);
	if (!in)
		throw std::runtime_error("Failed to read file: " + filename);

	in.seekg(0, std::ios::end);
	content.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&content[0], content.size());
	in.close();

	return content;
}

// load file contents, compile shader, report errors
void load_shader(GLuint shader, const std::string& filename)
{
	std::string source = read_file(filename);
	const char* ptr = source.c_str();
	glShaderSource(shader, 1, (const GLchar**)&ptr, nullptr);
	glCompileShader(shader);
	// error check
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	GLint log_length;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
	if (log_length > 0)
	{
		char buffer[log_length];
		glGetShaderInfoLog(shader, log_length, nullptr, buffer);
		cerr << buffer;
	}
	if (status == GL_FALSE)
		throw std::runtime_error("Failed to compile shader: " + filename);
}

void check_gl()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		throw std::runtime_error("OpenGL error: " + std::string((const char*)gluErrorString(error)));
}

#include "sprite.hpp"
#include "dude.hpp"
#include "tower.hpp"
#include "texman.hpp"
#include "control.hpp"

int main(int argc, char** argv)
{
	// INIT SYSTEMS

	// start SDL
	SDL_Init(SDL_INIT_VIDEO);

	// start SDL image
	int img_flags = IMG_INIT_PNG;
	if (IMG_Init(img_flags) & img_flags != img_flags)
	{
		cerr << "IMG_Init failed: " << IMG_GetError() << endl;
		return 1;
	}

	// specify non-deprecated OpenGL context
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// window dimensions
	unsigned int width = 1024;
	unsigned int height = 768;

	// create window
	SDL_Window* window = SDL_CreateWindow(
		"TOWER SMASH",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width,
		height,
		SDL_WINDOW_OPENGL
	);
	if (window == nullptr)
	{
		cerr << "SDL_CreateWindow failed: " << SDL_GetError() << endl;
		return 1;
	}

	// create context
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == nullptr)
	{
		cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << endl;
		return 1;
	}

	// start GLEW
	glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		cerr << "glewInit failed: " << glewGetErrorString(error) << endl;
		return 1;
	}

	// CREATE SHADER PROGRAM

	// compile shaders
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	load_shader(vertex_shader, "vertex.glsl");

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	load_shader(fragment_shader, "fragment.glsl");

	// create program
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	glLinkProgram(program);
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	GLint log_length;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
	if (log_length > 0)
	{
		char buffer[log_length];
		glGetProgramInfoLog(program, log_length, nullptr, buffer);
		cerr << buffer;
	}
	if (status == GL_FALSE)
	{
		cerr << "glLinkProgram failed\n";
		return 1;
	}

	glDeleteShader(fragment_shader);
	glDeleteShader(vertex_shader);

	glUseProgram(program);

	TextureManager textures;

	Sprite dude_sprite(program, 28, 21, textures["dude.png"]);
	Dude dude(dude_sprite);
	Tower tower(program, 240, 220, textures["stone.png"]);

	glEnable(GL_DEPTH_TEST);
	// only draw opaque pixels
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int last_time = SDL_GetTicks();
	unsigned int now;
	unsigned int frame_time;

	// set up camera
	glm::vec3 camera_pos(0.f, 0.f, 120.f);
	glm::mat4 camera = glm::lookAt(camera_pos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

	GLint camera_u = glGetUniformLocation(program, "camera");
	glUniformMatrix4fv(camera_u, 1, GL_FALSE, glm::value_ptr(camera));

	// set up perspective
	float zNear = 1.f;
	float zFar = 1000.f;
	glUniform1f(glGetUniformLocation(program, "zNear"), zNear);
	glUniform1f(glGetUniformLocation(program, "zFar"), zFar);

	glm::mat4 perspective = glm::perspectiveFov(1.57f, (float)width, (float)height, zNear, zFar);
	glUniformMatrix4fv(glGetUniformLocation(program, "perspective"), 1, GL_FALSE, glm::value_ptr(perspective));

	// we only use one texture at a time
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program, "sprite"), 0);

	float cam_speed = 200.f;

	KeyControls controls;

	GLint time_u = glGetUniformLocation(program, "time");

	// main loop
	SDL_Event event;
	bool running = true;
	while (running)
	{
		// update timers
		now = SDL_GetTicks();
		frame_time = now - last_time;
		last_time = now;

		glUniform1ui(time_u, now);

		// event handling
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					controls.update(event);
					break;
			}
		}

		if (controls["quit"])
			running = false;

		glm::vec2 dir(0.f, 0.f);
		if (controls["up"])
			dir.y += 1;
		if (controls["down"])
			dir.y -= 1;
		if (controls["right"])
			dir.x += 1;
		if (controls["left"])
			dir.x -= 1;

		if (dir != glm::vec2(0.f, 0.f))
			dude.move(dir);

		int cam_dir = 0;
		if (controls["zoomout"])
			cam_dir += 1;
		if (controls["zoomin"])
			cam_dir -= 1;

		if (cam_dir != 0)
		{
			camera_pos.z += (cam_dir * cam_speed * frame_time) / 1000.f;
			camera = glm::lookAt(camera_pos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
			glUniformMatrix4fv(camera_u, 1, GL_FALSE, glm::value_ptr(camera));
		}

		// draw
		glClearColor(0.2f, 0.2f, 0.2f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		dude.draw();
		tower.draw(camera_pos.z);

		SDL_GL_SwapWindow(window);
	}

	glDeleteProgram(program);

	// clean up
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);

	// stop SDL image
	IMG_Quit();

	// stop SDL
	SDL_Quit();

	return 0;
}
