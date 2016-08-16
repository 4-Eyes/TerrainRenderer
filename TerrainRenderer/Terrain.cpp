#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "loadTGA.h"
#include "Terrain.h"
using namespace std;

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

GLuint matrixLoc;
GLuint vaoID;
GLuint textures[5];
glm::mat4 proj, view, projView;

float CDR = 3.14159265 / 180.0;
glm::vec3 eye = glm::vec3(100, 80, 12.0);
glm::vec4 light = glm::vec4(20.0, 80, 80, 1.0);
glm::vec3 lookAt = glm::vec3(100, 0, 100.0);
glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
GLuint lightLoc;
GLuint mvMatrixLoc;
GLuint eyeLoc;
bool isMesh = false;

GLuint loadShader(GLenum shaderType, string filename)
{
	ifstream file(filename.c_str());
	string data = "";
	while (!file.eof())
	{
		string tmp;
		getline(file, tmp);
		data += tmp + "\r\n";
	}
	file.close();

//	if (filename == "Terrain.geom")
//	{
//		data = data.substr(3, data.length() - 3);
//	}
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
	glGenTextures(5, textures);   //Generate 1 texture ID
	// Load height map
	glActiveTexture(GL_TEXTURE0);  //Texture unit 0
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	loadTGA("heightmap2.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Load snow texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	loadTGA("snow.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Load rock texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	loadTGA("rock.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Load grass texture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	loadTGA("grass.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Load water texture
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	loadTGA("water.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void generateGrid()
{
	for (int i = 0; i < GRID_SIZE; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			int vertPos = (i * GRID_SIZE + j) * 3;
			verts[vertPos] = GRID_MIN_X + CELL_SIZE * i;
			verts[vertPos + 1] = 0.0f;
			verts[vertPos + 2] = GRID_MIN_Z + CELL_SIZE * j;
			if (i == GRID_SIZE - 1 || j == GRID_SIZE -1) continue;
			int elemPos = (i * (GRID_SIZE -1) + j) * 4;
			elems[elemPos] = i * GRID_SIZE + j;
			elems[elemPos + 3] = (i + 1) * GRID_SIZE + j;
			elems[elemPos + 2] = (i + 1) * GRID_SIZE + j + 1;
			elems[elemPos + 1] = i * GRID_SIZE + j + 1;
		}
	}
}


void initialise()
{
	generateGrid();
	// set up shaders
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, "Terrain.vert");
	GLuint fragShader = loadShader(GL_FRAGMENT_SHADER, "Terrain.frag");
	GLuint tessEvalShader = loadShader(GL_TESS_EVALUATION_SHADER, "Terrain.eval");
	GLuint tessContShader = loadShader(GL_TESS_CONTROL_SHADER, "Terrain.cont");
	GLuint geometryShader = loadShader(GL_GEOMETRY_SHADER, "Terrain.geom");

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragShader);
	glAttachShader(program, tessEvalShader);
	glAttachShader(program, tessContShader);
	glAttachShader(program, geometryShader);
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

	proj = glm::perspective(20.0f, 1.0f, 10.0f, 1000.0f);
	view = glm::lookAt(eye, lookAt, up);
	projView = proj * view;

	matrixLoc = glGetUniformLocation(program, "mvpMatrix");
	mvMatrixLoc = glGetUniformLocation(program, "mvMatrix");
	lightLoc = glGetUniformLocation(program, "lightPos");
	eyeLoc = glGetUniformLocation(program, "eyePos");
	glUniform4fv(lightLoc, 1, &light[0]);

	// Load textures
	loadTextures();
	GLuint texLoc = glGetUniformLocation(program, "heightSampler");
	glUniform1i(texLoc, 0);
	GLuint snowLoc = glGetUniformLocation(program, "snowSampler");
	glUniform1i(snowLoc, 1);
	GLuint rockLoc = glGetUniformLocation(program, "rockSampler");
	glUniform1i(rockLoc, 2);
	GLuint grassLoc = glGetUniformLocation(program, "grassSampler");
	glUniform1i(grassLoc, 3);
	GLuint waterLoc = glGetUniformLocation(program, "waterSampler");
	glUniform1i(waterLoc, 4);

	// Pass in variables
	GLuint gridLoc = glGetUniformLocation(program, "gridSize");
	glUniform1i(gridLoc, GRID_SIZE);

	GLuint cellSizeLoc = glGetUniformLocation(program, "cellSize");
	glUniform1f(cellSizeLoc, CELL_SIZE);

	GLuint terrainHeightLoc = glGetUniformLocation(program, "terrainHeight");
	glUniform1f(terrainHeightLoc, TERRAIN_HEIGHT);

	GLuint waterFractionLoc = glGetUniformLocation(program, "waterFraction");
	glUniform1f(waterFractionLoc, WATER_FRACTION);

	GLuint grassFractionLoc = glGetUniformLocation(program, "grassFraction");
	glUniform1f(grassFractionLoc, GRASS_FRACTION);

	GLuint grassRockBlendFractionLoc = glGetUniformLocation(program, "grassRockBlendFraction");
	glUniform1f(grassRockBlendFractionLoc, GRASS_ROCK_BLEND_FRACTION);

	GLuint rockFractionLoc = glGetUniformLocation(program, "rockFraction");
	glUniform1f(rockFractionLoc, ROCK_FRACTION);

	GLuint rockSnowBlendFractionLoc = glGetUniformLocation(program, "rockSnowBlendFraction");
	glUniform1f(rockSnowBlendFractionLoc, ROCK_SNOW_BLEND_FRACTION);

	GLuint highTessLevelLoc = glGetUniformLocation(program, "highTessLevel");
	glUniform1f(highTessLevelLoc, HIGH_TESS_LEVEL);

	GLuint lowTessLevelLoc = glGetUniformLocation(program, "lowTessLevel");
	glUniform1f(lowTessLevelLoc, LOW_TESS_LEVEL);

	GLuint tessDisanceMaxLoc = glGetUniformLocation(program, "tessDistanceMax");
	glUniform1f(tessDisanceMaxLoc, TESS_DISTANCE_MAX);

	GLuint tessDisanceMinLoc = glGetUniformLocation(program, "tessDistanceMin");
	glUniform1f(tessDisanceMinLoc, TESS_DISTANCE_MIN);

	GLuint tessMulLevel1Loc = glGetUniformLocation(program, "tessMulLevel1");
	glUniform1f(tessMulLevel1Loc, TESS_MUL_LEVEL_1);

	GLuint tessMulLevel2Loc = glGetUniformLocation(program, "tessMulLevel2");
	glUniform1f(tessMulLevel2Loc, TESS_MUL_LEVEL_2);

	GLuint tessMulLevel3Loc = glGetUniformLocation(program, "tessMulLevel3");
	glUniform1f(tessMulLevel3Loc, TESS_MUL_LEVEL_3);

	GLuint tessMulLevel4Loc = glGetUniformLocation(program, "tessMulLevel4");
	glUniform1f(tessMulLevel4Loc, TESS_MUL_LEVEL_4);

	GLuint tessGradLevel1Loc = glGetUniformLocation(program, "tessGradLevel1");
	glUniform1f(tessGradLevel1Loc, TESS_GRAD_LEVEL_1);

	GLuint tessGradLevel2Loc = glGetUniformLocation(program, "tessGradLevel2");
	glUniform1f(tessGradLevel2Loc, TESS_GRAD_LEVEL_2);

	GLuint tessGradLevel3Loc = glGetUniformLocation(program, "tessGradLevel3");
	glUniform1f(tessGradLevel3Loc, TESS_GRAD_LEVEL_3);

	GLuint tessGradLevel4Loc = glGetUniformLocation(program, "tessGradLevel4");
	glUniform1f(tessGradLevel4Loc, TESS_GRAD_LEVEL_4);

	GLuint enableGradientTesselationMultiplierLoc = glGetUniformLocation(program, "enableGradientTesselationMultiplier");
	glUniform1i(enableGradientTesselationMultiplierLoc, ENABLE_GRADIENT_TESSELATION_MULTIPLIER);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	GLuint vboID[4];

	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	glGenVertexArrays(2, vboID);

	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);  // Vertex position

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, isMesh ? GL_LINE : GL_FILL);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glutPostRedisplay();
}

