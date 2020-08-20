#define RUNMODE 1

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

using namespace std;

//
// Function Declaration
//

void processInput(GLFWwindow *window, unsigned int VBO, unsigned int EBO);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mainProcess(GLFWwindow* window, unsigned int shaderProgram, unsigned int VAO, unsigned int VBO, unsigned int EBO);

struct color { float r; float g; float b; float a; };
struct iter_ { bool r; bool g; bool b; };

//
// Background Variables
//

color background = { 0.0f, 0.5f, 1.0f, 1.0f };
iter_ backgrounditer = { true, true, true };
bool backgrounditerenable = false;

bool leftClick = false;
bool rightClick = false;

//
// Extra Variables
//
float vertices[] = {
	-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f,	// Bottom Left Mid
	 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f,	// Bottom Right Mid
	 0.0f,  0.5f, 0.0f, 0.5f, 1.0f, 0.0f	// Top Mid
};
unsigned int indices[] = {  // note that we start from 0!
	0, 1, 2
};

//
// BEGIN FUNCTIONS
// Main Function
//
int main() {
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

	GLFWwindow* window = glfwCreateWindow(800, 600, "Test OPENGL", NULL, NULL);

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
		if (!leftClick) {

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
		leftClick = true;
	}
	else {
		leftClick = false;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
		if (!rightClick) {
			backgrounditer.r = (rand() % 2 == 1 ? true : false);
			backgrounditer.g = (rand() % 2 == 1 ? true : false);
			backgrounditer.b = (rand() % 2 == 1 ? true : false);
			background.r = 0.5f;
			background.g = 0.5f;
			background.b = 0.5f;
			backgrounditerenable = !backgrounditerenable;
			cout << "Background Color Change: " << backgrounditerenable << endl;
		}
		rightClick = true;
	}
	else {
		rightClick = false;
	}
}