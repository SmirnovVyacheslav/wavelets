#pragma once

#include "open_gl.h"

int font_list_base_2d = 2000;

Shader::Shader(const GLchar* vs_path, const GLchar* fs_path, const GLchar* gs_path)
{
	shader_program = glCreateProgram();

	add_shader(GL_VERTEX_SHADER, load(vs_path, vs_code));
	if (gs_path != NULL) add_shader(GL_GEOMETRY_SHADER_EXT, load(gs_path, gs_code));
	add_shader(GL_FRAGMENT_SHADER, load(fs_path, fs_code));

	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	
	if (!success)
	{
		glGetProgramInfoLog(shader_program, sizeof(log), NULL, log);
		exit(1);
	}

	glValidateProgram(shader_program);
	glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &success);
	
	if (!success)
	{
		glGetProgramInfoLog(shader_program, sizeof(log), NULL, log);
		exit(1);
	}
}

const GLchar* Shader::load(const GLchar* path, const GLchar* code)
{
	string shader_code;
	ifstream shader_file;
	stringstream shader_stream;

	shader_file.open(path);

	shader_stream << shader_file.rdbuf();

	shader_file.close();

	shader_code = shader_stream.str();

	code = shader_code.c_str();

	return code;
}

void Shader::add_shader(GLenum shader_type, const GLchar* shader_code)
{
	GLuint shader = glCreateShader(shader_type);

	glShaderSource(shader, 1, &shader_code, NULL);
	
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	
	if (success)
	{
		glAttachShader(shader_program, shader);
	}
	else
	{
		glGetShaderInfoLog(shader, sizeof(log), NULL, log);
		exit(1);
	}
}

void Shader::use()
{
	glUseProgram(shader_program);
}

GLuint Shader::operator*()
{
	return shader_program;
}

Open_GL::Open_GL()
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 16;
	pfd.cDepthBits = 16;

	view_hdc = GetDC(View);
	plot_hdc = GetDC(Plot);
	GLuint iPixelFormat = ChoosePixelFormat(view_hdc, &pfd);

	if (iPixelFormat != 0)
	{
		PIXELFORMATDESCRIPTOR bestMatch_pfd;
		DescribePixelFormat(view_hdc, iPixelFormat, sizeof(pfd), &bestMatch_pfd);
		DescribePixelFormat(plot_hdc, iPixelFormat, sizeof(pfd), &bestMatch_pfd);

		if (bestMatch_pfd.cDepthBits < pfd.cDepthBits)
		{
			return;
		}

		if (SetPixelFormat(view_hdc, iPixelFormat, &pfd) == FALSE)
		{
			DWORD dwErrorCode = GetLastError();
			return;
		}

		if (SetPixelFormat(plot_hdc, iPixelFormat, &pfd) == FALSE)
		{
			DWORD dwErrorCode = GetLastError();
			return;
		}
	}
	else
	{
		DWORD dwErrorCode = GetLastError();
		return;
	}

	view_hRC = wglCreateContext(view_hdc);
	plot_hRC = wglCreateContext(plot_hdc);

	wglMakeCurrent(view_hdc, view_hRC);
	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		exit(1);
	}

	shader.reset(new Shader("vertex.vs", "fragment.fs", "geometry.gs"));

	uniform_step_x = glGetUniformLocation(*(*shader), "step_x");
	uniform_step_y = glGetUniformLocation(*(*shader), "step_y");

	wglMakeCurrent(plot_hdc, plot_hRC);
	glClearColor(0.0f, 0.5f, 0.5f, 1.0f);

	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_FLAT);

	/*base = glGenLists(256);

	font = CreateFont(12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		L"Comic Sans MS");

	SelectObject(plot_hdc, font);

	wglUseFontOutlines(plot_hdc, 0, 256, base, 0.0f, 0.0f, WGL_FONT_POLYGONS, gmf);*/

	/*nFontList = glGenLists(256);

	font = CreateFont(12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		L"Comic Sans MS");
	SelectObject(plot_hdc, font);
	wglUseFontBitmaps(plot_hdc, 0, 256, nFontList);*/

	/*SelectObject(plot_hdc, GetStockObject(SYSTEM_FONT));
	wglUseFontBitmaps(plot_hdc, 0, 255, 1000);
	glListBase(1000);*/

	 // set the start of the display lists for the 2d font


		// somewhere in Prog_Init()....

		HFONT font = CreateFont(20, 0, 0, 0,
			FW_NORMAL, FALSE, FALSE, FALSE,
			ANSI_CHARSET, 0,
			0, 0, 0, L"Times New Roman"); // can only use true type fonts 
										 //  this one is 40 points in size

	SelectObject(plot_hdc, font);

	wglUseFontBitmaps(plot_hdc, 0, 127, font_list_base_2d);

	plot_sh.reset(new Shader("vertex_pl.vs", "fragment_pl.fs"));

	//uniform_step_x_p = glGetUniformLocation(*(*shader), "step_x");
	//uniform_step_y_p = glGetUniformLocation(*(*shader), "step_y");
}

