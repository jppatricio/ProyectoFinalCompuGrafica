//glew include
#include <GL/glew.h>

//std includes
#include <sstream>
#include <string>
#include <iostream>
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

Sphere sphere(20, 20);
Cylinder cylinder(20, 20, 0.5, 0.5);
Box box;
Box boxWater;
Box windowsBox;
Box boxStone;
Box windowsBanio;
Box leaveBox;

Sphere sphereAnimacion(20, 20);
Cylinder cylinderAnimacion(20, 20, 0.5, 0.5);
Cylinder cylinderAnimacion2(20, 20, 0.5, 0.5);
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
Model modelDesk;
Model modelCPU;
Model chair;
Model tree;
Model leaves;
Model busto;
Model pedestal;
Model avion;
void animacionProgramador(glm::mat4 view, glm::mat4 projection);

GLuint textureID1, textureCespedID, textureWaterID, pared_q, puerta_principal, ventana1, ventana2,
 ventana3, piedra, ventanaLab, windowsImg, keyboard, pizarron, tronco, hojas, bronce, ventana4, papel;
GLuint cubeTextureID;

float interpolation = 0.0;

//::::::::::::::::::::::::::::::::::::::::::A
//OBTENCION DE LOS KEYFRAMES DEL ARCHIVO---------
std::vector<std::vector<glm::mat4>> keyFramesProg;
//NUMERO DE PASOS ENTRE KEYFRAMES (I a I+1)
int numPasosAnimProg = 20;
int numPasosAnimProgCurr = 0;

// Indices del arreglo keyFramesBrazo el actual y el siguiente
int indexKeyFrameProgCurr = 0;
int indexKeyFrameProgNext = 1;
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

std::vector<std::vector<glm::mat4>> getKeyFrames(std::string fileName) {
	std::vector<std::vector<glm::mat4>> keyFrames;
	std::string line;
	std::ifstream infile(fileName);
	std::string s = line;
	while (std::getline(infile, line))
	{
		std::vector<glm::mat4> transforms;
		s = line;
		size_t pos1 = 0;
		std::string token1;
		std::string delimiter1 = "|";// ES LA MATRIZ 0,0,0,0,0,0,0,0|...
		while ((pos1 = s.find(delimiter1)) != std::string::npos) {
			token1 = s.substr(0, pos1);
			//std::cout << token1 << std::endl;

			size_t pos2 = 0;
			std::string token2;
			std::string delimiter2 = ",";

			int i = 0;
			int j = 0;
			glm::mat4 transform;
			float aaa[16];
			while ((pos2 = token1.find(delimiter2)) != std::string::npos) {
				token2 = token1.substr(0, pos2);
				aaa[i++] = atof(token2.c_str());
				/*if (j == 0)
					transform[i].x = atof(token2.c_str());
				if (j == 1)
					transform[i].y = atof(token2.c_str());
				if (j == 2)
					transform[i].z = atof(token2.c_str());
				if (j == 3)
					transform[i].w = atof(token2.c_str());*/
				/*j++;
				if (j > 3) {
					i++;
					j = 0;
				}*/
				token1.erase(0, pos2 + delimiter2.length());
			}
			aaa[i++] = atof(token1.c_str());
			transform = glm::make_mat4(aaa);
			transforms.push_back(transform);
			s.erase(0, pos1 + delimiter1.length());
		}
		keyFrames.push_back(transforms);
	}
	return keyFrames;
}

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

