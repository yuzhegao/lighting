/*************************************************
用于遍历整个3d模型的数据集，并且截取各个视角的渲染结果，
经过图像处理后，生成line drawing 用于训练
这里是用于生成CubeData数据集的img的程序
*************************************************/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ShowMat.h>

#include <learnopengl/shader_s.h>

#include <FreeImage.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

#include <opencv2/opencv.hpp>  
#include<opencv2/highgui/highgui.hpp>  
#include<opencv2/imgproc/imgproc.hpp> 


using namespace std;
using namespace cv;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
int Init();
void read_off(const std::string filename);
void grab(string img_name);
void img_proccess(string img_name);
void calNormal();


// settings
const unsigned int WIDTH = 256;
const unsigned int HEIGHT = 256;
GLFWwindow* window;


float *verBuffer; //顶点坐标缓冲区
unsigned int  *indBuffer; //绘制索引缓冲区
float *NormalBuffer; //各顶点法线向量缓冲区

int nVertices = 0;//顶点数
int nFaces = 0;//面数
int nEdges;//边数

double max_x = -1000000, max_y = -1000000, max_z = -1000000;
double min_x = INFINITE, min_y = INFINITE, min_z = INFINITE;
float mLength = 0;
int camera_index = 0;

string modelpath = "D:/MyCode/cube_dataset/off(2998)/";
string imgpath = "D:/MyCode/cube_dataset/img(2998x13)";// 这俩string其实没有用

int main()
{
	Init();
	Shader ourShader("lightingshader2.vs", "lightingshader2.fs");
	unsigned int VBO, VAO, EBO;

	std::vector<glm::vec3> camera_pos = { glm::vec3(0.5, 0.5, 0.4), //透视视角x8
		glm::vec3(0.4, 0.5, 0.5),
		glm::vec3(-0.4, 0.5, 0.5),
		glm::vec3(-0.5, 0.5, 0.4),
		glm::vec3(-0.5, 0.5, -0.4),
		glm::vec3(-0.4, 0.5, -0.5),
		glm::vec3(0.4, 0.5, -0.5),
		glm::vec3(0.5, 0.5, -0.4),

		glm::vec3(0.000001, 0.812, 0.0), //正视视角x5
		glm::vec3(0.0, 0.0, 0.812),
		glm::vec3(0.812, 0.0, 0.0),
		glm::vec3(0.0, 0.0, -0.812),
		glm::vec3(-0.812, 0.0, 0.0), };

	int off_index = 0;
	char off_filename[30], img_name[30],end_img_name[30];
	
	for (off_index = 714; off_index < 3000; off_index++)
	{
		sprintf(off_filename, "D:/MyCode/cube_dataset/off(2998)/%d.off", off_index);

		if (_access(off_filename, 0) == -1){ 
			std::cout << off_filename << " not exist" << std::endl;
			continue; }
		read_off(off_filename);
		calNormal();
		mLength = max(max_y - min_y, max(max_x - min_x, max_z - min_z));
		cout << "max length: " << mLength << endl;
		cout << "min_x:" << min_x << " min_y:" << min_y << " min_z:" << min_z << endl;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * nVertices, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)* 3 * nVertices, verBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)* 3 * nVertices, sizeof(float)* 3 * nVertices, NormalBuffer);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(float)* 3 * nVertices));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 3 * nFaces, indBuffer, GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		while (!glfwWindowShouldClose(window))
		{
			processInput(window);

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 transform;
			transform = glm::scale(transform, glm::vec3(1.f / mLength));
			transform = glm::translate(transform, glm::vec3(-(max_x + min_x) / 2, -(max_y + min_y) / 2, -(max_z + min_z) / 2));
			glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = glm::lookAt(camera_pos[camera_index] * (float)2.0, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			ourShader.use();
			unsigned int ModelLoc = glGetUniformLocation(ourShader.ID, "model");
			glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(transform));
			unsigned int ViewLoc = glGetUniformLocation(ourShader.ID, "view");
			glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, glm::value_ptr(view));
			unsigned int ProjLoc = glGetUniformLocation(ourShader.ID, "perspective");
			glUniformMatrix4fv(ProjLoc, 1, GL_FALSE, glm::value_ptr(projection));

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, nFaces * 3, GL_UNSIGNED_INT, 0);

			glfwSwapBuffers(window);
			glfwPollEvents();

			sprintf(img_name, "D:/MyCode/cube_dataset/img(2998x13)/%d_%d.jpg", off_index, camera_index);
			grab(img_name);
			img_proccess(img_name);
			camera_index += 1;
			camera_index %= 13;
			std::cout << "camera_index: " << camera_index << std::endl;

			sprintf(end_img_name, "D:/MyCode/cube_dataset/img(2998x13)/%d_12.jpg", off_index);
			if (_access(end_img_name, 0) != -1){ break; }			
		}
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);

		max_x = -1000000; max_y = -1000000; max_z = -1000000;
		min_x = INFINITE; min_y = INFINITE; min_z = INFINITE;
	}
	while (1);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	/*
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
	camera_index += 1;
	camera_index %= 13;
	std::cout << "camera_index: " << camera_index << std::endl;
	}*/
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif


	window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
}