Open_GL::~Open_GL()
{
	if (view_hRC != NULL)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(view_hRC);
		view_hRC = NULL;
	}

	if (view_hdc != NULL)
	{
		ReleaseDC(View, view_hdc);
		view_hdc = NULL;
	}

	if (plot_hRC != NULL)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(plot_hRC);
		plot_hRC = NULL;
	}

	if (plot_hdc != NULL)
	{
		ReleaseDC(Plot, plot_hdc);
		plot_hdc = NULL;
	}

	glDeleteLists(base, 256);
}

GLvoid Open_GL::glPrint(const char *fmt)
{
	float         length = 0;
	char          text[256];
	va_list              ap;
	if (fmt == NULL)            // Если нет текста,
		return;

	va_start(ap, fmt);         // Анализ строки на переменные
	vsprintf(text, fmt, ap);   // И конвертация символов в реальные коды
	va_end(ap);

	for (unsigned int loop = 0;loop<(strlen(text));loop++)//Цикл поиска размера строки
	{
		length += gmf[(unsigned char)text[loop]].gmfCellIncX;
		// Увеличение размера на ширину символа
	}
	//glRasterPos2i(0.2, 0.2);
	glTranslatef(-length, 1.0f, 0.0f);
	

	glPushAttrib(GL_LIST_BIT); // Сохраняет в стеке значения битов списка отображения
	glListBase(base);          // Устанавливает базовый символ в 0
	
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	
	glPopAttrib(); // Восстанавливает значение Display List Bits
}

void Open_GL::create_buffer()
{
	x = (*wave)->get_width();
	y = (*wave)->get_height();
	z = (*wave)->get_layer();

	step_x = 2.0 / x;
	step_y = 2.0 / y;
	
	view_data = new vector3f[x * y * z];

	create_plot();

	update_buffer();
}

void Open_GL::update_buffer()
{
	wglMakeCurrent(view_hdc, view_hRC);

	float color;
	auto it = (*wave)->get_iterator();

	for (int k = 0; k < x * y * z; k+= x * y)
		for (int j = 0; j < y; ++j)
			for (int i = 0; i < x; ++i)
			{
				color = static_cast<float>(abs(it[k + j * x + i]));
				view_data[k + j * x + i] = vector3f(-1.0 + i * step_x, 1.0 - j * step_y, color);
			}

	glGenBuffers(1, &view_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, view_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vector3f) * x * y * z, view_data, GL_STATIC_DRAW);

	update_plot();
}

void Open_GL::render()
{
	wglMakeCurrent(view_hdc, view_hRC);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (wave.get() != nullptr)
	{
		(*shader).use();

		glUniform1f(uniform_step_x, step_x);
		glUniform1f(uniform_step_y, step_y);

		glEnableVertexAttribArray(0);
		
		glBindBuffer(GL_ARRAY_BUFFER, view_VBO);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);//3 * sizeof(vector3f), 0);

		glDrawArrays(GL_POINTS, x * y * (*wave)->get_cur_layer(), x * y);

		glDisableVertexAttribArray(0);

		glUseProgram(0);
	}

	SwapBuffers(view_hdc);
}