bool actionAnim = false, animFinished = true, cerrada = true;
float zDoorSlide = -17.66;
//Variables de animacion hoja
float hojaX = 1.0, hojaY, hojaYaux = 14.0, hojaZ, angulo = 0.0f, zSuma;
bool vuelta1 = true;
bool vuelta2 = false;
//Variables de animacion avion
float avionX, avionZ, ang = 0.0f;

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

	sphere.init();
	cylinder.init();
	box.init();
	box.scaleUVS(glm::vec2(100.0, 100.0));
	boxWater.init();
	boxWater.scaleUVS(glm::vec2(1.0, 1.0));
	windowsBox.init();
	windowsBox.scaleUVS(glm::vec2(1.0, 1.0));
	windowsBanio.init();
	windowsBanio.scaleUVS(glm::vec2(1.0, 1.0));
	boxStone.init();
	boxStone.scaleUVS(glm::vec2(1.0, 1.0));
	leaveBox.init();
	leaveBox.scaleUVS(glm::vec2(1.0, 1.0));

	modelRock.loadModel("../../models/rock/rock.obj");
	modelRail.loadModel("../../models/railroad/railroad_track.obj");
	modelAirCraft.loadModel("../../models/Aircraft_obj/E 45 Aircraft_obj.obj");
	modelDesk.loadModel("../../models/desk.3ds");
	modelCPU.loadModel("../../models/cpu/monitor_win10.fbx");
	chair.loadModel("../../models/willisau_modica_armchair_cantilever_3ds/willisau_modica_armchair_cantilever_mat(1)_3ds.3ds");
	tree.loadModel("../../models/TreeSet3/TreeSet3.fbx");
	leaves.loadModel("../../models/TreeSet3/Leaves3.fbx");
	busto.loadModel("../../models/NelsonMandela/16097_NelsonMandela_V3.obj");
	pedestal.loadModel("../../models/pedestal/10421_square_pedastal_iterations-2.obj");
	avion.loadModel("../../models/Bomber.obj");

	camera->setPosition(glm::vec3(0.0f, 17.0f, 30.0f));
	camera->setYawPitch(-89.0f, -25.0f);
	
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

	//textura cesped
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

	//texturas-----------------------------:::::::::::::::::::::::::::VENTANAS::::::::::::::::::::::::::::::
	texture = Texture("../../Textures/puerta_principal.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &puerta_principal);
	glBindTexture(GL_TEXTURE_2D, puerta_principal);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	texture = Texture("../../Textures/ventana1.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &ventana1);
	glBindTexture(GL_TEXTURE_2D, ventana1);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	texture = Texture("../../Textures/ventana2.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &ventana2);
	glBindTexture(GL_TEXTURE_2D, ventana2);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	texture = Texture("../../Textures/ventana3cortinas.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &ventana3);
	glBindTexture(GL_TEXTURE_2D, ventana3);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	texture = Texture("../../Textures/ventana4.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &ventana4);
	glBindTexture(GL_TEXTURE_2D, ventana4);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	//:::::::::::::::::::::::::::::::::::::PIEDRA::::::::::::::::::::::::::::::::::
	/*texture = Texture("../../Textures/cobblestone.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &piedra);
	glBindTexture(GL_TEXTURE_2D, piedra);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);*/
	texture = Texture("../../Textures/piedranegra.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &piedra);
	glBindTexture(GL_TEXTURE_2D, piedra);
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

	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//textura ventanasLab
	texture = Texture("../../Textures/ventanaLab.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &ventanaLab);
	glBindTexture(GL_TEXTURE_2D, ventanaLab);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	//
	//textura ventanasLab
	texture = Texture("../../Textures/win10.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &windowsImg);
	glBindTexture(GL_TEXTURE_2D, windowsImg);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);
	//keyboards
	texture = Texture("../../Textures/keyboard.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &keyboard);
	glBindTexture(GL_TEXTURE_2D, keyboard);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// pizarron
	texture = Texture("../../Textures/pizarron.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &pizarron);
	glBindTexture(GL_TEXTURE_2D, pizarron);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//textura metal
	texture = Texture("../../Textures/pared_q.png");
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

	//textura metal
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

	//Tronco
	texture = Texture("../../Textures/BarkDecidious0194_7_S.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &tronco);
	glBindTexture(GL_TEXTURE_2D, tronco);
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

	//Hojas
	texture = Texture("../../Textures/Leaves0017_1_S.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &hojas);
	glBindTexture(GL_TEXTURE_2D, hojas);
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

	// Bronce
		texture = Texture("../../Textures/bronce.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &bronce);
	glBindTexture(GL_TEXTURE_2D, bronce);
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

	//Papel
	texture = Texture("../../Textures/papel.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &papel);
	glBindTexture(GL_TEXTURE_2D, papel);
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


	// SKYBOX
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
	sphere.destroy();
	cylinder.destroy();

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
	//Tecla F para ir al frente
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		camera->setPosition(glm::vec3(0.0f, 17.0f, 30.0f));
		camera->setYawPitch(-89.0f, -25.0f);
	}	
	//Tecla G para isometrico
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		camera->setYawPitch(-50.0f,-25.0f);
		camera->setPosition(glm::vec3(-38.0f, 25.0f, 50.0f));
	}
	//Tecla H para ir al salon
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		camera->setPosition(glm::vec3(8.5f, 10.0f, -18.0f));
		camera->setYawPitch(0.0f, -0.3f);
	}
	//Tecla E para interactuar con puerta
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		actionAnim = true;
	}
		
	glfwPollEvents();
	return continueApplication;
}

void renderizarSalon(glm::mat4 view, glm::mat4 projection) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ventanaLab);
	windowsBox.setShader(&shaderLighting);
	windowsBox.setProjectionMatrix(projection);
	windowsBox.setViewMatrix(view);
	float x = 10; // Para una esquina del laboratorio
	float z = -19;
	float y = 10; // altura predet del nivel
	//:::::::::::::::::::::::::::::::ventanas
	for (float i = 0; i < 7; i += 1.33) {
		windowsBox.setPosition(glm::vec3(x, y, z - i));
		windowsBox.setScale(glm::vec3(.1, 2, 1.33));
		windowsBox.render();
	}
	for (float i = 0; i < 7; i = i + 1.33) {
		windowsBox.setPosition(glm::vec3(x + i + 0.5, y, z + 2));
		windowsBox.setScale(glm::vec3(1.33, 2, .1));
		windowsBox.render();
	}
	for (float i = 0; i < 10; i = i + 1.33) {
		windowsBox.setPosition(glm::vec3(x + i - 2.3, y, z - 7.31));
		windowsBox.setScale(glm::vec3(1.33, 2, .1));
		windowsBox.render();
	}

	// ESCRITORIOS ::::::::::::::::
	modelDesk.setShader(&shaderLighting);
	modelDesk.setProjectionMatrix(projection);
	modelDesk.setViewMatrix(view);
	modelDesk.setScale(glm::vec3(1.0, 1.0, 1.0));
	modelDesk.setOrientation(glm::vec3(-90, 90, 0));

	modelDesk.setPosition(glm::vec3(x + .9, y - 1, z - 4.5));
	modelDesk.render();
	modelDesk.setPosition(glm::vec3(x + 2.3, y - 1, z - 4.5));
	modelDesk.render();

	modelDesk.setPosition(glm::vec3(x + .9, y - 1, z - 2.5));
	modelDesk.render();
	modelDesk.setPosition(glm::vec3(x + 2.3, y - 1, z - 2.5));
	modelDesk.render();

	modelDesk.setPosition(glm::vec3(x + .9, y - 1, z - .5));
	modelDesk.render();
	modelDesk.setPosition(glm::vec3(x + 2.3, y - 1, z - .5));
	modelDesk.render();
	//
	modelDesk.setPosition(glm::vec3(x + 5.5, y - 1, z - 4.5));
	modelDesk.render();
	modelDesk.setPosition(glm::vec3(x + 6.9, y - 1, z - 4.5));
	modelDesk.render();

	modelDesk.setPosition(glm::vec3(x + 5.5, y - 1, z - 2.5));
	modelDesk.render();
	modelDesk.setPosition(glm::vec3(x + 6.9, y - 1, z - 2.5));
	modelDesk.render();

	modelDesk.setPosition(glm::vec3(x + 5.5, y - 1, z - .5));
	modelDesk.render();
	modelDesk.setPosition(glm::vec3(x + 6.9, y - 1, z - .5));
	modelDesk.render();
	// MONITORES :::::::::::
	glBindTexture(GL_TEXTURE_2D, windowsImg);

	modelCPU.setShader(&shaderLighting);
	modelCPU.setProjectionMatrix(projection);
	modelCPU.setViewMatrix(view);
	modelCPU.setScale(glm::vec3(1, 1, 1));
	modelCPU.setOrientation(glm::vec3(-90, 0, 0));

	modelCPU.setPosition(glm::vec3(x + .7, y - 0.24, z - 4.5));
	modelCPU.render();
	modelCPU.setPosition(glm::vec3(x + 1.6, y - 0.24, z - 4.5));
	modelCPU.render();
	modelCPU.setPosition(glm::vec3(x + 2.5, y - 0.24, z - 4.5));
	modelCPU.render();

	modelCPU.setPosition(glm::vec3(x + .7, y - 0.24, z - 2.5));
	modelCPU.render();
	modelCPU.setPosition(glm::vec3(x + 1.6, y - 0.24, z - 2.5));
	modelCPU.render();
	modelCPU.setPosition(glm::vec3(x + 2.5, y - 0.24, z - 2.5));
	modelCPU.render();

	modelCPU.setPosition(glm::vec3(x + .7, y - 0.24, z - .5));
	modelCPU.render();
	modelCPU.setPosition(glm::vec3(x + 1.6, y - 0.24, z - .5));
	modelCPU.render();
	modelCPU.setPosition(glm::vec3(x + 2.5, y - 0.24, z - .5));
	modelCPU.render();
	//-----
	modelCPU.setPosition(glm::vec3(x + .7 + 4.6, y - 0.24, z - 4.5));
	modelCPU.render();
	modelCPU.setPosition(glm::vec3(x + 1.6 + 4.6, y - 0.24, z - 4.5));
	modelCPU.render();
	modelCPU.setPosition(glm::vec3(x + 2.5 + 4.6, y - 0.24, z - 4.5));
	modelCPU.render();

	modelCPU.setPosition(glm::vec3(x + .7 + 4.6, y - 0.24, z - 2.5));
	modelCPU.render();
	modelCPU.setPosition(glm::vec3(x + 1.6 + 4.6, y - 0.24, z - 2.5));
	modelCPU.render();
	modelCPU.setPosition(glm::vec3(x + 2.5 + 4.6, y - 0.24, z - 2.5));
	modelCPU.render();

	modelCPU.setPosition(glm::vec3(x + .7 + 4.6, y - 0.24, z - .5));
	modelCPU.render();
	modelCPU.setPosition(glm::vec3(x + 1.6 + 4.6, y - 0.24, z - .5));
	modelCPU.render();
	modelCPU.setPosition(glm::vec3(x + 2.5 + 4.6, y - 0.24, z - .5));
	modelCPU.render();
	//Teclados :::::::::::::::::::::
	glBindTexture(GL_TEXTURE_2D, keyboard);
	windowsBox.setShader(&shaderLighting);
	windowsBox.setProjectionMatrix(projection);
	windowsBox.setViewMatrix(view);
	windowsBox.setScale(glm::vec3(.35, .01, .2));
	windowsBox.setPosition(glm::vec3(x + .7, y - 0.24, .1 + z - 4.5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + 1.6, y - 0.24, .1 + z - 4.5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + 2.5, y - 0.24, .1 + z - 4.5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + .7, y - 0.24, .1 + z - 2.5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + 1.6, y - 0.24, .1 + z - 2.5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + 2.5, y - 0.24, .1 + z - 2.5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + .7, y - 0.24, .1 + z - .5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + 1.6, y - 0.24, .1 + z - .5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + 2.5, y - 0.24, .1 + z - .5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + .7 + 4.6, y - 0.24, .1 + z - 4.5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + 1.6 + 4.6, y - 0.24, .1 + z - 4.5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + 2.5 + 4.6, y - 0.24, .1 + z - 4.5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + .7 + 4.6, y - 0.24, .1 + z - 2.5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + 1.6 + 4.6, y - 0.24, .1 + z - 2.5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + 2.5 + 4.6, y - 0.24, .1 + z - 2.5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + .7 + 4.6, y - 0.24, .1 + z - .5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + 1.6 + 4.6, y - 0.24, .1 + z - .5));
	windowsBox.render();
	windowsBox.setPosition(glm::vec3(x + 2.5 + 4.6, y - 0.24, .1 + z - .5));
	windowsBox.render();
	// SILLAS ::::::::::::::
	chair.setShader(&shaderLighting);
	chair.setProjectionMatrix(projection);
	chair.setViewMatrix(view);
	chair.setScale(glm::vec3(.01, .01, .01));
	chair.setOrientation(glm::vec3(-90, 180, 0));

	chair.setPosition(glm::vec3(x + .7, y - 1, z - 3.8));
	chair.render();
	chair.setPosition(glm::vec3(x + 1.6, y - 1, z - 3.8));
	chair.render();
	chair.setPosition(glm::vec3(x + 2.5, y - 1, z - 3.8));
	chair.render();
	chair.setPosition(glm::vec3(x + .7, y - 1, z - 1.8));
	chair.render();
	chair.setPosition(glm::vec3(x + 1.6, y - 1, z - 1.8));
	chair.render();
	chair.setPosition(glm::vec3(x + 2.5, y - 1, z - 1.8));
	chair.render();
	chair.setPosition(glm::vec3(x + .7, y - 1, z + 0.2));
	chair.render();
	chair.setPosition(glm::vec3(x + 1.6, y - 1, z + 0.2));
	chair.render();
	chair.setPosition(glm::vec3(x + 2.5, y - 1, z + 0.2));
	chair.render();
	chair.setPosition(glm::vec3(x + .7 + 4.6, y - 1, z - 3.8));
	chair.render();
	chair.setPosition(glm::vec3(x + 1.6 + 4.6, y - 1, z - 3.8));
	chair.render();
	chair.setPosition(glm::vec3(x + 2.5 + 4.6, y - 1, z - 3.8));
	chair.render();
	chair.setPosition(glm::vec3(x + .7 + 4.6, y - 1, z - 1.8));
	chair.render();
	chair.setPosition(glm::vec3(x + 1.6 + 4.6, y - 1, z - 1.8));
	chair.render();
	chair.setPosition(glm::vec3(x + 2.5 + 4.6, y - 1, z - 1.8));
	chair.render();
	chair.setPosition(glm::vec3(x + .7 + 4.6, y - 1, z + 0.2));
	chair.render();
	chair.setPosition(glm::vec3(x + 1.6 + 4.6, y - 1, z + 0.2));
	chair.render();
	chair.setPosition(glm::vec3(x + 2.5 + 4.6, y - 1, z + 0.2));
	chair.render();
	// PIZARRON ::::::::::::::::::::
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pizarron);
	windowsBox.setShader(&shaderLighting);
	windowsBox.setProjectionMatrix(projection);
	windowsBox.setViewMatrix(view);
	windowsBox.setScale(glm::vec3(2.66, 1.33, .01));
	windowsBox.setPosition(glm::vec3(x + 4, y, .1 + z - 7.33));
	windowsBox.render();
}

void renderizarEdificio(glm::mat4 view, glm::mat4 projection) {

	//::::::::::::::::::::::::::::::::: SOLID WALLS ::::::::::::::::::::::::::::::::::
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pared_q);
	box.setShader(&shaderLighting);
	box.setProjectionMatrix(projection);
	box.setViewMatrix(view);
	//1
	box.setPosition(glm::vec3(-12.5, 5.5, 25));
	box.setScale(glm::vec3(11.0, 11, 0.3));
	box.render();
	//2
	box.setPosition(glm::vec3(-7.5, 5.5, 20));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//3
	box.setPosition(glm::vec3(-7.5, 5.5, 15));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//4
	box.setPosition(glm::vec3(-7.5, 5.5, 10));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//5
	box.setPosition(glm::vec3(-7, 5.5, 5));
	box.setScale(glm::vec3(1.0, 11, .5));
	box.render();
	//6
	box.setPosition(glm::vec3(-2.33, 5.5, 4.5));
	box.setScale(glm::vec3(0.3, 11, 1.0));
	box.render();
	//7
	box.setPosition(glm::vec3(2.33, 5.5, 4.5));
	box.setScale(glm::vec3(0.3, 11, 1.0));
	box.render();
	//8
	box.setPosition(glm::vec3(7, 5.5, 5));
	box.setScale(glm::vec3(1.0, 11, 1.0));
	box.render();
	//9
	box.setPosition(glm::vec3(7.5, 5.5, 10));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//10 
	box.setPosition(glm::vec3(12.5, 5.5, 15));
	box.setScale(glm::vec3(11, 11, 0.3));
	box.render();

	//11
	box.setPosition(glm::vec3(7, 5.5, -5.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//12
	box.setPosition(glm::vec3(7.5, 5.5, -10.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//13
	box.setPosition(glm::vec3(7.5, 5.5, -15.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//14
	box.setPosition(glm::vec3(7.5, 5.5, -20.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//15
	box.setPosition(glm::vec3(7.5, 5.5, -25.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//16 
	box.setPosition(glm::vec3(12.5, 5.5, -31.0));
	box.setScale(glm::vec3(11, 11, 0.3));
	box.render();

	//17
	box.setPosition(glm::vec3(17.5, 5.5, 10.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//18
	box.setPosition(glm::vec3(17.5, 5.5, 4.25));
	box.setScale(glm::vec3(1.25, 11, 3.5));
	box.render();
	//19
	box.setPosition(glm::vec3(17.5, 5.5, 0.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//20
	box.setPosition(glm::vec3(17.5, 5.5, -5.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//21
	box.setPosition(glm::vec3(17.5, 5.5, -10.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//22
	box.setPosition(glm::vec3(17.5, 5.5, -15.75));
	box.setScale(glm::vec3(1.25, 11, 3.5));
	box.render();
	//22
	box.setPosition(glm::vec3(17.5, 5.5, -20.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//23
	box.setPosition(glm::vec3(17.5, 5.5, -25.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();

	//24
	box.setPosition(glm::vec3(-12.5, 5.5, -10.0));
	box.setScale(glm::vec3(11.0, 11, 0.3));
	box.render();
	//25
	box.setPosition(glm::vec3(-17.5, 5.5, -5.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//26
	box.setPosition(glm::vec3(-17.5, 5.5, 0.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//27
	box.setPosition(glm::vec3(-17.5, 5.5, 5.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//28
	box.setPosition(glm::vec3(-17.5, 5.5, 10.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//29
	box.setPosition(glm::vec3(-17.5, 5.5, 15.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();
	//28
	box.setPosition(glm::vec3(-17.5, 5.5, 20.0));
	box.setScale(glm::vec3(1.0, 11, 0.3));
	box.render();

	//29
	box.setPosition(glm::vec3(-7, 5.5, -5));
	box.setScale(glm::vec3(1.0, 11, .5));
	box.render();
	//30
	box.setPosition(glm::vec3(-2.33, 5.5, -4.5));
	box.setScale(glm::vec3(0.3, 11, 1.0));
	box.render();
	//31
	box.setPosition(glm::vec3(2.33, 5.5, -4.5));
	box.setScale(glm::vec3(0.3, 11, 1.0));
	box.render();
	//32
	box.setPosition(glm::vec3(7, 5.5, -5));
	box.setScale(glm::vec3(1.0, 11, 1.0));
	box.render();

	//TECHOS
	box.setPosition(glm::vec3(0, 2.5, 1.5));
	box.setScale(glm::vec3(14, 1, 7));
	box.render();

	box.setPosition(glm::vec3(0, 5.5, 1.5));
	box.setScale(glm::vec3(14, 1, 7));
	box.render();

	box.setPosition(glm::vec3(0, 8.5, 1.5));
	box.setScale(glm::vec3(14, 1, 7));
	box.render();

	//Para hacer el cubo
	box.setPosition(glm::vec3(-4.65, 2.5, -3.5));
	box.setScale(glm::vec3(4.7, 1, 3));
	box.render();

	box.setPosition(glm::vec3(-4.65, 5.5, -3.5));
	box.setScale(glm::vec3(4.7, 1, 3));
	box.render();

	box.setPosition(glm::vec3(-4.65, 8.5, -3.5));
	box.setScale(glm::vec3(4.7, 1, 3));
	box.render();

	box.setPosition(glm::vec3(4.65, 2.5, -3.5));
	box.setScale(glm::vec3(4.7, 1, 3));
	box.render();

	box.setPosition(glm::vec3(4.65, 5.5, -3.5));
	box.setScale(glm::vec3(4.7, 1, 3));
	box.render();

	box.setPosition(glm::vec3(4.65, 8.5, -3.5));
	box.setScale(glm::vec3(4.7, 1, 3));
	box.render();
	//

	//Para hacer los intermedios
	box.setPosition(glm::vec3(0.0, 1.25, -4.25));
	box.setScale(glm::vec3(4.4, 0.5, 1.5));
	box.render();

	box.setPosition(glm::vec3(0.0, 4.0, -4.25));
	box.setScale(glm::vec3(4.4, 0.5, 1.5));
	box.render();

	box.setPosition(glm::vec3(0.0, 7.0, -4.25));
	box.setScale(glm::vec3(4.4, 0.5, 1.5));
	box.render();
	//

	//Cubo
	box.setPosition(glm::vec3(2.33, 5.5, -3.0));
	box.setScale(glm::vec3(0.3, 11, 2.0));
	box.render();

	box.setPosition(glm::vec3(0.0, 5.5, -2.75));
	box.setScale(glm::vec3(0.1, 11, 1.5));
	box.render();

	box.setPosition(glm::vec3(-2.33, 5.5, -3.0));
	box.setScale(glm::vec3(0.3, 11, 2.0));
	box.render();
	//

	//Paredes escalera
	box.setPosition(glm::vec3(0, 0.5, -4.8));
	box.setScale(glm::vec3(4.66, 1.0, 0.1));
	box.render();

	box.setPosition(glm::vec3(0, 8.125, -4.8));
	box.setScale(glm::vec3(4.66, 1.75, 0.1));
	box.render();

	box.setPosition(glm::vec3(0, 8.5, -4.9));
	box.setScale(glm::vec3(4.66, 1.0, 0.2));
	box.render();

	box.setPosition(glm::vec3(0, 10.5, -4.8));
	box.setScale(glm::vec3(4.66, 1.0, 0.1));
	box.render();
	//

	//Paredes traseras
	//PB
	box.setPosition(glm::vec3(4.66, 0.75, -4.8));
	box.setScale(glm::vec3(2.33, 1.5, 0.1));
	box.render();

	box.setPosition(glm::vec3(6.4075, 1.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();

	box.setPosition(glm::vec3(2.9125, 1.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();

	box.setPosition(glm::vec3(-4.66, 0.75, -4.8));
	box.setScale(glm::vec3(2.33, 1.5, 0.1));
	box.render();

	box.setPosition(glm::vec3(-6.4075, 1.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();

	box.setPosition(glm::vec3(-2.9125, 1.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();

	//P1
	box.setPosition(glm::vec3(4.66, 3.75, -4.8));
	box.setScale(glm::vec3(2.33, 1.5, 0.1));
	box.render();

	box.setPosition(glm::vec3(6.4075, 4.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();

	box.setPosition(glm::vec3(2.9125, 4.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();

	box.setPosition(glm::vec3(-4.66, 3.75, -4.8));
	box.setScale(glm::vec3(2.33, 1.5, 0.1));
	box.render();

	box.setPosition(glm::vec3(-6.4075, 4.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();

	box.setPosition(glm::vec3(-2.9125, 4.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();

	//P2
	box.setPosition(glm::vec3(4.66, 6.75, -4.8));
	box.setScale(glm::vec3(2.33, 1.5, 0.1));
	box.render();

	box.setPosition(glm::vec3(6.4075, 7.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();

	box.setPosition(glm::vec3(2.9125, 7.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();

	box.setPosition(glm::vec3(-4.66, 6.75, -4.8));
	box.setScale(glm::vec3(2.33, 1.5, 0.1));
	box.render();

	box.setPosition(glm::vec3(-6.4075, 7.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();

	box.setPosition(glm::vec3(-2.9125, 7.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();

	//P3
	box.setPosition(glm::vec3(4.66, 9.75, -4.8));
	box.setScale(glm::vec3(2.33, 1.5, 0.1));
	box.render();

	box.setPosition(glm::vec3(6.4075, 10.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();

	box.setPosition(glm::vec3(2.9125, 10.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();

	box.setPosition(glm::vec3(-4.66, 9.75, -4.8));
	box.setScale(glm::vec3(2.33, 1.5, 0.1));
	box.render();

	box.setPosition(glm::vec3(-6.4075, 10.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();

	box.setPosition(glm::vec3(-2.9125, 10.0, -4.8));
	box.setScale(glm::vec3(1.165, 2.0, 0.1));
	box.render();
	//

	//Escaleras PB
	box.setPosition(glm::vec3(1.1, 0.15, -2.1875));
	box.setScale(glm::vec3(2.2, 0.3, 0.375));
	box.render();

	box.setPosition(glm::vec3(1.1, 0.45, -2.5625));
	box.setScale(glm::vec3(2.2, 0.3, 0.375));
	box.render();

	box.setPosition(glm::vec3(1.1, 0.75, -2.9375));
	box.setScale(glm::vec3(2.2, 0.3, 0.375));
	box.render();

	box.setPosition(glm::vec3(1.1, 1.05, -3.3125));
	box.setScale(glm::vec3(2.2, 0.3, 0.375));
	box.render();
	//Regreso PB
	box.setPosition(glm::vec3(-1.1, 1.65, -3.3125));
	box.setScale(glm::vec3(2.2, 0.3, 0.375));
	box.render();

	box.setPosition(glm::vec3(-1.1, 1.95, -2.9375));
	box.setScale(glm::vec3(2.2, 0.3, 0.375));
	box.render();

	box.setPosition(glm::vec3(-1.1, 2.25, -2.5625));
	box.setScale(glm::vec3(2.2, 0.3, 0.375));
	box.render();

	box.setPosition(glm::vec3(-1.1, 2.55, -2.1875));
	box.setScale(glm::vec3(2.2, 0.3, 0.375));
	box.render();
	//

	//Escaleras P1
	box.setPosition(glm::vec3(1.1, 3.125, -2.1875));
	box.setScale(glm::vec3(2.2, 0.25, 0.375));
	box.render();

	box.setPosition(glm::vec3(1.1, 3.375, -2.5625));
	box.setScale(glm::vec3(2.2, 0.25, 0.375));
	box.render();

	box.setPosition(glm::vec3(1.1, 3.625, -2.9375));
	box.setScale(glm::vec3(2.2, 0.25, 0.375));
	box.render();

	box.setPosition(glm::vec3(1.1, 3.875, -3.3125));
	box.setScale(glm::vec3(2.2, 0.25, 0.375));
	box.render();
	//Regreso P1
	box.setPosition(glm::vec3(-1.1, 4.4, -3.35));
	box.setScale(glm::vec3(2.2, 0.3, 0.3));
	box.render();

	box.setPosition(glm::vec3(-1.1, 4.7, -3.05));
	box.setScale(glm::vec3(2.2, 0.3, 0.3));
	box.render();

	box.setPosition(glm::vec3(-1.1, 5.0, -2.75));
	box.setScale(glm::vec3(2.2, 0.3, 0.3));
	box.render();

	box.setPosition(glm::vec3(-1.1, 5.3, -2.45));
	box.setScale(glm::vec3(2.2, 0.3, 0.3));
	box.render();

	box.setPosition(glm::vec3(-1.1, 5.6, -2.15));
	box.setScale(glm::vec3(2.2, 0.3, 0.3));
	box.render();
	//

	//Escaleras P2
	box.setPosition(glm::vec3(1.1, 6.125, -2.1875));
	box.setScale(glm::vec3(2.2, 0.25, 0.375));
	box.render();

	box.setPosition(glm::vec3(1.1, 6.375, -2.5625));
	box.setScale(glm::vec3(2.2, 0.25, 0.375));
	box.render();

	box.setPosition(glm::vec3(1.1, 6.625, -2.9375));
	box.setScale(glm::vec3(2.2, 0.25, 0.375));
	box.render();

	box.setPosition(glm::vec3(1.1, 6.875, -3.3125));
	box.setScale(glm::vec3(2.2, 0.25, 0.375));
	box.render();
	//Regreso P2
	box.setPosition(glm::vec3(-1.1, 7.4, -3.35));
	box.setScale(glm::vec3(2.2, 0.3, 0.3));
	box.render();

	box.setPosition(glm::vec3(-1.1, 7.7, -3.05));
	box.setScale(glm::vec3(2.2, 0.3, 0.3));
	box.render();

	box.setPosition(glm::vec3(-1.1, 8.0, -2.75));
	box.setScale(glm::vec3(2.2, 0.3, 0.3));
	box.render();

	box.setPosition(glm::vec3(-1.1, 8.3, -2.45));
	box.setScale(glm::vec3(2.2, 0.3, 0.3));
	box.render();

	box.setPosition(glm::vec3(-1.1, 8.6, -2.15));
	box.setScale(glm::vec3(2.2, 0.3, 0.3));
	box.render();
	//

	box.setPosition(glm::vec3(0, 12, 0));
	box.setScale(glm::vec3(14, 2, 10));
	box.render();

	box.setPosition(glm::vec3(12.5, 2.5, -8));
	box.setScale(glm::vec3(11, 1, 46));
	box.render();

	box.setPosition(glm::vec3(12.5, 5.5, -8));
	box.setScale(glm::vec3(11, 1, 46));
	box.render();

	box.setPosition(glm::vec3(12.5, 8.5, -8));
	box.setScale(glm::vec3(11, 1, 46));
	box.render();

	box.setPosition(glm::vec3(12.5, 12, -8));
	box.setScale(glm::vec3(11, 2, 46));
	box.render();

	//::::::::::
	box.setPosition(glm::vec3(-12.5, 2.5, 7.5));
	box.setScale(glm::vec3(11, 1, 35));
	box.render();

	box.setPosition(glm::vec3(-12.5, 5.5, 7.5));
	box.setScale(glm::vec3(11, 1, 35));
	box.render();

	box.setPosition(glm::vec3(-12.5, 8.5, 7.5));
	box.setScale(glm::vec3(11, 1, 35));
	box.render();

	box.setPosition(glm::vec3(-12.5, 12, 7.5));
	box.setScale(glm::vec3(11, 2, 35));
	box.render();
	//::::::::::



	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	//:::::::::::::::::::::::::::::::::WINDOWS::::::::::::::::::::::::::::::::::
	// puerta principal
	glBindTexture(GL_TEXTURE_2D, puerta_principal);
	windowsBox.setShader(&shaderLighting);
	windowsBox.setProjectionMatrix(projection);
	windowsBox.setViewMatrix(view);

	windowsBox.setPosition(glm::vec3(0, 1, 4.5));
	windowsBox.setScale(glm::vec3(4.66, 2, .1));
	windowsBox.render();
	// ventanas 1er nivel
	glBindTexture(GL_TEXTURE_2D, ventana2);
	windowsBox.setShader(&shaderLighting);
	windowsBox.setProjectionMatrix(projection);
	windowsBox.setViewMatrix(view);

	windowsBox.setPosition(glm::vec3(4.66, 1, 4.5));
	windowsBox.setScale(glm::vec3(4.66, 2, .1));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-4.66, 1, 4.5));
	windowsBox.setScale(glm::vec3(4.66, 2, .1));
	windowsBox.render();
	// ventanas otros niveles
	glBindTexture(GL_TEXTURE_2D, ventana1);
	windowsBox.setShader(&shaderLighting);
	windowsBox.setProjectionMatrix(projection);
	windowsBox.setViewMatrix(view);

	windowsBox.setPosition(glm::vec3(0, 4, 4.5));
	windowsBox.setScale(glm::vec3(4.66, 2, .1));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(4.66, 4, 4.5));
	windowsBox.setScale(glm::vec3(4.66, 2, .1));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-4.66, 4, 4.5));
	windowsBox.setScale(glm::vec3(4.66, 2, .1));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(0, 7, 4.5));
	windowsBox.setScale(glm::vec3(4.66, 2, .1));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(4.66, 7, 4.5));
	windowsBox.setScale(glm::vec3(4.66, 2, .1));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-4.66, 7, 4.5));
	windowsBox.setScale(glm::vec3(4.66, 2, .1));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(0, 10, 4.5));
	windowsBox.setScale(glm::vec3(4.66, 2, .1));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(4.66, 10, 4.5));
	windowsBox.setScale(glm::vec3(4.66, 2, .1));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-4.66, 10, 4.5));
	windowsBox.setScale(glm::vec3(4.66, 2, .1));
	windowsBox.render();

	//Ventanas escaleras
	windowsBox.setPosition(glm::vec3(0, 2.625, -4.8));
	windowsBox.setScale(glm::vec3(4.66, 2.25, 0.1));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(0, 5.5, -4.8));
	windowsBox.setScale(glm::vec3(4.66, 2.5, 0.1));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(0, 9.5, -4.8));
	windowsBox.setScale(glm::vec3(4.66, 1.0, 0.1));
	windowsBox.render();
	//

	// ventanas izq
	glBindTexture(GL_TEXTURE_2D, ventana3);
	windowsBox.setShader(&shaderLighting);
	windowsBox.setProjectionMatrix(projection);
	windowsBox.setViewMatrix(view);

	//PB
	windowsBox.setPosition(glm::vec3(-7.5, 1.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 1.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 1.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 1.0, 17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 1.0, 22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	//P1
	windowsBox.setPosition(glm::vec3(-7.5, 4.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 4.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 4.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 4.0, 17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 4.0, 22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	//P2
	windowsBox.setPosition(glm::vec3(-7.5, 7.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 7.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 7.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 7.0, 17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 7.0, 22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	//P3
	windowsBox.setPosition(glm::vec3(-7.5, 10.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 10.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 10.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 10.0, 17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-7.5, 10.0, 22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	// ventanas derecha
	//PB
	windowsBox.setPosition(glm::vec3(7.5, 1.0, -28.0));
	windowsBox.setScale(glm::vec3(0.1, 2, 5.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 1.0, -22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 1.0, -17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 1.0, -12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 1.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 1.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 1.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	//P1
	windowsBox.setPosition(glm::vec3(7.5, 4.0, -28.0));
	windowsBox.setScale(glm::vec3(0.1, 2, 5.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 4.0, -22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 4.0, -17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 4.0, -12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 4.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 4.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 4.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	//P2
	windowsBox.setPosition(glm::vec3(7.5, 7.0, -28.0));
	windowsBox.setScale(glm::vec3(0.1, 2, 5.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 7.0, -22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 7.0, -17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 7.0, -12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 7.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 7.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 7.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	//P3
	windowsBox.setPosition(glm::vec3(7.5, 10.0, -28.0));
	windowsBox.setScale(glm::vec3(0.1, 2, 5.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 10.0, -22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 10.0, -17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 10.0, -12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 10.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 10.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(7.5, 10.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	// ventanas derecha externas
	//PB
	windowsBox.setPosition(glm::vec3(17.5, 1.0, -28.0));
	windowsBox.setScale(glm::vec3(0.1, 2, 5.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 1.0, -22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 1.0, -17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 1.0, -12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 1.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 1.0, -2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 1.0, 2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 1.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 1.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	//P1
	windowsBox.setPosition(glm::vec3(17.5, 4.0, -28.0));
	windowsBox.setScale(glm::vec3(0.1, 2, 5.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 4.0, -22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 4.0, -17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 4.0, -12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 4.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 4.0, -2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 4.0, 2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 4.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 4.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	//P2
	windowsBox.setPosition(glm::vec3(17.5, 7.0, -28.0));
	windowsBox.setScale(glm::vec3(0.1, 2, 5.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 7.0, -22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 7.0, -17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 7.0, -12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 7.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 7.0, -2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 7.0, 2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 7.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 7.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	//P3
	windowsBox.setPosition(glm::vec3(17.5, 10.0, -28.0));
	windowsBox.setScale(glm::vec3(0.1, 2, 5.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 10.0, -22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 10.0, -17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 10.0, -12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 10.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 10.0, -2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 10.0, 2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 10.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(17.5, 10.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	// ventanas izquierda externas
	//PB
	windowsBox.setPosition(glm::vec3(-17.5, 1.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 1.0, -2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 1.0, 2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 1.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 1.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 1.0, 17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 1.0, 22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	//P1
	windowsBox.setPosition(glm::vec3(-17.5, 4.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 4.0, -2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 4.0, 2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 4.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 4.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 4.0, 17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 4.0, 22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	//P2
	windowsBox.setPosition(glm::vec3(-17.5, 7.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 7.0, -2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 7.0, 2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 7.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 7.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 7.0, 17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 7.0, 22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	//P3
	windowsBox.setPosition(glm::vec3(-17.5, 10.0, -7.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 10.0, -2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 10.0, 2.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 10.0, 7.53));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.66));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 10.0, 12.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 10.0, 17.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	windowsBox.setPosition(glm::vec3(-17.5, 10.0, 22.50));
	windowsBox.setScale(glm::vec3(0.1, 2, 4.7));
	windowsBox.render();

	//Ventanas baños
	glBindTexture(GL_TEXTURE_2D, ventana4);
	windowsBanio.setShader(&shaderLighting);
	windowsBanio.setProjectionMatrix(projection);
	windowsBanio.setViewMatrix(view);
	windowsBanio.setOrientation(glm::vec3(45.0, 0.0, 0.0));

	//PB
	windowsBanio.setPosition(glm::vec3(4.66, 1.75, -4.8));
	windowsBanio.setScale(glm::vec3(2.33, 0.5, 0.1));
	windowsBanio.render();

	windowsBanio.setPosition(glm::vec3(-4.66, 1.75, -4.8));
	windowsBanio.setScale(glm::vec3(2.33, 0.5, 0.1));
	windowsBanio.render();

	//P1
	windowsBanio.setPosition(glm::vec3(4.66, 4.75, -4.8));
	windowsBanio.setScale(glm::vec3(2.33, 0.5, 0.1));
	windowsBanio.render();

	windowsBanio.setPosition(glm::vec3(-4.66, 4.75, -4.8));
	windowsBanio.setScale(glm::vec3(2.33, 0.5, 0.1));
	windowsBanio.render();

	//P2
	windowsBanio.setPosition(glm::vec3(4.66, 7.75, -4.8));
	windowsBanio.setScale(glm::vec3(2.33, 0.5, 0.1));
	windowsBanio.render();

	windowsBanio.setPosition(glm::vec3(-4.66, 7.75, -4.8));
	windowsBanio.setScale(glm::vec3(2.33, 0.5, 0.1));
	windowsBanio.render();

	//P3
	windowsBanio.setPosition(glm::vec3(4.66, 10.75, -4.8));
	windowsBanio.setScale(glm::vec3(2.33, 0.5, 0.1));
	windowsBanio.render();

	windowsBanio.setPosition(glm::vec3(-4.66, 10.75, -4.8));
	windowsBanio.setScale(glm::vec3(2.33, 0.5, 0.1));
	windowsBanio.render();
	//
	//Fin ventanas

	//:::::::::::::::::::::::::::::::::PIEDRA::::::::::::::::::::::::::::::::::
	//jardinera
	glBindTexture(GL_TEXTURE_2D, piedra);
	boxStone.setShader(&shaderLighting);
	boxStone.setProjectionMatrix(projection);
	boxStone.setViewMatrix(view);
	boxStone.setOrientation(glm::vec3(0.0, 0.0, -90.0));

	boxStone.setPosition(glm::vec3(1.0, 0.5, 17.0));
	boxStone.setScale(glm::vec3(1.0, 1.0f, 5.0f));
	boxStone.render();

	boxStone.setPosition(glm::vec3(-1.0, 0.5, 17.0));
	boxStone.setScale(glm::vec3(1.0, 1.0f, 5.0f));
	boxStone.render();

	boxStone.setPosition(glm::vec3(0.0, 0.5, 20.0));
	boxStone.setScale(glm::vec3(1.0, 3.0f, 1.0f));
	boxStone.render();

	boxStone.setPosition(glm::vec3(0.0, 0.5, 14.0));
	boxStone.setScale(glm::vec3(1.0, 3.0f, 1.0f));
	boxStone.render();

}

void renderizarExterior(glm::mat4 view, glm::mat4 projection) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tronco);
	
	tree.setShader(&shaderLighting);
	tree.setProjectionMatrix(projection);
	tree.setViewMatrix(view);
	tree.setScale(glm::vec3(0.6f, 0.6f, 0.6f));
	tree.setOrientation(glm::vec3(-90, 0, 0));

	tree.setPosition(glm::vec3(0.0, 0.0, 17.0));
	tree.render();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hojas);

	leaves.setShader(&shaderLighting);
	leaves.setProjectionMatrix(projection);
	leaves.setViewMatrix(view);
	leaves.setScale(glm::vec3(0.6f, 0.6f, 0.6f));
	leaves.setOrientation(glm::vec3(-90, 0, 0));

	leaves.setPosition(glm::vec3(0.0, 0.0, 17.0));
	leaves.render();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bronce);
	busto.setShader(&shaderLighting);
	busto.setProjectionMatrix(projection);
	busto.setViewMatrix(view);
	busto.setScale(glm::vec3(0.025f, 0.03f, 0.03f));
	busto.setOrientation(glm::vec3(-90, -180, 0));

	busto.setPosition(glm::vec3(-0.25, 1.15, 12.6));
	busto.render();

	glActiveTexture(GL_TEXTURE0);
	pedestal.setShader(&shaderLighting);
	pedestal.setProjectionMatrix(projection);
	pedestal.setViewMatrix(view);
	pedestal.setScale(glm::vec3(0.014f, 0.010f, 0.014f));
	pedestal.setOrientation(glm::vec3(-90, 0, 0));

	pedestal.setPosition(glm::vec3(0.0, 0.0, 13.0));
	pedestal.render();
}

void animacionPuerta(glm::mat4 view, glm::mat4 projection) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ventanaLab);
	windowsBox.setShader(&shaderLighting);
	windowsBox.setProjectionMatrix(projection);
	windowsBox.setViewMatrix(view);
	windowsBox.setPosition(glm::vec3(10, 10, zDoorSlide));
	windowsBox.setScale(glm::vec3(.1, 2, 1.33));
	windowsBox.render();

	if (actionAnim) {
		actionAnim = false;
		animFinished = false;
	}
	if (!animFinished) {
		if (cerrada) {
			if (zDoorSlide <= -19) {
				zDoorSlide = -19;
				animFinished = true;
				cerrada = false;
			}
			else {
				zDoorSlide -= 0.05;
			}
		}
		else {
			if (zDoorSlide >= -17.66) {
				zDoorSlide = -17.66;
				animFinished = true;
				cerrada = true;
			}
			else {
				zDoorSlide += 0.05;
			}
		}
	}
}

void animacionHoja(glm::mat4 view, glm::mat4 projection) {

	glBindTexture(GL_TEXTURE_2D, hojas);
	leaveBox.setShader(&shaderLighting);
	leaveBox.setProjectionMatrix(projection);
	leaveBox.setViewMatrix(view);
	leaveBox.setScale(glm::vec3(0.2, 0.001f, 0.2f));


	leaveBox.setOrientation(glm::vec3(0.0, 0.0, -angulo));
	leaveBox.setPosition(glm::vec3(hojaX, 0.5 + hojaY, 14.0));
	leaveBox.render();

	leaveBox.setOrientation(glm::vec3(0.0, 0.0, angulo));
	leaveBox.setPosition(glm::vec3(hojaX, hojaY, 14.0));
	leaveBox.render();

	leaveBox.setOrientation(glm::vec3(0.0, 0.0, -angulo));
	leaveBox.setPosition(glm::vec3(-2.0 + hojaX, hojaY, 14.0));
	leaveBox.render();

	leaveBox.setOrientation(glm::vec3(0.0, 0.0, angulo));
	leaveBox.setPosition(glm::vec3(2.0 + hojaX, hojaY, 14.0));
	leaveBox.render();

	leaveBox.setOrientation(glm::vec3(0.0, 0.0, -angulo));
	leaveBox.setPosition(glm::vec3(hojaX, 0.5 + hojaY, 16.0));
	leaveBox.render();

	leaveBox.setOrientation(glm::vec3(0.0, 0.0, angulo));
	leaveBox.setPosition(glm::vec3(hojaX, hojaY, 15.0));
	leaveBox.render();

	leaveBox.setOrientation(glm::vec3(0.0, 0.0, -angulo));
	leaveBox.setPosition(glm::vec3(-3.0 + hojaX, hojaY, 16.0));
	leaveBox.render();

	leaveBox.setOrientation(glm::vec3(0.0, 0.0, angulo));
	leaveBox.setPosition(glm::vec3(3.0 + hojaX, hojaY, 15.0));
	leaveBox.render();

	hojaX = 1.0*glm::sin(glm::radians(angulo));
	hojaY = hojaYaux * glm::cos(glm::radians(angulo));
	hojaYaux -= 0.01f;

	if (hojaYaux <= 0.0f) {
		hojaYaux = 10.0f;
	}

	if (vuelta1)
	{
		angulo -= 0.2f;
		if (angulo < -30.0f)
		{
			vuelta1 = false;
			vuelta2 = true;
		}

	}
	if (vuelta2)
	{
		angulo += 0.2f;
		if (angulo > 30.0f)
		{
			vuelta2 = false;
			vuelta1 = true;
		}

	}
}

void animacionAvion(glm::mat4 view, glm::mat4 projection) {
	glBindTexture(GL_TEXTURE_2D, papel);
	avion.setShader(&shaderLighting);
	avion.setColor(glm::vec3(0.4f, 0.4f, 0.4f));
	avion.setProjectionMatrix(projection);
	avion.setViewMatrix(view);
	avion.setScale(glm::vec3(0.002f, 0.002f, 0.002f));

	avion.setOrientation(glm::vec3(0.0, -90 + ang, 0.0));
	avion.setPosition(glm::vec3(13 + avionX, 10.5, -21 + avionZ));
	avion.render();

	avionX = 2.0 * glm::sin(glm::radians(ang));
	avionZ = 2.0 * glm::cos(glm::radians(ang));

	ang += 0.2f;
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

	//OBTENCION DE LOS KEYFRAMES DEL ARCHIVO
	std::vector<std::vector<glm::mat4>> keyFramesBrazo = getKeyFrames("../../animaciones/animationMano.txt");
	//NUMERO DE PASOS ENTRE KEYFRAMES (I a I+1)
	int numPasosAnimBrazo = 20;
	int numPasosAnimBrazoCurr = 0;

	// Indices del arreglo keyFramesBrazo el actual y el siguiente
	int indexKeyFrameBrazoCurr = 0;
	int indexKeyFrameBrazoNext = 1;
	float interpolation = 0.0;

	//OBTENCION DE LOS KEYFRAMES DEL ARCHIVO2---------
	keyFramesProg = getKeyFrames("../../animaciones/keyframeAnim.txt");
	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


	while (psi) {
		psi = processInput(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Matrix de proyeccion en perspectiva
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / screenWidth, 0.01f, 100.0f);
		// matrix de vista
		glm::mat4 view = camera->getViewMatrix();

		cylinderAnimacion.setProjectionMatrix(projection);
		cylinderAnimacion.setViewMatrix(view);
		sphereAnimacion.setProjectionMatrix(projection);
		sphereAnimacion.setViewMatrix(view);

		shaderTexture.turnOn();
		// Importante si se quiere renderizar modelos y texturas
		glActiveTexture(GL_TEXTURE0);
		
		// Iluminaci�n
		glm::mat4 lightModelmatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
		lightModelmatrix = glm::translate(lightModelmatrix, glm::vec3(0.0f, 0.0f, -ratio));

		shaderMateriales.turnOn();
		glUniform3fv(shaderMateriales.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3(lightModelmatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));
		glUniform3fv(shaderMateriales.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
		glUniform3f(shaderMateriales.getUniformLocation("light.ambient"), 0.2, 0.2, 0.2);
		glUniform3f(shaderMateriales.getUniformLocation("light.diffuse"), 0.3, 0.3, 0.3);
		glUniform3f(shaderMateriales.getUniformLocation("light.specular"), 0.5, 0.3, 0.2);
		glUniform3f(shaderMateriales.getUniformLocation("material.ambient"), 1.0, 0.2, 0.6);
		glUniform3f(shaderMateriales.getUniformLocation("material.diffuse"), 0.4, 0.5, 0.8);
		glUniform3f(shaderMateriales.getUniformLocation("material.specular"), 0.5, 0.3, 0.2);
		glUniform1f(shaderMateriales.getUniformLocation("material.shininess"), 32.0);
		/*cylinder.render();*/
		shaderMateriales.turnOff();

		shaderLighting.turnOn();
		glUniform3fv(shaderLighting.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
		//Directional light
		glUniform3f(shaderLighting.getUniformLocation("directionalLight.light.ambient"), 0.25, 0.25, 0.25);
		glUniform3f(shaderLighting.getUniformLocation("directionalLight.light.diffuse"), 0.1, 0.1, 0.1);
		glUniform3f(shaderLighting.getUniformLocation("directionalLight.light.specular"), 0.15, 0.15, 0.15);
		glUniform3fv(shaderLighting.getUniformLocation("directionalLight.direction"), 1, glm::value_ptr(glm::vec3(0, -1.0, 0.0)));
		//Numero de luces spot y point
		int locCount = shaderLighting.getUniformLocation("pointLightCount");
		glUniform1i(shaderLighting.getUniformLocation("pointLightCount"), 1);
		glUniform1i(shaderLighting.getUniformLocation("spotLightCount"), 1);
		// Point light
		glUniform3fv(shaderLighting.getUniformLocation("pointLights[0].position"), 1, glm::value_ptr(glm::vec3(lightModelmatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));
		glUniform1f(shaderLighting.getUniformLocation("pointLights[0].constant"), 1.0f);
		glUniform1f(shaderLighting.getUniformLocation("pointLights[0].linear"), 0.14f);
		glUniform1f(shaderLighting.getUniformLocation("pointLights[0].quadratics"), 0.07f);
		glUniform3f(shaderLighting.getUniformLocation("pointLights[0].light.ambient"), 0.025, 0.025, 0.025);
		glUniform3f(shaderLighting.getUniformLocation("pointLights[0].light.diffuse"), 0.2, 0.2, 0.2);
		glUniform3f(shaderLighting.getUniformLocation("pointLights[0].light.specular"), 0.2, 0.2, 0.2);
		// Spot light
		glUniform3fv(shaderLighting.getUniformLocation("spotLights[0].position"), 1, glm::value_ptr(camera->getPosition()));
		glUniform3fv(shaderLighting.getUniformLocation("spotLights[0].direction"), 1, glm::value_ptr(camera->getFront()));
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].outerCutOff"), glm::cos(glm::radians(15.0f)));
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].constant"), 1.0f);
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].linear"), 0.14f);
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].quadratics"), 0.07f);
		glUniform3f(shaderLighting.getUniformLocation("spotLights[0].light.ambient"), 0.025, 0.025, 0.025);
		glUniform3f(shaderLighting.getUniformLocation("spotLights[0].light.diffuse"), 0.7, 0.7, 0.7);
		glUniform3f(shaderLighting.getUniformLocation("spotLights[0].light.specular"), 0.7, 0.7, 0.7);
		shaderLighting.turnOff();

		/*modelRock.setShader(&shaderLighting);
		modelRock.setProjectionMatrix(projection);
		modelRock.setViewMatrix(view);
		modelRock.setPosition(glm::vec3(5.0, 3.0, -20.0));
		modelRock.setScale(glm::vec3(1.0, 1.0, 1.0));
		modelRock.render();*/

		/*modelAirCraft.setShader(&shaderLighting);
		modelAirCraft.setProjectionMatrix(projection);
		modelAirCraft.setViewMatrix(view);
		modelAirCraft.setScale(glm::vec3(1.0, 1.0, 1.0));
		glm::mat4 matrixAirCraft = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, aircraftZ));
		matrixAirCraft = glm::translate(matrixAirCraft, glm::vec3(10.0, 2.0, 15.0));
		matrixAirCraft = glm::rotate(matrixAirCraft, rotationAirCraft, glm::vec3(0, 1, 0));
		modelAirCraft.render(matrixAirCraft);*/

		glm::quat firstQuat;
		glm::quat secondQuat;
		glm::quat finalQuat;
		glm::mat4 interpoltaedMatrix;
		glm::vec4 transformComp1;
		glm::vec4 transformComp2;
		glm::vec4 finalTrans;
		animacionProgramador(view, projection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pared_q);//          v----Numero de articulaciones + 1
		if (keyFramesBrazo[indexKeyFrameBrazoCurr].size() == 7 && keyFramesBrazo[indexKeyFrameBrazoNext].size() == 7) {

			//Matriz de rotaci�n actual
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][0]); // el 0 es la matriz de mayor jerarquia (Hombro)
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][0]);
			//Slerp hace la interpolacion del quaternion
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			//Se convierte el quaternion a una matriz de 4x4
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			//Se obtiene la traslaci�n en la matriz del frame i
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][0] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			//Se obtiene la traslaci�n en la matriz del frame i + 1
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][0] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

			//Se obtiene la interpolacion entre el frame i con el  frame i + 1
			//transformComp1 es el frame i
			//transformComp2 es el frame i + 1
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;

			//Unimos la matriz de interpolaci�n con la de traslacion
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			// Animacion KeyFrames
			glm::mat4 matrixGlobalAnimation = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 2.0f, 0.0f));
			// Se modela siempre con los ejes de giro en el eje z
			// Articulacion 1
			glm::mat4 keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			//copia del 20-capAnim
			/*	*/glm::mat4 cylinderMatrixJ0 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			/*	*/cylinderMatrixJ0 = glm::scale(cylinderMatrixJ0, glm::vec3(0.08f, 0.08f, 0.08f));
			/*	*/sphereAnimacion.render(cylinderMatrixJ0);

			// Articulacion 2
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][1]);// se va por la siguiente matriz de transf
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][1]);
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][1] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][1] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			//copia del 20-capAnim
			/*	*/glm::mat4 cylinderMatrixJ1 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			/*	*/cylinderMatrixJ1 = glm::scale(cylinderMatrixJ1, glm::vec3(0.1f, 0.1f, 0.1f));
			/*	*/sphereAnimacion.render(cylinderMatrixJ1);
			
			// Articulacion 3
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][2]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][2]);
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][2] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][2] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ2 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ2 = glm::scale(cylinderMatrixJ2, glm::vec3(0.11f, 0.11f, 0.11f));
			sphereAnimacion.render(cylinderMatrixJ2);

			// Hueso 1
			glm::mat4 cylinderMatrixL1 = glm::translate(keyFrameJoint, glm::vec3(0.0f, 0.0f, 0.125f));
			cylinderMatrixL1 = glm::rotate(cylinderMatrixL1, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixL1 = glm::scale(cylinderMatrixL1, glm::vec3(0.1f, 0.25f, 0.1f));
			cylinderAnimacion.render(cylinderMatrixL1);

			// Articulacion 4
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][3]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][3]);
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][3] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][3] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ3 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ3 = glm::scale(cylinderMatrixJ3, glm::vec3(0.11f, 0.11f, 0.11f));
			sphereAnimacion.render(cylinderMatrixJ3);

			// Articulacion 5
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][4]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][4]);
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][4] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][4] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ4 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ4 = glm::scale(cylinderMatrixJ4, glm::vec3(0.11f, 0.11f, 0.11f));
			sphereAnimacion.render(cylinderMatrixJ4);

			// Hueso 2
			glm::mat4 cylinderMatrixL2 = glm::translate(keyFrameJoint, glm::vec3(0.0f, 0.0f, 0.125f));
			cylinderMatrixL2 = glm::rotate(cylinderMatrixL2, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixL2 = glm::scale(cylinderMatrixL2, glm::vec3(0.1f, 0.25f, 0.1f));
			cylinderAnimacion.render(cylinderMatrixL2);

			// Articulacion 6
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][5]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][5]);
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][5] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][5] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ5 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ5 = glm::scale(cylinderMatrixJ5, glm::vec3(0.11f, 0.11f, 0.11f));
			sphereAnimacion.render(cylinderMatrixJ5);

		}

		numPasosAnimBrazoCurr++;
		interpolation = numPasosAnimBrazoCurr / (float)numPasosAnimBrazo;

		if (interpolation >= 1.0) {
			interpolation = 0;
			numPasosAnimBrazoCurr = 0;
			indexKeyFrameBrazoCurr = indexKeyFrameBrazoNext;
			indexKeyFrameBrazoNext++;
		}

		if (indexKeyFrameBrazoNext > keyFramesBrazo.size() - 1) {
			interpolation = 0;
			indexKeyFrameBrazoCurr = 0;
			indexKeyFrameBrazoNext = 1;
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureCespedID);
		box.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setPosition(glm::vec3(0.0, 0.0, 0.0));
		box.setScale(glm::vec3(100.0, 0.001, 100.0));
		box.render();

		renderizarEdificio(view, projection);
		renderizarSalon(view, projection);
		renderizarExterior(view, projection);
		animacionPuerta(view, projection);
		animacionHoja(view, projection);
		animacionAvion(view, projection);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureWaterID);
		boxWater.setShader(&shaderLighting);
		boxWater.setProjectionMatrix(projection);
		boxWater.setViewMatrix(view);
		boxWater.setPosition(glm::vec3(3.0, 2.0, -5.0));
		boxWater.setScale(glm::vec3(10.0, 0.001, 10.0));
		boxWater.offsetUVS(glm::vec2(0.0001, 0.0001));
		boxWater.render();

		if (angle > 2 * M_PI)
			angle = 0.0;
		else
			angle += 0.001;

		sphere.setShader(&shaderColor);
		sphere.setColor(glm::vec3(0.4f, 0.3f, 0.6f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
		sphere.enableWireMode();
		sphere.render(lightModelmatrix);

		// Se Dibuja el Skybox
		shaderCubeTexture.turnOn();
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextureID);
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		GLuint cubeTextureId = shaderCubeTexture.getUniformLocation("skybox");
		glUniform1f(cubeTextureId, 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		sphere.setShader(&shaderCubeTexture);
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(glm::mat4(glm::mat3(view)));
		sphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));
		sphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
		shaderCubeTexture.turnOff();

		if (finishRotation) {
			if (direcionAirCraft)
				aircraftZ -= 0.01;
			else
				aircraftZ += 0.01;
			if (direcionAirCraft && aircraftZ < -6.0) {
				direcionAirCraft = false;
				finishRotation = false;
				aircraftZ = -6.0;
			}if (!direcionAirCraft && aircraftZ > 6.0) {
				direcionAirCraft = true;
				finishRotation = false;
				aircraftZ = 6.0;
			}
		}
		else {
			rotationAirCraft += 0.01;
			if (!direcionAirCraft) {
				if (rotationAirCraft > glm::radians(180.0f)) {
					finishRotation = true;
					rotationAirCraft = glm::radians(180.0f);
				}
			}
			else {
				if (rotationAirCraft > glm::radians(360.0f)) {
					finishRotation = true;
					rotationAirCraft = glm::radians(0.0f);
				}
			}
		}

		glfwSwapBuffers(window);
	}
}


void animacionProgramador(glm::mat4 view, glm::mat4 projection) {
	glm::quat firstQuat;
	glm::quat secondQuat;
	glm::quat finalQuat;
	glm::mat4 interpoltaedMatrix;
	glm::vec4 transformComp1;
	glm::vec4 transformComp2;
	glm::vec4 finalTrans;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pared_q);//          v----Numero de articulaciones + 1
	if (keyFramesProg[indexKeyFrameProgCurr].size() == 3 && keyFramesProg[indexKeyFrameProgNext].size() == 3) {
		//::::::::::::::::::::::::::::::::: ANIMACION POR KEYFRAMES
			//----
		glm::mat4 matrix0 = glm::mat4(1.0f);
		matrix0 = glm::translate(matrix0, glm::vec3(10.7f, 9.75f, -22.85f));
		matrix0 = glm::rotate(matrix0, 3.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		matrix0 = glm::scale(matrix0, glm::vec3(.5f, .5f, .5f));


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
		cylinder.render(matrix7);
		//---
		matrixs12 = glm::scale(matrixs12, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.render(matrixs12);

		matrix2 = glm::scale(matrix2, glm::vec3(0.1, 0.6, 0.2));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.render(matrix2);
		matrixs4 = glm::scale(matrixs4, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.render(matrixs4);
		matrix1 = glm::scale(matrix1, glm::vec3(0.15f, 0.8f, 0.15f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.render(matrix1);
		matrixs2 = glm::scale(matrixs2, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
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
		cylinder.render(matrix10);
		matrixs14 = glm::scale(matrixs14, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.render(matrixs14);
		matrix9 = glm::scale(matrix9, glm::vec3(0.1, 0.6, 0.2));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.render(matrix9);
		matrixs13 = glm::scale(matrixs13, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.render(matrixs13);
		matrix8 = glm::scale(matrix8, glm::vec3(0.15f, 0.8f, 0.15f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.render(matrix8);
		matrixs3 = glm::scale(matrixs3, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.render(matrixs3);
		matrixs1 = glm::scale(matrixs1, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.render(matrixs1);

		glm::mat4 matrixs5 = glm::translate(matrix0, glm::vec3(0.0f, 0.5f, 0.0f));
		//CABEZA
		glm::mat4 matrix11 = glm::mat4(1.0f);
		matrix11 = glm::translate(matrixs5, glm::vec3(0.0f, 0.5f, 0.0f));
		matrix11 = glm::scale(matrix11, glm::vec3(0.4f, 0.6f, 0.4f));
		cylinder2.setProjectionMatrix(projection);
		cylinder2.setViewMatrix(view);
		cylinder2.render(matrix11);

		//		BRAZO DERECHO
		//primera transf
		//Matriz de rotaci�n actual
		firstQuat = glm::quat_cast(keyFramesProg[indexKeyFrameProgCurr][0]);
		secondQuat = glm::quat_cast(keyFramesProg[indexKeyFrameProgNext][0]);
		//Slerp hace la interpolacion del quaternion
		finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
		//Se convierte el quaternion a una matriz de 4x4
		interpoltaedMatrix = glm::mat4_cast(finalQuat);
		//Se obtiene la traslaci�n en la matriz del frame i
		transformComp1 = keyFramesProg[indexKeyFrameProgCurr][0] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		//Se obtiene la traslaci�n en la matriz del frame i + 1
		transformComp2 = keyFramesProg[indexKeyFrameProgNext][0] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
		interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

		// Animacion KeyFrames
		glm::mat4 matrixGlobalAnimation = glm::translate(glm::mat4(1.0f), glm::vec3(10.75f, 9.75f, -23.3f));
		matrixGlobalAnimation = glm::rotate(matrixGlobalAnimation, 3.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		matrixGlobalAnimation = glm::scale(matrixGlobalAnimation, glm::vec3(0.5f, 0.5f, 0.5f));

		glm::mat4 keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;

		glm::mat4 matrixs6 = glm::translate(matrixs5, glm::vec3(0.3f, 0.0f, 0.0f));
		//copia
		/*	*/ glm::mat4 matrix3 = glm::rotate(keyFrameJoint, .2f, glm::vec3(1.0, 0.0f, 0.0));

		glm::mat4 matrixs8 = glm::translate(matrix3, glm::vec3(0.3f, 0.0f, 0.0f));
		glm::mat4 matrix5 = glm::rotate(matrixs8, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix5 = glm::rotate(matrix5, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		matrix5 = glm::rotate(matrix5, 1.5f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix5 = glm::translate(matrix5, glm::vec3(0.0f, -0.3f, 0.0f));
		glm::mat4 matrixs9 = glm::translate(matrix5, glm::vec3(0.0f, -0.3f, 0.0f));
		matrixs9 = glm::scale(matrixs9, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.render(matrixs9);
		matrix5 = glm::scale(matrix5, glm::vec3(0.1, 0.6, 0.2));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.render(matrix5);
		matrixs8 = glm::scale(matrixs8, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.render(matrixs8);
		matrix3 = glm::scale(matrix3, glm::vec3(0.5f, 0.15f, 0.15f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.render(matrix3);
		matrixs6 = glm::scale(matrixs6, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.render(matrixs6);



		glm::mat4 matrixs7 = glm::translate(matrixs5, glm::vec3(-0.3f, 0.0f, 0.0f));

		//segunda transf
		//Matriz de rotaci�n actual
		firstQuat = glm::quat_cast(keyFramesProg[indexKeyFrameProgCurr][1]);
		secondQuat = glm::quat_cast(keyFramesProg[indexKeyFrameProgNext][1]);
		//Slerp hace la interpolacion del quaternion
		finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
		//Se convierte el quaternion a una matriz de 4x4
		interpoltaedMatrix = glm::mat4_cast(finalQuat);
		//Se obtiene la traslaci�n en la matriz del frame i
		transformComp1 = keyFramesProg[indexKeyFrameProgCurr][1] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		//Se obtiene la traslaci�n en la matriz del frame i + 1
		transformComp2 = keyFramesProg[indexKeyFrameProgNext][1] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		//Se obtiene la interpolacion entre el frame i con el  frame i + 1
		//transformComp1 es el frame i
		//transformComp2 es el frame i + 1
		finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;

		//Unimos la matriz de interpolaci�n con la de traslacion
		interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

		// Animacion KeyFrames
		matrixGlobalAnimation = glm::translate(glm::mat4(1.0f), glm::vec3(10.75f, 9.75f, -23.2f));
		matrixGlobalAnimation = glm::rotate(matrixGlobalAnimation, 3.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		matrixGlobalAnimation = glm::scale(matrixGlobalAnimation, glm::vec3(0.5f, 0.5f, 0.5f));
		// Se modela siempre con los ejes de giro en el eje z
		// Articulacion 2
		keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;

		//copia
		/*	*/ glm::mat4 matrix4 = glm::rotate(keyFrameJoint, .2f, glm::vec3(1.0, 0.0f, 0.0));
		matrix4 = glm::translate(matrix4, glm::vec3(-0.25f, 0.0f, 0.0f));

		glm::mat4 matrixs10 = glm::translate(matrix4, glm::vec3(-0.3f, 0.0f, 0.0f));
		glm::mat4 matrix6 = glm::rotate(matrixs10, 0.0f, glm::vec3(0.0f, 0.0f, -1.0f));
		matrix6 = glm::rotate(matrix6, -0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		matrix6 = glm::rotate(matrix6, -1.5f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix6 = glm::translate(matrix6, glm::vec3(0.0f, -0.3f, 0.0f));
		glm::mat4 matrixs11 = glm::translate(matrix6, glm::vec3(0.0f, -0.3f, 0.0f));
		matrixs11 = glm::scale(matrixs11, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.render(matrixs11);
		matrix6 = glm::scale(matrix6, glm::vec3(0.1, 0.6, 0.2));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.render(matrix6);
		matrixs10 = glm::scale(matrixs10, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.render(matrixs10);
		matrix4 = glm::scale(matrix4, glm::vec3(0.5f, 0.15f, 0.15f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.render(matrix4);
		matrixs7 = glm::scale(matrixs7, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.render(matrixs7);

		matrixs5 = glm::scale(matrixs5, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.render(matrixs5);

		matrix0 = glm::scale(matrix0, glm::vec3(0.6f, 1.0f, 0.6f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.render(matrix0);
	}

	numPasosAnimProgCurr++;
	interpolation = numPasosAnimProgCurr / (float)numPasosAnimProg;

	if (interpolation >= 1.0) {
		interpolation = 0;
		numPasosAnimProgCurr = 0;
		indexKeyFrameProgCurr = indexKeyFrameProgNext;
		indexKeyFrameProgNext++;
	}

	if (indexKeyFrameProgNext > keyFramesProg.size() - 1) {
		interpolation = 0;
		indexKeyFrameProgCurr = 0;
		indexKeyFrameProgNext = 1;
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}