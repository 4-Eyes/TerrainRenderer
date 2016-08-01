﻿#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include "loadTGA.h"
using namespace std;

GLuint matrixLoc;
GLuint vaoID, heightTexID;
glm::mat4 projView;
const int gridSize = 512; // makes it a gridSize x gridSize grid
float verts[gridSize * gridSize * 3];
GLushort elems[gridSize * gridSize * 4];
float CDR = 3.14159265 / 180.0;

GLuint loadShader(GLenum shaderType, string filename)
{
	ifstream foo(filename.c_str());
	string data = "";
	while (!foo.eof())
	{
		string tmp;
		getline(foo, tmp);
		data += tmp + "\r\n";
	}
	foo.close();
	const char * shaderTxt = data.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderTxt, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		const char *strShaderType = NULL;
		cerr << "Compile failure in shader: " << strInfoLog << endl;
		delete[] strInfoLog;
	}
	return shader;
}

void loadTextures()
{
	glGenTextures(1, &heightTexID);   //Generate 1 texture ID
	// Load height map
	glActiveTexture(GL_TEXTURE0);  //Texture unit 0
	glBindTexture(GL_TEXTURE_2D, heightTexID);
	loadTGA("HeightMap.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void generateGrid()
{
	float min = -512;
	float max = 512.0;
	float incr = (max - min) / gridSize;
	for (int i = 0; i < gridSize; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			int vertPos = (i * gridSize + j) * 3;
			verts[vertPos] = min + incr * j;
			verts[vertPos + 1] = 0;
			verts[vertPos + 2] = min + incr + i;
			if (i == gridSize - 1 || j == gridSize -1) continue;
			int elemPos = (i * (gridSize -1) + j) * 4;
			elems[elemPos] = i * gridSize + j;
			elems[elemPos + 1] = (i + 1) * gridSize + j;
			elems[elemPos + 2] = (i + 1) * gridSize + j + 1;
			elems[elemPos + 3] = i * gridSize + j + 1;
		}
	}
}

void populateGrid()
{
	for (auto i = 0; i < 512; i++)
	{
		for (auto j = 0; j < 512; j++)
		{
			auto apos = i * 512 + j, ppos = apos * 3;
			verts[ppos] = i;
			verts[ppos + 1] = 0.0f;
			verts[ppos + 2] = -j;

			if (i != 511 && j != 511)
			{
				auto epos = apos * 4, npos = apos + 512;
				elems[epos] = apos;
				elems[epos + 1] = npos;
				elems[epos + 2] = npos + 1;
				elems[epos + 3] = apos + 1;
			}
		}
	}
}

void initialise()
{
//	generateGrid();
	populateGrid();
	loadTextures();
	glm::mat4 proj, view;
	// set up shaders
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, "Terrain.vert");
	GLuint fragShader = loadShader(GL_FRAGMENT_SHADER, "Terrain.frag");
//	GLuint tessEvalShader = loadShader(GL_TESS_EVALUATION_SHADER, "TerrainTES.glsl");
//	GLuint tessContShader = loadShader(GL_TESS_CONTROL_SHADER, "TerrainTCS.glsl");

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragShader);
//	glAttachShader(program, tessEvalShader);
//	glAttachShader(program, tessContShader);
	glLinkProgram(program);

	// Make sure the shaders have been linked correctly
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
	glUseProgram(program);

	GLuint texLoc = glGetUniformLocation(program, "heightSampler");
	glUniform1i(texLoc, 0);

	proj = glm::perspective(20.0f * CDR, 1.0f, 10.0f, 1000.0f);
	view = lookAt(glm::vec3(0.0, 5.0, 12.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	GLuint vboID[4];

	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	glGenVertexArrays(2, vboID);

	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, gridSize * gridSize * 3, verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);  // Vertex position

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (gridSize - 1) * (gridSize - 1) * 4, elems, GL_STATIC_DRAW);

	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void display()
{
	glm::mat4 prodMatrix = projView;        //Model-view-proj matrix

	glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, &prodMatrix[0][0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
	glDrawElements(GL_QUADS, (gridSize - 1) * (gridSize - 1) * 4, GL_UNSIGNED_SHORT, NULL);
	glFlush();
}

void update(int value)
{
	glutTimerFunc(50, update, 0);
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Terrain");
	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	if (glewInit() == GLEW_OK)
	{
		cout << "GLEW initialization successful! " << endl;
		cout << " Using GLEW version " << glewGetString(GLEW_VERSION) << endl;
	}
	else
	{
		cerr << "Unable to initialize GLEW  ...exiting." << endl;
		exit(EXIT_FAILURE);
	}

	initialise();
	glutDisplayFunc(display);
	glutTimerFunc(50, update, 0);
	glutMainLoop();
}