void Open_GL::create_plot()
{
	plot_data = new vector2f[plot_data_sz * 2];
}

void Open_GL::update_plot()
{
	wglMakeCurrent(plot_hdc, plot_hRC);

	vector<int> plot_y = vector<int>(plot_data_sz);
	for (int i = 0; i < plot_data_sz; ++i) plot_y[i] = 0;

	(*wave)->data_dispersion(plot_step_x, plot_y);

	plot_max = static_cast<int>(*max_element(plot_y.cbegin(), plot_y.cend()));

	for (int i = 0, j = 0; j < plot_data_sz - 1; i += 2, j++)
	{
		plot_data[i] = vector2f(plot_offset_x + plot_step_x * j * 2 * plot_scale, plot_offset_y + float(plot_y[j]) / plot_max * plot_size);
		plot_data[i + 1] = vector2f(plot_offset_x + plot_step_x * (j + 1) * 2 * plot_scale, plot_offset_y + float(plot_y[j + 1]) / plot_max * plot_size);
	}

	glGenBuffers(1, &plot_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, plot_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vector2f) * plot_data_sz * 2, plot_data, GL_STATIC_DRAW);
}

void Open_GL::render_plot()
{
	wglMakeCurrent(plot_hdc, plot_hRC);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_LINES);
	glColor3d(1, 1, 1);
	glLineWidth(3);

	glVertex3d(-1.0, plot_offset_y, 0.0);
	glVertex3d(1.0, plot_offset_y, 0.0);

	glVertex3d(plot_offset_x, -1.0, 0.0);
	glVertex3d(plot_offset_x, 1.0, 0.0);

	for (int i = 1; i < 20; i++)
	{
		glVertex3d(plot_offset_x + plot_spread * i, plot_offset_y - 0.01, 0.0);
		glVertex3d(plot_offset_x + plot_spread * i, plot_offset_y + 0.01, 0.0);

		glVertex3d(plot_offset_x - 0.01, plot_offset_y + 0.1 * i, 0.0);
		glVertex3d(plot_offset_x + 0.01, plot_offset_y + 0.1 * i, 0.0);
	}

	glEnd();

	if (wave.get() != nullptr)
	{
		glColor3d(1.0, 0.0, 0.0);
		float step = plot_max / (plot_size * 10);

		for (int i = 1; i < 20; ++i)
		{
			//if (i % 2)
			//{
				glRasterPos3f(plot_offset_x + plot_spread * i - 0.05, plot_offset_y - 0.05, 0.0);
				glListBase(font_list_base_2d);
				glCallLists(7, GL_UNSIGNED_BYTE, to_string(0.1 * i).c_str());
			//}

			glRasterPos3f(-1.0, plot_offset_y + 0.1 * i, 0.0);
			glListBase(font_list_base_2d);
			glCallLists(7, GL_UNSIGNED_BYTE, to_string(step * i).c_str());
		}

		if ((*wave)->get_filter_value() != 0.0)
		{
			glBegin(GL_LINES);
			glColor3d(0.0, 0.0, 0.0);
			glLineWidth(4);

			glVertex3d(plot_offset_x + (*wave)->get_filter_value() * plot_scale, 1.0, 0.0);
			glVertex3d(plot_offset_x + (*wave)->get_filter_value() * plot_scale, plot_offset_y, 0.0);

			glEnd();
		}

		(*plot_sh).use();

		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, plot_VBO);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_LINES, 0, plot_data_sz * 2);

		glDisableVertexAttribArray(0);

		glUseProgram(0);
		
	}

	SwapBuffers(plot_hdc);
}

void Open_GL::resize(int cx, int cy)
{
	wglMakeCurrent(view_hdc, view_hRC);
	glViewport(0, 0, cx, cy);

	wglMakeCurrent(plot_hdc, plot_hRC);
	glViewport(0, 0, cx, cy);
}