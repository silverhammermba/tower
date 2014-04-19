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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

#include "sprite.hpp"
#include "texman.hpp"

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

	glUseProgram(program);

	TextureManager textures;

	Sprite dude(program, 28, 21, textures["dude.png"]);

	// variable positions
	GLint position_a = glGetAttribLocation(program, "position");
	GLint tex_coord_a = glGetAttribLocation(program, "tex_coord");
	GLint perspective_u = glGetUniformLocation(program, "perspective");
	GLint camera_u = glGetUniformLocation(program, "camera");
	GLint time_u = glGetUniformLocation(program, "time");

	GLuint tower_vao;
	glGenVertexArrays(1, &tower_vao);
	glBindVertexArray(tower_vao);

	GLuint tower_vbo;
	glGenBuffers(1, &tower_vbo);

	// wrap count
	unsigned int rw = 10;
	unsigned int rh = 7;

	// triangle strip forming sides of a cube
	GLfloat tower_verts[] = {
		40.f, 40.f,  0.f, 0.f, rh * 1.f,
		40.f, 40.f, 80.f, 0.f, 0.f,

		40.f, -40.f,  0.f, rw * 1.f, rh * 1.f,
		40.f, -40.f, 80.f, rw * 1.f, 0.f,

		-40.f, -40.f,  0.f, rw * 2.f, rh * 1.f,
		-40.f, -40.f, 80.f, rw * 2.f, 0.f,

		-40.f, 40.f,  0.f, rw * 3.f, rh * 1.f,
		-40.f, 40.f, 80.f, rw * 3.f, 0.f,

		40.f, 40.f,  0.f, rw * 4.f, rh * 1.f,
		40.f, 40.f, 80.f, rw * 4.f, 0.f
	};

	glBindBuffer(GL_ARRAY_BUFFER, tower_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tower_verts), tower_verts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(position_a);
	glVertexAttribPointer(position_a, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(tex_coord_a);
	glVertexAttribPointer(tex_coord_a, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int last_time = SDL_GetTicks();
	unsigned int now;
	unsigned int frame_time;

	// camera position in pixels
	GLfloat camera[3] = {0.f, 0.f, 120.f};
	glUniform3fv(camera_u, 1, camera);

	glm::mat4 perspective = glm::perspectiveFov(1.57f, (float)width, (float)height, 1.f, 1000.f);
	glUniformMatrix4fv(perspective_u, 1, GL_FALSE, glm::value_ptr(perspective));

	// we only use one texture at a time
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program, "sprite"), 0);

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
			if (event.type == SDL_QUIT)
				running = false;
		}

		// draw
		glClearColor(0.2f, 0.2f, 0.2f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, textures["stone.png"]);

		glBindVertexArray(tower_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);

		dude.draw();

		SDL_GL_SwapWindow(window);
	}

	glDeleteBuffers(1, &tower_vbo);
	glDeleteVertexArrays(1, &tower_vao);

	glDeleteProgram(program);
	glDeleteShader(fragment_shader);
	glDeleteShader(vertex_shader);

	// clean up
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);

	// stop SDL image
	IMG_Quit();

	// stop SDL
	SDL_Quit();

	return 0;
}
