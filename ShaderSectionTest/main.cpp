#define RUNMODE 1
#define PI 3.14159265
#define IWIDTH 800
#define IHEIGHT 800

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <cmath>
#include <algorithm>

using namespace std;

//
// Function Declaration
//

void rotateTriangle(GLFWwindow* window, bool direction);
void processInput(GLFWwindow *window, unsigned int VBO, unsigned int EBO);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mainProcess(GLFWwindow* window, unsigned int shaderProgram, unsigned int VAO, unsigned int VBO, unsigned int EBO);

struct color { float r; float g; float b; float a; };
struct iter_ { bool r; bool g; bool b; };
struct keypresses { bool leftClick; bool rightClick; bool equal; bool minus; bool e; bool q; bool r; };

keypresses ckeys = { false, false, false, false, false, false, false };

//
// Background Variables
//

color background = { 0.0f, 0.5f, 1.0f, 1.0f };
iter_ backgrounditer = { true, true, true };
bool backgrounditerenable = false;

//
// Extra Variables
//

float height_ = IHEIGHT;
float width_ = IWIDTH;

float vertices[] = {
	height_/width_*-0.5f*cos(PI/6), -0.5f*sin(PI/6),	 0.0f, 1.0f, 1.0f, 1.0f,	// Bottom Left Mid
	 height_ / width_ * 0.5f*cos(PI/6), -0.5f*sin(PI/6),	 0.0f, 0.0f, 0.0f, 0.0f,	// Bottom Right Mid
	 0.0f,				  0.5f,				 0.0f, 0.5f, 1.0f, 0.0f	// Top Mid
};

const float resetVertices[] = {
	height_ / width_ * -0.5f * cos(PI / 6), -0.5f * sin(PI / 6),	 0.0f, 1.0f, 1.0f, 1.0f,	// Bottom Left Mid
	 height_ / width_ * 0.5f * cos(PI / 6), -0.5f * sin(PI / 6),	 0.0f, 0.0f, 0.0f, 0.0f,	// Bottom Right Mid
	 0.0f,				  0.5f,				 0.0f, 0.5f, 1.0f, 0.0f	// Top Mid
};
	
unsigned int indices[] = {  // note that we start from 0!
	0, 1, 2
};

//
// BEGIN FUNCTIONS
// Main Function
//
int WinMain() {
	//
	// Read from shader files.
	// 

	srand((unsigned)time(NULL));

	fstream fragShaderFile("fragmentShader.frag", fstream::in);
	fstream vertexShaderFile("vertexShader.vert", fstream::in);

	string fragString = "";
	char tchar;

	while (fragShaderFile >> noskipws >> tchar) {
		fragString += tchar;
	}
	const char* fragShaderSource = const_cast<char*>(fragString.c_str());

	string vertString = "";
	while (vertexShaderFile >> noskipws >> tchar) {
		vertString += tchar;
	}
	const char* vertexShaderSource = const_cast<char*>(vertString.c_str());
	//cout << fragShaderSource << endl << vertexShaderSource << endl;

	// 
	// Init System
	// 

	cout << "System Start Shader Section.\n";
	
	int success;
	char infoLog[512];

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(IWIDTH, IHEIGHT, "Test OPENGL", NULL, NULL);

	if (window == NULL) {
		cout << "Failed to create window.\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to init glad.\n";
		return -1;
	}

	// 
	// Vertex Shader
	//

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);


	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR:SHADER:VERTEX:COMPILATION_FAILED " << infoLog << endl;
	}


	// 
	// Fragment Shader
	//

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cout << "ERROR:SHADER:FRAGMENT:COMPILATION_FAILED " << infoLog << endl;
	}

	// 
	// Shader Program
	// 

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cout << "ERROR:SHADER:PROGRAM:LINK_FAILED " << infoLog << endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// 
	// Vertex Buffer and Array
	//

	// DECLARATION
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	// BIND VERTEX ARRAY
	glBindVertexArray(VAO);

	// Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

	// Element Buffer Object (Indices)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // ENABLES POSITION ATTRIBUTE
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	// 
	// Main Loop
	//
	
	while (!glfwWindowShouldClose(window)) {
		if (RUNMODE == 1)
		{
			mainProcess(window, shaderProgram, VAO, VBO, EBO);
		}
	}
	
	// 
	// Program Termination
	//

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}