void read_off(const std::string filename)
{
	if (filename.empty()){
		return;
	}
	std::ifstream fin;
	int i = 0;
	fin.open(filename);
	std::string str;
	fin >> str;             //str的内容为OFF
	fin >> nVertices;        //顶点数
	fin >> nFaces;           //面数
	fin >> nEdges;           //边数
	std::cout << str << " " << nVertices << " " << nFaces << " " << nEdges << std::endl;

	verBuffer = new float[nVertices * 3];
	indBuffer = new unsigned int[nFaces * 3];

	for (i = 0; i < nVertices; i++){
		float x = 0; float y = 0; float z = 0;
		fin >> x; fin >> y; fin >> z;

		verBuffer[3 * i] = x;
		verBuffer[3 * i + 1] = y;
		verBuffer[3 * i + 2] = z;
		if (x < min_x){ min_x = x; }
		if (x > max_x){ max_x = x; }
		if (y < min_y){ min_y = y; }
		if (y > max_y){ max_y = y; }
		if (z < min_z){ min_z = z; }
		if (z > max_z){ max_z = z; }
	}
	cout << endl;

	for (i = 0; i < nFaces; i++)            //读取每个面的信息
	{
		int n1 = 0;
		int n2 = 0;                         //每行的第一个为顶点数，
		int n3 = 0;
		int n4 = 0;
		fin >> n1;
		fin >> n2;
		fin >> n3;
		fin >> n4;
		indBuffer[3 * i] = n2;
		indBuffer[3 * i + 1] = n3;
		indBuffer[3 * i + 2] = n4;
	}
	fin.close();
	std::cout << filename << "load success" << endl;
}

void grab(string img_name)
{
	unsigned char *mpixels = new unsigned char[WIDTH * HEIGHT * 3];
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, mpixels);
	glReadBuffer(GL_BACK);
	for (int i = 0; i < (int)WIDTH*HEIGHT * 3; i += 3)
	{
		mpixels[i] ^= mpixels[i + 2] ^= mpixels[i] ^= mpixels[i + 2];
	}
	FIBITMAP* bitmap = FreeImage_Allocate(WIDTH, HEIGHT, 24, 8, 8, 8);

	for (int y = 0; y < FreeImage_GetHeight(bitmap); y++)
	{
		BYTE *bits = FreeImage_GetScanLine(bitmap, y);
		for (int x = 0; x < FreeImage_GetWidth(bitmap); x++)
		{
			bits[0] = mpixels[(y*WIDTH + x) * 3 + 0];
			bits[1] = mpixels[(y*WIDTH + x) * 3 + 1];
			bits[2] = mpixels[(y*WIDTH + x) * 3 + 2];
			bits += 3;
		}
	}

	FreeImage_Save(FIF_JPEG, bitmap, img_name.c_str(), JPEG_DEFAULT);

	FreeImage_Unload(bitmap);

}

void img_proccess(string img_name)
{
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y, dst;

	Mat src = imread(img_name, 0);

	Sobel(src, grad_x, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);

	Sobel(src, grad_y, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);

	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst);
	dst = 255 - dst;
	imwrite(img_name, dst);
}

void calNormal()
{
	NormalBuffer = new float[nVertices * 3];
	memset(NormalBuffer, 0, sizeof(NormalBuffer));
	int idx = 0;
	for (idx = 0; idx < nFaces * 3; idx += 3){
		//一个三角面片的三个顶点的索引
		unsigned int index0 = indBuffer[idx];
		unsigned int index1 = indBuffer[idx + 1];
		unsigned int index2 = indBuffer[idx + 2];

		//得到该面片的三个顶点的坐标
		glm::vec3 vertex1 = glm::vec3(verBuffer[index0 * 3], verBuffer[index0 * 3 + 1], verBuffer[index0 * 3 + 2]);
		glm::vec3 vertex2 = glm::vec3(verBuffer[index1 * 3], verBuffer[index1 * 3 + 1], verBuffer[index1 * 3 + 2]);
		glm::vec3 vertex3 = glm::vec3(verBuffer[index2 * 3], verBuffer[index2 * 3 + 1], verBuffer[index2 * 3 + 2]);

		glm::vec3 v1 = vertex2 - vertex1;
		glm::vec3 v2 = vertex3 - vertex2;
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);
		//第一个顶点的法向量
		NormalBuffer[index0 * 3] = normal.x;
		NormalBuffer[index0 * 3 + 1] = normal.y;
		NormalBuffer[index0 * 3 + 2] = normal.z;
		//第2个顶点的法向量
		NormalBuffer[index1 * 3] = normal.x;
		NormalBuffer[index1 * 3 + 1] = normal.y;
		NormalBuffer[index1 * 3 + 2] = normal.z;
		//第3个顶点的法向量
		NormalBuffer[index2 * 3] = normal.x;
		NormalBuffer[index2 * 3 + 1] = normal.y;
		NormalBuffer[index2 * 3 + 2] = normal.z;
	}
	cout << "the normal infomation" << endl;

	//进行法向量的归一化
	for (idx = 0; idx < nVertices * 3; idx += 3){
		float Nx = NormalBuffer[idx];
		float Ny = NormalBuffer[idx + 1];
		float Nz = NormalBuffer[idx + 2];

		Nx /= sqrt(Nx*Nx + Ny*Ny + Nz*Nz);
		Ny /= sqrt(Nx*Nx + Ny*Ny + Nz*Nz);
		Nz /= sqrt(Nx*Nx + Ny*Ny + Nz*Nz);

		NormalBuffer[idx] = Nx;
		NormalBuffer[idx + 1] = Ny;
		NormalBuffer[idx + 2] = Nz;

		//cout << "[" << Nx << "," << Ny << "," << Nz << "]" << endl;
	}
}