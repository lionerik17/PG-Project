#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"

#include <iostream>
#include "Airplane.cpp"

int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

glm::mat4 airportModelMatrix;
GLuint airportModelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::vec3 ground;

glm::mat4 airplaneModelMatrix;
GLuint airplaneModelLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(
	glm::vec3(-20.0f, 5.0f, -60.0f),
	glm::vec3(20.0f, 5.0f, -60.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.5f;
float airplaneSpeed = 5.0f;
float groundOffset = 2.5f;

glm::vec3 cameraOffset(0.0f, 5.0f, 20.0f);

bool pressedKeys[1024];

gps::Model3D airportModel;
gps::BoundingBox airportBoundingBox;
glm::vec3 airplanePosition(0.0f, 6.0f, -60.0f);
Airplane airplane(airplanePosition, glm::mat4(1.0f), 0);
gps::Shader myCustomShader;

gps::Model3D airplaneModel;
gps::BoundingBox airplaneBoundingBox;
GLuint objectIDLoc;

glm::vec3 lightPos;
GLuint lightPosLoc;

GLuint diffuseTexture;
GLuint specularTexture;
GLuint diffuseTextureLoc;
GLuint specularTextureLoc;

GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (width == 0 || height == 0) {
		return;
	}

	glWindowWidth = width;
	glWindowHeight = height;
	glfwGetFramebufferSize(window, &retina_width, &retina_height);
	glViewport(0, 0, retina_width, retina_height);
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void updateCameraPosition() {
	glm::vec3 airplanePosition = airplane.getPosition();
	glm::vec3 forwardDirection = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));

	glm::vec3 newCameraPosition = airplanePosition + forwardDirection * (-cameraOffset.z) + glm::vec3(0.0f, cameraOffset.y, 0.0f);

	myCamera.setPosition(newCameraPosition);
	myCamera.setTarget(airplanePosition);
}


void processMovement()
{
	glm::vec3 currentPosition = myCamera.getPosition();
	glm::vec3 newPosition = currentPosition;

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		airplane.moveForward(airplaneSpeed);
		newPosition = myCamera.getPosition();
		if (newPosition.y < ground.y) {
			currentPosition.y += 0.1f;
			myCamera.setPosition(currentPosition);
		}
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * airplaneModelMatrix));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		updateCameraPosition();
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
		airplane.moveBackward(airplaneSpeed);
		newPosition = myCamera.getPosition();
		if (newPosition.y < ground.y) {
			currentPosition.y += 0.1f;
			myCamera.setPosition(currentPosition);
		}
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * airplaneModelMatrix));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		updateCameraPosition();
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
		newPosition = myCamera.getPosition();
		if (newPosition.y < ground.y) {
			currentPosition.y += 0.1f;
			myCamera.setPosition(currentPosition);
		}
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * airplaneModelMatrix));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
		newPosition = myCamera.getPosition();
		if (newPosition.y < ground.y) {
			currentPosition.y += 0.1f;
			myCamera.setPosition(currentPosition);
		}
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * airplaneModelMatrix));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	if (pressedKeys[GLFW_KEY_UP]) {
		myCamera.rotate(cameraSpeed, 0.0f);
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * airplaneModelMatrix));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	if (pressedKeys[GLFW_KEY_DOWN]) {
		myCamera.rotate(-cameraSpeed, 0.0f);
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * airplaneModelMatrix));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	if (pressedKeys[GLFW_KEY_LEFT]) {
		myCamera.rotate(0.0f, -cameraSpeed);
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * airplaneModelMatrix));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	if (pressedKeys[GLFW_KEY_RIGHT]) {
		myCamera.rotate(0.0f, cameraSpeed);
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * airplaneModelMatrix));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	airplane.applyGravity();
	airplane.updateShader();
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);

	glfwMakeContextCurrent(glWindow);
	glfwSwapInterval(1);

#if not defined (__APPLE__)
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	airportModel.LoadModel("objects/airport/airport.obj", "objects/airport/");
	airportBoundingBox = airportModel.getBoundingBox();

	airplaneModel.LoadModel("objects/airplane/airplane.obj", "objects/airplane/");
	airplaneBoundingBox = airplaneModel.getBoundingBox();
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
}

void initUniforms() {
	objectIDLoc = glGetUniformLocation(myCustomShader.shaderProgram, "objectID");
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");

	airportModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)); // Airport is MASSIVE
	airportBoundingBox.transform(airportModelMatrix);
	airportModelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "airportModel");
	glUniformMatrix4fv(airportModelLoc, 1, GL_FALSE, glm::value_ptr(airportModelMatrix));

	airplaneModelMatrix = glm::translate(glm::mat4(1.0f), airplanePosition);
	airplaneModelMatrix = glm::scale(airplaneModelMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
	airplaneModelMatrix = glm::rotate(airplaneModelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	airplaneModelMatrix = glm::rotate(airplaneModelMatrix, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	airplaneBoundingBox.transform(airplaneModelMatrix);
	airplaneModelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "airplaneModel");
	glUniformMatrix4fv(airplaneModelLoc, 1, GL_FALSE, glm::value_ptr(airplaneModelMatrix));

	airplane = Airplane(airplanePosition, airplaneModelMatrix, airplaneModelLoc);
	ground = airportBoundingBox.getMin() + airplaneBoundingBox.getMin() + glm::vec3(0.0f, groundOffset, 0.0f);

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lightDir = glm::vec3(0.0f, -1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));

	lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightPos = glm::vec3(0.0f, 1.0f, 1.0f);
	lightPosLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos");
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

	diffuseTextureLoc = glGetUniformLocation(myCustomShader.shaderProgram, "diffuseTexture");
	glUniform1i(diffuseTextureLoc, 0);
	specularTextureLoc = glGetUniformLocation(myCustomShader.shaderProgram, "specularTexture");
	glUniform1i(specularTextureLoc, 1);
}

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniform1i(objectIDLoc, 0); // Set objectID to 0 for airport
	normalMatrix = glm::mat3(glm::inverseTranspose(view * airportModelMatrix));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	airportModel.Draw(myCustomShader);

	glUniform1i(objectIDLoc, 1); // Set objectID to 1 for airplane
	normalMatrix = glm::mat3(glm::inverseTranspose(view * airplaneModelMatrix));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	airplaneModel.Draw(myCustomShader);
}

void cleanup() {
	glfwDestroyWindow(glWindow);
	glfwTerminate();
}

int main(int argc, const char* argv[]) {
	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}