void display()
{
	view = glm::lookAt(eye, lookAt, up);
	projView = proj * view;
	glm::mat4 prodMatrix = projView;        //Model-view-proj matrix

	glUniform4fv(lightLoc, 1, &light[0]);
	glUniform3fv(eyeLoc, 1, &eye[0]);
	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, &prodMatrix[0][0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
	glDrawElements(GL_PATCHES, GRID_SIZE * GRID_SIZE * 4, GL_UNSIGNED_SHORT, NULL);
	glFlush();
}

void processSpecialKeys(int key, int xx, int yy)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		eye += glm::vec3(1, 0, 0);
		lookAt += glm::vec3(1, 0, 0);
		break;
	case GLUT_KEY_RIGHT:
		eye += glm::vec3(-1, 0, 0);
		lookAt += glm::vec3(-1, 0, 0);
		break;
	case GLUT_KEY_UP:
		eye += glm::vec3(0, 1, 0);
		lookAt += glm::vec3(0, 1, 0);
		break;
	case GLUT_KEY_DOWN:
		eye += glm::vec3(0, -1, 0);
		lookAt += glm::vec3(0, -1, 0);
		break;
	}
	glutPostRedisplay();
}

void proceesKey(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'w':
		eye += glm::vec3(0, 0, 1);
		lookAt += glm::vec3(0, 0, 1);
		break;
	case 's':
		eye += glm::vec3(0, 0, -1);
		lookAt += glm::vec3(0, 0, -1);
		break;
	case 'r' :
		isMesh = !isMesh;
		glPolygonMode(GL_FRONT_AND_BACK, isMesh ? GL_LINE : GL_FILL);
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
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
	glutSpecialFunc(processSpecialKeys);
	glutKeyboardFunc(proceesKey);
	glutMainLoop();
}