// ///////// //
// FUNCTIONS //
// ///////// //

//
// Main Loop Process
// The event loop that passes every frame.
//
void mainProcess(\
	GLFWwindow* window,\
	unsigned int shaderProgram, \
	unsigned int VAO, \
	unsigned int VBO, \
	unsigned int EBO) 
{
	processInput(window, VBO, EBO);

	// Generate Width & Height
	GLfloat uniform_width = glGetUniformLocation(shaderProgram, "Width");
	GLfloat uniform_height = glGetUniformLocation(shaderProgram, "Height");

	int width;
	int height;

	glfwGetWindowSize(window, &width, &height);
	//cout << width << " " << height << endl;

	glUniform1f(uniform_width, width);
	glUniform1f(uniform_height, height);

	// Clear Screen with color ()
	true == backgrounditer.r ? background.r+=0.1 : background.r-=0.01; 
	if (background.r > 0.99) { backgrounditer.r = !backgrounditer.r; }
	if (background.r < 0.01) { backgrounditer.r = !backgrounditer.r; }

	true == backgrounditer.g ? background.g += 0.01 : background.g -= 0.01;
	if (background.g > 0.99) { backgrounditer.g = !backgrounditer.g; }
	if (background.g < 0.01) { backgrounditer.g = !backgrounditer.g; }

	true == backgrounditer.b ? background.b += 0.01 : background.b -= 0.01;
	if (background.b > 0.99) { backgrounditer.b = !backgrounditer.b; }
	if (background.b < 0.01) { backgrounditer.b = !backgrounditer.b; }

	if (!backgrounditerenable) { background.r = 1; background.g = 1; background.b = 1; }
	glClearColor(background.r, background.g, background.b, background.a);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw Screen
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO)
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

// 
// Resize Frame
//
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

//
// Process Input Triggers
//
void processInput(GLFWwindow* window, unsigned int VBO, unsigned int EBO) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		if (!ckeys.leftClick) {

			int vsize = sizeof(vertices) / sizeof(vertices[0]) - 1;
			bool do_ = false;
			int tmp = 0;

			cout << endl;
			for (int x = 0; x < vsize+1; x++ )
			{
				if (tmp > 2) {
					do_ = !do_;
					tmp = 0;
				}
				if (do_) {
					float toDo = (rand() % 101) / 100.0f;
					cout << "Vertex Index " << x << "\t(" << vertices[x] << ")\tis now " << toDo << endl;
					vertices[x] = toDo;
				}
				tmp++;
				
			}
			cout << endl;
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		}
		ckeys.leftClick = true;
	}
	else {
		ckeys.leftClick = false;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
		if (!ckeys.rightClick) {
			backgrounditer.r = (rand() % 2 == 1 ? true : false);
			backgrounditer.g = (rand() % 2 == 1 ? true : false);
			backgrounditer.b = (rand() % 2 == 1 ? true : false);
			background.r = 0.5f;
			background.g = 0.5f;
			background.b = 0.5f;
			backgrounditerenable = !backgrounditerenable;
			cout << "Background Color Change: " << backgrounditerenable << endl;
		}
		ckeys.rightClick = true;
	}
	else {
		ckeys.rightClick = false;
	}

	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
		int vsize = sizeof(vertices) / sizeof(vertices[0]) - 1;
		bool do_ = true;
		int tmp = 0;

		cout << endl;
		for (int x = 0; x < vsize + 1; x++)
		{
			if (tmp > 2) {
				do_ = !do_;
				tmp = 0;
			}
			if (do_) {
				if (vertices[x] > 0) {
					cout << "Vertex Index " << x << "\t(" << vertices[x] << ")\tis now " << vertices[x] + 0.01 << endl;
					vertices[x] += 0.01;
				}
				else if (vertices[x] < 0) {
					cout << "Vertex Index " << x << "\t(" << vertices[x] << ")\tis now " << vertices[x] - 0.01 << endl;
					vertices[x] -= 0.01;
				}

			}
			tmp++;

		}
		cout << endl;
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);


		if (!ckeys.equal) {
		}
		ckeys.equal = true;
	}
	else {
		ckeys.equal = false;
	}

	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
		int vsize = sizeof(vertices) / sizeof(vertices[0]) - 1;
		bool do_ = true;
		int tmp = 0;

		cout << endl;
		for (int x = 0; x < vsize + 1; x++)
		{
			if (tmp > 2) {
				do_ = !do_;
				tmp = 0;
			}
			if (do_) {
				if (vertices[x] > 0) {
					cout << "Vertex Index " << x << "\t(" << vertices[x] << ")\tis now " << vertices[x] - 0.01 << endl;
					vertices[x] -= 0.01;
				}
				else if (vertices[x] < 0) {
					cout << "Vertex Index " << x << "\t(" << vertices[x] << ")\tis now " << vertices[x] + 0.01 << endl;
					vertices[x] += 0.01;
				}
			}
			tmp++;

		}
		cout << endl;
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		if (!ckeys.minus) {
		}
		ckeys.minus = true;
	}
	else {
		ckeys.minus = false;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		rotateTriangle(window, false);
		if (!ckeys.q) {
		}
		ckeys.q = true;
	}
	else {
		ckeys.q = false;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		rotateTriangle(window, true);
		if (!ckeys.e) {
		}
		ckeys.e = true;
	}
	else {
		ckeys.e = false;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {

		if (!ckeys.r) {
			for (int i = 0; i < sizeof(vertices) / sizeof(vertices[0]); ++i)
			{
				cout << "Vertices " << vertices[i] << " is now " << resetVertices[i] << endl;
				vertices[i] = resetVertices[i];

			}
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		}
		ckeys.r = true;
	}
	else {
		ckeys.r = false;
	}
}

