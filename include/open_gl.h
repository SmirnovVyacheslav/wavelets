#pragma once

#include<Windows.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include<glew.h>
#include<gl/GL.h>
#include<gl/GLU.h>

#include "wavelet.h"

using namespace std;

#define IDC_MYICON                      2
#define IDD_OPENGL_DIALOG               102
#define IDD_ABOUTBOX                    103
#define IDS_APP_TITLE                   103
#define IDM_ABOUT                       104
#define IDM_EXIT                        105
#define IDS_HELLO                       106
#define IDI_OPENGL_ICON                 107
#define IDC_OPENGL_MENU                 108
#define IDC_OPENGL_ACCELERATOR          109
#define IDC_OPENGL                      110
#define IDR_MAINFRAME                   128
#define IDC_STATIC                      -1

#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        129
#define _APS_NEXT_COMMAND_VALUE         32771
#define _APS_NEXT_CONTROL_VALUE         1000
#define _APS_NEXT_SYMED_VALUE           110
#endif
#endif

extern HDC view_hdc, plot_hdc;
extern HWND View, Plot;
extern unique_ptr<Wavelet> wave;

class Shader
{
	GLuint shader_program;

	const GLchar* vs_code;
	const GLchar* gs_code;
	const GLchar* fs_code;

	GLint success;
	GLchar log[1024];

	const GLchar* load(const GLchar* path, const GLchar* code);

	void add_shader(GLenum shader_type, const GLchar* shader_code);

public:

	Shader(const GLchar* vs_path, const GLchar* fs_path, const GLchar* gs_path = NULL);

	void use();

	GLuint operator*();
};

class Open_GL
{
	struct vector2f
	{
		float x, y;

		vector2f() {};

		vector2f(float _x, float _y) : x(_x), y(_y) {};
	};

	struct vector3f
	{
		float x, y, z;

		vector3f() {};

		vector3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
	};

	GLuint nFontList;
	GLYPHMETRICSFLOAT agmf[256];

	GLuint base;

	bool   keys[256];
	GLYPHMETRICSFLOAT gmf[256];
	HFONT  font;

	GLuint view_VBO, plot_VBO;
	HGLRC view_hRC, plot_hRC;

	vector3f* view_data;
	vector2f* plot_data;

	int x, y, z;
	float step_x, step_y;
	
	const int plot_data_sz = 1000;
	const float plot_step_x = 0.001;
	//float plot_step_y;

	GLint uniform_step_x, uniform_step_y;
	//GLint uniform_step_x_p, uniform_step_y_p;

	unique_ptr<Shader> shader;
	unique_ptr<Shader> plot_sh;

public:

	Open_GL();

	~Open_GL();

	void create_buffer();

	void update_buffer();

	void render();

	void create_plot();

	void update_plot();

	void render_plot();

	void resize(int cx, int cy);

	GLvoid glPrint(const char *fmt);
};