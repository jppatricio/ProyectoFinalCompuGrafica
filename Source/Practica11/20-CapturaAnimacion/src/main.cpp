//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>

//glfw include
#include <GLFW/glfw3.h>

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// program include
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"
// Geometry primitives
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
//Texture includes
#include "Headers/Texture.h"
//Model includes
#include "Headers/Model.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphereAnimacion(20, 20);
Cylinder cylinderAnimacion(20, 20, 0.5, 0.5);
Cylinder cylinderAnimacion2(20, 20, 0.5, 0.5);
Box box;

Sphere sphere(20, 20);
Cylinder cylinder(20, 20, 0.5, 0.5);
Cylinder cylinder2(12, 12, 0.6, 0.45);

Shader shaderColor;
Shader shaderTexture;
Shader shaderCubeTexture;
Shader shaderMateriales;
Shader shaderDirectionLight;
Shader shaderPointLight;
Shader shaderSpotLight;
Shader shaderLighting;

Model modelRock;
Model modelRail;
Model modelAirCraft;
Model arturito;
Model modelTrain;

GLuint textureID1, textureID2, textureID3, textureCespedID, textureWaterID, textureCubeTexture, pared_q;
GLuint cubeTextureID;

float rot1 = 0.0f, rot2 = 0.0, rot3 = 0.0, rot4 = 0.0, rot5 = 0.0;
float armAx1 = -0.3f;
float armAx2 = +0.3f;
bool saveFrame = false, availableSave = true;