void rotateTriangle(GLFWwindow* window, bool direction)
{
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	float fwidth = width;
	float fheight = height;

	int vsize = sizeof(vertices) / sizeof(vertices[0]) - 1;
	bool do_ = true;
	int tmp = 0;
	for (int x = 0; x < vsize + 1; x++)
	{
		if (do_ == false && tmp > 4) {
			do_ = !do_;
			tmp = 0;
		}
		if (do_ == true && tmp > 0) {
			do_ = !do_;
			tmp = 0;
		}
		if (do_) {
			float angle;

			if (vertices[x + 1] == 0 && vertices[x] == 0) {
				angle = 0;
			}

			if (!vertices[x+1] == 0 && !vertices[x] == 0) {
				// TYLER, we want pythag
				angle = abs(atan((vertices[x+1] / vertices[x])));

				// Bottom Left
				if (vertices[x+1] < 0 && vertices[x] < 0) { angle += PI; }

				// Bottom Right
				if (vertices[x+1] < 0 && vertices[x] > 0) { angle = -angle; }

				// Top Left
				if (vertices[x+1] > 0 && vertices[x] < 0) { angle = PI - angle; }

				// Top Right
				if (vertices[x+1] > 0 && vertices[x] > 0) {}

			}
			if (vertices[x+1] == 0 && vertices[x] > 0) { angle = 0; }

			if (vertices[x+1] == 0 && vertices[x] < 0) { angle = PI; }

			if (vertices[x+1] > 0 && vertices[x] == 0) { angle = PI / 2; }

			if (vertices[x+1] < 0 && vertices[x] == 0) { angle = (3 * PI) / 2; }


			float radius = sqrt((pow(vertices[x], 2) + pow(vertices[x+1], 2)));

			cout << "Point 1:(" << vertices[x] << "\t," << vertices[x+1] << "\t) is radius " << radius << "\t and angle " << angle * 180 / PI << ".\n";

			// Adds or removes a degree of rotation
			(direction == true ? angle += PI / 180 : angle -= PI / 180);

			vertices[x+1] = radius * sin(angle);
			vertices[x] = radius * cos(angle);
			//
			// TODO: Fix this stupid thing. If i resize the window to non-square, the rotation is fucky (not quite circular). 
			// I need to create a second array of vertices which is to be displayed, while there is another backend array of 
			// vertices to be manipulated without modification, otherwise even more fucky things happen.
			// (ALSO THE ZOOM IS SHITTY BECAUSE IT EXPANDS like y=x, it needs to use the angles, easy fix (more or less)
			// (ALSO ALSO make the zoom thingo a function pleaze.)
			//
		}
		tmp++;

	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

}

int main() { WinMain(); }