GLenum types[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

std::string fileNames[6] = { 
	"../../Textures/ame_rocky/rockyvalley_ft.tga",
	"../../Textures/ame_rocky/rockyvalley_bk.tga",
	"../../Textures/ame_rocky/rockyvalley_up.tga",
	"../../Textures/ame_rocky/rockyvalley_dn.tga",
	"../../Textures/ame_rocky/rockyvalley_rt.tga",
	"../../Textures/ame_rocky/rockyvalley_lf.tga"
};

int screenWidth;
int screenHeight;

GLFWwindow * window;

bool exitApp = false;
int lastMousePosX, offsetX;
int lastMousePosY, offsetY;

double deltaTime;

std::string matToString(glm::mat4 matrix) {
	std::stringstream ss;
	const float *pSource = (const float*)glm::value_ptr(matrix);
	for (int i = 0; i < 16; ++i) {
		ss << pSource[i];
		if(i < 15)
			ss << ",";
	}
	return ss.str();
}

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroyWindow();
void destroy();
bool processInput(bool continueApplication = true);

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroyWindow();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
			exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	shaderColor.initialize("../../Shaders/transformaciones.vs", "../../Shaders/transformaciones.fs");
	shaderTexture.initialize("../../Shaders/texturizado_res.vs", "../../Shaders/texturizado_res.fs");
	shaderCubeTexture.initialize("../../Shaders/cubeTexture.vs", "../../Shaders/cubeTexture.fs");
	shaderMateriales.initialize("../../Shaders/iluminacion_materiales_res.vs", "../../Shaders/iluminacion_materiales_res.fs");
	shaderDirectionLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/directionalLight.fs");
	shaderPointLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/pointLight.fs");
	shaderSpotLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/spotLight.fs");
	shaderLighting.initialize("../../Shaders/typeLight.vs", "../../Shaders/multipleLights.fs");

	sphereAnimacion.init();
	sphereAnimacion.setShader(&shaderLighting);
	sphereAnimacion.setColor(glm::vec3(0.3, 0.3, 1.0));

	cylinderAnimacion.init();
	cylinderAnimacion.setShader(&shaderLighting);
	cylinderAnimacion.setColor(glm::vec3(0.8, 0.3, 1.0));

	cylinderAnimacion2.init();
	cylinderAnimacion2.setShader(&shaderLighting);
	cylinderAnimacion2.setColor(glm::vec3(0.2, 0.7, 0.3));

	sphere.init();
	sphere.setShader(&shaderLighting);
	sphere.setColor(glm::vec3(0.3, 0.3, 1.0));

	cylinder.init();
	cylinder.setShader(&shaderLighting);
	cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));

	cylinder2.init();
	cylinder2.setShader(&shaderLighting);
	cylinder2.setColor(glm::vec3(0.2, 0.7, 0.3));

	modelRock.loadModel("../../models/rock/rock.obj");
	modelRail.loadModel("../../models/railroad/railroad_track.obj");
	modelAirCraft.loadModel("../../models/Aircraft_obj/E 45 Aircraft_obj.obj");

	camera->setPosition(glm::vec3(0.0f, 0.0f, 0.4f));
	
	// Textura Ladrillos
	int imageWidth, imageHeight;
	Texture texture("../../Textures/texturaLadrillos.jpg");
	FIBITMAP* bitmap = texture.loadImage(false);
	unsigned char * data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID1);
	glBindTexture(GL_TEXTURE_2D, textureID1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	

	

	texture = Texture("../../Textures/cesped.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureCespedID);
	glBindTexture(GL_TEXTURE_2D, textureCespedID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	texture = Texture("../../Textures/water2.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureWaterID);
	glBindTexture(GL_TEXTURE_2D, textureWaterID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	texture = Texture("../../Textures/metal.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &pared_q);
	glBindTexture(GL_TEXTURE_2D, pared_q);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	glGenTextures(1, &cubeTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		texture = Texture(fileNames[i]);
		FIBITMAP* bitmap = texture.loadImage(true);
		data = texture.convertToData(bitmap, imageWidth, imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		}
		else
			std::cout << "Failed to load texture" << std::endl;
		texture.freeImage(bitmap);
	}

}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();

	shaderColor.destroy();
	shaderTexture.destroy();
	shaderCubeTexture.destroy();
	shaderMateriales.destroy();
	shaderDirectionLight.destroy();
	shaderPointLight.destroy();
	shaderSpotLight.destroy();
	sphereAnimacion.destroy();
	cylinderAnimacion.destroy();
	cylinderAnimacion2.destroy();
	box.destroy();
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, deltaTime);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}
	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			rot1 -= 0.005f;
		else
			rot1 += 0.005f;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			rot2 -= 0.005f;
		else
			rot2 += 0.005f;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			rot3 -= 0.005f;
		else
			rot3 += 0.005f;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			rot4 -= 0.005f;
		else
			rot4 += 0.005f;
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			rot5 -= 0.005f;
			armAx1 += 0.0005f;
			armAx2 += 0.0005f;
		}
		else {
			rot5 += 0.005f;
			armAx1 -= 0.0005f;
			armAx2 -= 0.0005f;
		}
	if (availableSave && glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		saveFrame = true;
		availableSave = false;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
		availableSave = true;
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	double lastTime = TimeManager::Instance().GetTime();

	float angle = 0.0;
	float ratio = 20.0;

	float aircraftZ = 0.0;
	bool direcionAirCraft = true;
	float rotationAirCraft = 0.0;
	bool finishRotation = true;
	std::stringstream ss;
	std::stringstream ss2;

	std::ofstream myfile;
	myfile.open("../../animaciones/animationMano.txt"); // FILE A DONDE SE VA A ESCRIBIR

	std::ofstream myFileKeyframe;
	myFileKeyframe.open("../../animaciones/keyframeAnim.txt"); // FILE A DONDE SE VA A ESCRIBIR

	while (psi) {
		psi = processInput(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Matrix de proyeccion en perspectiva
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / screenWidth, 0.01f, 100.0f);
		// matrix de vista
		glm::mat4 view = camera->getViewMatrix();

		ss.str("");
		ss2.str("");

		shaderLighting.turnOn();
		glUniform3fv(shaderLighting.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
		//Directional light
		glUniform3f(shaderLighting.getUniformLocation("directionalLight.light.ambient"), 0.025, 0.025, 0.025);
		glUniform3f(shaderLighting.getUniformLocation("directionalLight.light.diffuse"), 0.1, 0.1, 0.1);
		glUniform3f(shaderLighting.getUniformLocation("directionalLight.light.specular"), 0.15, 0.15, 0.15);
		glUniform3fv(shaderLighting.getUniformLocation("directionalLight.direction"), 1, glm::value_ptr(glm::vec3(0, -1.0, 0.0)));
		//Numero de luces spot y point
		glUniform1i(shaderLighting.getUniformLocation("pointLightCount"), 0);
		glUniform1i(shaderLighting.getUniformLocation("spotLightCount"), 1);
		// Spot light
		glUniform3fv(shaderLighting.getUniformLocation("spotLights[0].position"), 1, glm::value_ptr(camera->getPosition()));
		glUniform3fv(shaderLighting.getUniformLocation("spotLights[0].direction"), 1, glm::value_ptr(camera->getFront()));
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].outerCutOff"), glm::cos(glm::radians(15.0f)));
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].constant"), 1.0f);
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].linear"), 0.14f);
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].quadratics"), 0.07f);
		glUniform3f(shaderLighting.getUniformLocation("spotLights[0].light.ambient"), 0.025, 0.025, 0.025);
		glUniform3f(shaderLighting.getUniformLocation("spotLights[0].light.diffuse"), 0.7, 0.2, 0.6);
		glUniform3f(shaderLighting.getUniformLocation("spotLights[0].light.specular"), 0.1, 0.7, 0.8);
		shaderLighting.turnOff();

		cylinderAnimacion.setProjectionMatrix(projection);
		cylinderAnimacion.setViewMatrix(view);
		sphereAnimacion.setProjectionMatrix(projection);
		sphereAnimacion.setViewMatrix(view);

		// Se utiliza la textura de piel
		glBindTexture(GL_TEXTURE_2D, pared_q);
		
		glm::mat4 matrixL0 = glm::mat4(1.0f);
		
		// Se modela siempre con los ejes de giro en el eje z
		// CONVENCION DE DENAVIT-HATENBERG PARA CALCULAR LA CINEMATICA
		// DIRECTA DE UN CUERPO R�GIDO
		// Articulacion 1
		matrixL0 = glm::rotate(matrixL0, rot1, glm::vec3(0.0f, 0.0f, 1.0f));
		if (saveFrame)
			ss << matToString(matrixL0) << "|";// SE CONTATENA EN PIPES LAS MATRICES DE ROTACION (igual pa todas)
		glm::mat4 cylinderMatrixJ0 = glm::rotate(matrixL0, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
		cylinderMatrixJ0 = glm::scale(cylinderMatrixJ0, glm::vec3(0.08f, 0.08f, 0.08f));
		sphereAnimacion.render(cylinderMatrixJ0);
		
		// Articulacion 2
		matrixL0 = glm::rotate(matrixL0, 1.5708f, glm::vec3(1.0f, 0.0f, 0.0f));
		matrixL0 = glm::rotate(matrixL0, 1.5708f + rot2, glm::vec3(0.0f, 0.0f, 1.0f));
		if (saveFrame)
			ss << matToString(matrixL0) << "|";
		glm::mat4 cylinderMatrixJ1 = glm::rotate(matrixL0, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
		cylinderMatrixJ1 = glm::scale(cylinderMatrixJ1, glm::vec3(0.08f, 0.08f, 0.08f));
		sphereAnimacion.render(cylinderMatrixJ1);
		
		// Articulacion 3
		matrixL0 = glm::rotate(matrixL0, 1.5708f, glm::vec3(1.0f, 0.0f, 0.0f));
		matrixL0 = glm::rotate(matrixL0, rot3, glm::vec3(0.0f, 0.0f, 1.0f));
		if (saveFrame)
			ss << matToString(matrixL0) << "|";
		glm::mat4 cylinderMatrixJ2 = glm::rotate(matrixL0, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
		cylinderMatrixJ2 = glm::scale(cylinderMatrixJ2, glm::vec3(0.08f, 0.08f, 0.08f));
		sphereAnimacion.render(cylinderMatrixJ2);

		// Hueso 1
		glm::mat4 cylinderMatrixL1 = glm::translate(matrixL0, glm::vec3(0.0f, 0.0f, 0.125f));
		cylinderMatrixL1 = glm::rotate(cylinderMatrixL1, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
		cylinderMatrixL1 = glm::scale(cylinderMatrixL1, glm::vec3(0.1f, 0.25f, 0.1f));
		cylinderAnimacion.render(cylinderMatrixL1);

		// Articulacion 4
		matrixL0 = glm::translate(matrixL0,glm::vec3(0.0f, 0.0f, 0.25f));
		matrixL0 = glm::rotate(matrixL0, rot4, glm::vec3(0.0f, 0.0f, 1.0f));
		if (saveFrame)
			ss << matToString(matrixL0) << "|";
		glm::mat4 cylinderMatrixJ3 = glm::rotate(matrixL0, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
		cylinderMatrixJ3 = glm::scale(cylinderMatrixJ3, glm::vec3(0.08f, 0.08f, 0.08f));
		sphereAnimacion.render(cylinderMatrixJ3);

		// Articulacion 5
		matrixL0 = glm::rotate(matrixL0, 1.5708f, glm::vec3(1.0f, 0.0f, 0.0f));
		matrixL0 = glm::rotate(matrixL0, rot5, glm::vec3(0.0f, 0.0f, 1.0f));
		if (saveFrame)
			ss << matToString(matrixL0) << "|";
		glm::mat4 cylinderMatrixJ4 = glm::rotate(matrixL0, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
		cylinderMatrixJ4 = glm::scale(cylinderMatrixJ4, glm::vec3(0.08f, 0.08f, 0.08f));
		sphereAnimacion.render(cylinderMatrixJ4);

		// Hueso 2
		glm::mat4 cylinderMatrixL2 = glm::translate(matrixL0, glm::vec3(0.0f, 0.0f, 0.125f));
		cylinderMatrixL2 = glm::rotate(cylinderMatrixL2, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
		cylinderMatrixL2 = glm::scale(cylinderMatrixL2, glm::vec3(0.1f, 0.25f, 0.1f));
		cylinderAnimacion.render(cylinderMatrixL2);

		// Articulacion 6
		matrixL0 = glm::translate(matrixL0, glm::vec3(0.0f, 0.0f, 0.25f));
		if (saveFrame)
			ss << matToString(matrixL0) << "|";
		glm::mat4 cylinderMatrixJ5 = glm::rotate(matrixL0, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
		cylinderMatrixJ5 = glm::scale(cylinderMatrixJ5, glm::vec3(0.08f, 0.08f, 0.08f));
		sphereAnimacion.render(cylinderMatrixJ5);

		if (saveFrame) {
			myfile << ss.str() << "|" << std::endl; // se concatenan
		}

		//::::::::::::::::::::::::::::::::: ANIMACION POR KEYFRAMES
		//----
		glm::mat4 matrix0 = glm::mat4(1.0f);
		matrix0 = glm::translate(matrix0, glm::vec3(0.0f, 0.0f, -1.0f));

		glm::mat4 matrixs1 = glm::translate(matrix0, glm::vec3(0.0f, -0.5f, 0.0f));

		glm::mat4 matrixs2 = glm::translate(matrixs1, glm::vec3(-0.225f, 0.0f, 0.0f));

		glm::mat4 matrix1 = glm::rotate(matrixs2, -0.2f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix1 = glm::rotate(matrixs2, -1.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		matrix1 = glm::translate(matrix1, glm::vec3(0.0, -0.4, 0.0));

		glm::mat4 matrixs4 = glm::translate(matrix1, glm::vec3(0.0f, -0.4f, 0.0f));

		glm::mat4 matrix2 = glm::rotate(matrixs4, 0.3f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix2 = glm::rotate(matrixs4, 1.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		matrix2 = glm::translate(matrix2, glm::vec3(0.0f, -0.3f, 0.0f));

		glm::mat4 matrixs12 = glm::translate(matrix2, glm::vec3(0.0f, -0.3f, 0.0f));

		glm::mat4 matrix7 = glm::rotate(matrixs12, 1.5f, glm::vec3(-1.0f, 0.0f, 0.0f));
		matrix7 = glm::translate(matrix7, glm::vec3(0.0f, -0.1f, 0.0f));
		matrix7 = glm::scale(matrix7, glm::vec3(0.1f, 0.2f, 0.1f));

		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix7);
		//---
		matrixs12 = glm::scale(matrixs12, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs12);

		matrix2 = glm::scale(matrix2, glm::vec3(0.1, 0.6, 0.2));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix2);
		matrixs4 = glm::scale(matrixs4, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs4);
		matrix1 = glm::scale(matrix1, glm::vec3(0.15f, 0.8f, 0.15f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix1);
		matrixs2 = glm::scale(matrixs2, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs2);

		glm::mat4 matrixs3 = glm::translate(matrixs1, glm::vec3(0.225f, 0.0f, 0.0f));
		//pierna izquierda
		glm::mat4 matrix8 = glm::rotate(matrixs3, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix8 = glm::rotate(matrix8, -1.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		matrix8 = glm::translate(matrix8, glm::vec3(0.0, -0.4, 0.0));

		glm::mat4 matrixs13 = glm::translate(matrix8, glm::vec3(0.0f, -0.4f, 0.0f));

		glm::mat4 matrix9 = glm::rotate(matrixs13, 0.3f, glm::vec3(0.0f, 0.0f, -1.0f));
		matrix9 = glm::rotate(matrixs13, 1.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		matrix9 = glm::translate(matrix9, glm::vec3(0.0f, -0.3f, 0.0f));

		//pie
		glm::mat4 matrixs14 = glm::translate(matrix9, glm::vec3(0.0f, -0.3f, 0.0f));
		glm::mat4 matrix10 = glm::rotate(matrixs14, 1.5f, glm::vec3(-1.0f, 0.0f, 0.0f));
		matrix10 = glm::translate(matrix10, glm::vec3(0.0f, -0.1f, 0.0f));
		matrix10 = glm::scale(matrix10, glm::vec3(0.1f, 0.2f, 0.1f));

		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix10);
		matrixs14 = glm::scale(matrixs14, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs14);
		matrix9 = glm::scale(matrix9, glm::vec3(0.1, 0.6, 0.2));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix9);
		matrixs13 = glm::scale(matrixs13, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs13);
		matrix8 = glm::scale(matrix8, glm::vec3(0.15f, 0.8f, 0.15f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix8);
		matrixs3 = glm::scale(matrixs3, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs3);
		matrixs1 = glm::scale(matrixs1, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs1);

		glm::mat4 matrixs5 = glm::translate(matrix0, glm::vec3(0.0f, 0.5f, 0.0f));
		//CABEZA
		glm::mat4 matrix11 = glm::mat4(1.0f);
		matrix11 = glm::translate(matrixs5, glm::vec3(0.0f, 0.5f, 0.0f));
		matrix11 = glm::scale(matrix11, glm::vec3(0.4f, 0.6f, 0.4f));
		cylinder2.setProjectionMatrix(projection);
		cylinder2.setViewMatrix(view);
		cylinder2.enableWireMode();
		cylinder2.render(matrix11);

		//		BRAZO DERECHO
		glm::mat4 matrixs6 = glm::translate(matrixs5, glm::vec3(0.3f, 0.0f, 0.0f));
		glm::mat4 matrix3 = glm::rotate(matrixs6, -0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix3 = glm::rotate(matrix3, -1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
		matrix3 = glm::rotate(matrix3, armAx1, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix3 = glm::translate(matrix3, glm::vec3(0.25f, 0.0f, 0.0f));

		if (saveFrame)
			ss2 << matToString(matrix3) << "|";// SE CONTATENA EN PIPES LAS MATRICES DE ROTACION (igual pa todas)

		glm::mat4 matrixs8 = glm::translate(matrix3, glm::vec3(0.3f, 0.0f, 0.0f));
		glm::mat4 matrix5 = glm::rotate(matrixs8, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix5 = glm::rotate(matrix5, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		matrix5 = glm::rotate(matrix5, 1.5f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix5 = glm::translate(matrix5, glm::vec3(0.0f, -0.3f, 0.0f));
		glm::mat4 matrixs9 = glm::translate(matrix5, glm::vec3(0.0f, -0.3f, 0.0f));
		matrixs9 = glm::scale(matrixs9, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs9);
		matrix5 = glm::scale(matrix5, glm::vec3(0.1, 0.6, 0.2));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix5);
		matrixs8 = glm::scale(matrixs8, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs8);
		matrix3 = glm::scale(matrix3, glm::vec3(0.5f, 0.15f, 0.15f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix3);
		matrixs6 = glm::scale(matrixs6, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs6);

		//		BRAZO IZQUIERDO
		glm::mat4 matrixs7 = glm::translate(matrixs5, glm::vec3(-0.3f, 0.0f, 0.0f));
		glm::mat4 matrix4 = glm::rotate(matrixs7, 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix4 = glm::rotate(matrix4, 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
		matrix4 = glm::rotate(matrix4, armAx2, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix4 = glm::translate(matrix4, glm::vec3(-0.25f, 0.0f, 0.0f));

		if (saveFrame)
			ss2 << matToString(matrix4) << "|";// SE CONTATENA EN PIPES LAS MATRICES DE ROTACION (igual pa todas)
		glm::mat4 matrixs10 = glm::translate(matrix4, glm::vec3(-0.3f, 0.0f, 0.0f));
		glm::mat4 matrix6 = glm::rotate(matrixs10, 0.0f, glm::vec3(0.0f, 0.0f, -1.0f));
		matrix6 = glm::rotate(matrix6, -0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		matrix6 = glm::rotate(matrix6, -1.5f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix6 = glm::translate(matrix6, glm::vec3(0.0f, -0.3f, 0.0f));
		glm::mat4 matrixs11 = glm::translate(matrix6, glm::vec3(0.0f, -0.3f, 0.0f));
		matrixs11 = glm::scale(matrixs11, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs11);
		matrix6 = glm::scale(matrix6, glm::vec3(0.1, 0.6, 0.2));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix6);
		matrixs10 = glm::scale(matrixs10, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs10);
		matrix4 = glm::scale(matrix4, glm::vec3(0.5f, 0.15f, 0.15f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix4);
		matrixs7 = glm::scale(matrixs7, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs7);

		matrixs5 = glm::scale(matrixs5, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs5);

		matrix0 = glm::scale(matrix0, glm::vec3(0.6f, 1.0f, 0.6f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix0);
		//----
		if (saveFrame) {
			myFileKeyframe << ss2.str() << "|" << std::endl; // se concatenan
			saveFrame = false;
		}
		//:::::::::::::::::::::::::::::::::

		glfwSwapBuffers(window);
	}
	myFileKeyframe.close();
	myfile.close();// se cierra el archivo
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}