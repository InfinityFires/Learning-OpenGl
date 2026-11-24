//NOTE: NO LONGER ATTEMPT TO ADD INHERITENCE TO OBJECT CLASSES, WILL CAUSE ISSUES WITH LINKED LISTS.
//NOTE: DO NOT ATTEMPT TO CHANGE THE BIG ASS SWITCH STATEMENT:
//NOTE: ATTEMPT TO ADD UNIFORM BUFFER OBJECTS.
//NOTE: make a raycast system to detect which object the player is looking at.
//NOTE: FIGURE OUT HOW TO AVOID MEMORY LEAKS USING POINTERS IN A LINKED LIST
//NOTE: COMPLETE DELETION CODE FOR OBJECTS OF DIFFERENT TYPES.
//NOTE: IMPLEMENT FREETYPE FONTS.
#define _USE_MATH_DEFINES
#include <glad.h>
#include <glfw3.h>
#include <conio.h> 
#include <cstdlib> 
#include <iostream> 
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <string>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H  
#include <vector>
#include <map>
using namespace std;
GLenum const fmt1[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
GLenum const fmt2[] = { GL_RED, GL_RG, GL_SRGB, GL_SRGB_ALPHA };
std::fstream worldFile;
std::string currentFile = "NULL";
bool activateLights=true,menuOpen=false,firstMouse=true,createObjectMenuOpen=false;
int i = 0, j = 0, k = 0, scr_width = 800, scr_height = 600 ,worldFileEnd,subfps;
float totalTime;
unsigned int VAOTEXT,VBOTEXT,shaderProgramText,shaderProgramSkybox,shaderProgram,shaderProgramLight,shaderProgramMenu,transparencyCheckLoc;
unsigned int textColourLoc,textProjectionLoc,textTextLoc,skyboxTexLoc, skyboxViewLoc, skyboxProjLoc,lightIntensityLoc,modelLoc, modelLocMenu,buttonColorLoc, modelLocLight, projectionLoc, projectionLocLight, veiwLoc, veiwLocLight, viewPosLoc, sunLightDirLoc, texture1Loc, lightPosLoc, lightColorLoc, lightColorLocLight, materialAmbientIntensityLoc, materialSpecularIntensityLoc, materialDiffuseIntensityLoc, constLoc, linearLoc, quadraticLoc, normalMatLoc, normalMatLocLight, lightNumberLoc;
float lightIntensity = 1.0, currentFrame, cameraSpeedMultiplier = 5, cameraSpeed, mouseSensitivity = 0.1f, mixingAmount = 0.2, yaw = -90.0f, pitch = 0.0f, lastX = 800.0f / 2.0, lastY = 600.0 / 2.0, fov = 45.0f, deltaTime = 0.0f, lastFrame = 0.0f;
double xpos, ypos, xoffset,yoffset;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f), cameraFront = glm::vec3(0.0f, 0.0f, -1.0f), cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
unsigned int createShaderPrg(const char* vertexShaderSources, const char* fragmentShaderSources);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);
unsigned int VAOband, VBOband;
unsigned int VAOslider, VBOslider;
struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};
std::map<char, Character> Characters;
const char* textVertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec4 vertex; \n"
"out vec2 TexCoords;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
	"gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
	"TexCoords = vertex.zw;\n"
"}\n";
const char* textFragmentShaderSource =
"#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform sampler2D text;\n"
"uniform vec3 textColor;\n"
"void main()\n"
"{\n"
	"vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
	"color = vec4(textColor, 1.0) * sampled;\n"
"}\n";
const char* skyboxFragmentShaderSource=
"#version 330 core\n"
"out vec4 FragColor; \n"
"in vec3 TexCoords; \n"
"uniform samplerCube skybox; \n"
"void main()\n"
"{\n"
	"FragColor = texture(skybox, TexCoords); \n"
"}\n";
const char* skyboxVertexShaderSource = "#version 330 core\n"
"layout(location = 0) in vec3 aPos; \n"
"out vec3 TexCoords; \n"
"uniform mat4 projection; \n"
"uniform mat4 view; \n"
"void main()\n"
"{\n"
"    TexCoords = aPos; \n"
"    gl_Position = projection * view * vec4(aPos, 1.0); \n"
"}\n";
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"layout (location=2) in vec2 aTexCoord;"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 normalMatrix;\n"
"out vec3 normal;\n"
"out vec3 FragPos;\n"
"out vec2 texCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = projection*view*model*vec4(aPos, 1.0);\n"
"   normal=vec3(normalMatrix*vec4(aNormal,1.0));\n"
"	FragPos = vec3(model * vec4(aPos, 1.0));\n"
"   texCoord=aTexCoord;\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"struct pointLight {\n"
"	vec3 a;\n"
"   vec3 b;\n"
"	float c;\n"
"	float d;\n"
"	float e;\n"
"};\n"
"void pointLights(int i);\n"
"in vec3 normal;\n"
"in vec3 FragPos;\n"
"in vec2 texCoord;\n"
"uniform bool transparencyCheck;\n"
"uniform float lightIntensity;\n"
"uniform int lightNumber;"
"uniform pointLight pointLightArray[200];"
"uniform vec3 viewPos;\n"
"uniform vec3 sunLightDir;\n"
"uniform sampler2D texture1;\n"
"uniform sampler2D texture2;\n"
"uniform float materialAmbientIntensity;\n"
"uniform float materialSpecularIntensity;\n"
"uniform float materialDiffuseIntensity;\n"
"out vec4 FragColor;\n"
"vec3 viewDir = normalize(viewPos - FragPos);\n"
"vec3 globalDiffuse;\n"
"vec3 globalSpecular=vec3(0.0,0.0,0.0);\n"
"vec3 norm = normalize(normal); \n"
"void main()\n"
"{\n"		
"	int i;\n"
"   vec4 Tex=texture(texture1, texCoord);\n"		
"	vec3 diffuseTexture = Tex.rgb;\n"
"   if(!transparencyCheck && Tex.a < 1.0)\n"
"       discard;\n"
"	vec3 specularTexture = texture(texture2, texCoord).rgb;\n"
"	vec3 sunLightDir = normalize(sunLightDir);\n"
"	float diff = max(dot(norm, sunLightDir), 0.0);\n"
"	globalDiffuse = diff * vec3(1.0,1.0,1.0);\n"
"	for(i=0;i<lightNumber;i++)\n"
"   pointLights(i);\n"
"	FragColor=vec4(((vec3(1.0)*materialAmbientIntensity*diffuseTexture)+(globalDiffuse*diffuseTexture*materialDiffuseIntensity)+(globalSpecular*specularTexture*materialSpecularIntensity)),Tex.a);\n"
"}\n"
"void pointLights(int i)\n"
"{\n"
"   float distance = length(pointLightArray[i].b - FragPos);\n"
"   float attenuation = lightIntensity / (pointLightArray[i].c + pointLightArray[i].d * distance +pointLightArray[i].e * (distance * distance));\n"
"	vec3 lightDir = normalize(pointLightArray[i].b - FragPos);\n"
"	float diff = max(dot(norm, lightDir), 0.0);\n"
"	globalDiffuse += diff * pointLightArray[i].a*attenuation;\n"
"   vec3 halfwayDir = normalize(lightDir + viewDir);\n"
"	float spec = pow(max(dot(norm,halfwayDir), 0.0), 32);\n"
"	globalSpecular += 1.0 * spec * pointLightArray[i].a*attenuation;\n"
"}\0";

const char* lightVertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 modell;\n"
"uniform mat4 viewl;\n"
"uniform mat4 projectionl;\n"
"void main()\n"
"{\n"
"   gl_Position = projectionl*viewl*modell*vec4(aPos, 1.0);\n"
"}\0";

const char* lightFragmentShaderSource = "#version 330 core\n"
"uniform vec3 lightColorl;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor=vec4(lightColorl,1.0);\n"
"}\0";

const char* menuVertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
"   gl_Position = model*vec4(aPos, 1.0);\n"
"}\0";

const char* menuFragmentShaderSource = "#version 330 core\n"
"uniform vec3 buttonColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor=vec4(buttonColor,1.0);\n"
"}\0";

unsigned int loadCubemap(vector<std::string> faces);
class cooldown
{
public:
	static cooldown* first;
	static cooldown* last;
	cooldown* next;
	double max;
	double current;
	cooldown(double n)
	{
		if (first)
		{
			last->next = this;
			last = this;
		}
		else
		{
			first = this;
			last = this;
		}
		max = n;
		next = NULL;
		current = 0;
	}
	void updateCooldown(double dTime)
	{
		if (current < max)
		{
			current+=dTime;
		}
	}
	static void updateCooldowns(double dTime)
	{
		cooldown* innit = first;
		while (innit)
		{
			innit->updateCooldown(dTime);
			innit = innit->next;
		}
	}
	bool checkCooldown()
	{
		if (current < max)
			return false;
		current = 0;
		return true;
	}
};
cooldown* cooldown::first = NULL;
cooldown* cooldown::last = NULL;
cooldown tabCooldown(0.5);
cooldown createObejctCooldown(0.5);
class slider
{
public:
	static slider* first;
	static slider* last;
	static slider* currentSlider;
	int id;
	bool held;
	glm::vec3 sliderColor;
	glm::vec3 bandColor;
	glm::mat4 bandModel, sliderModel;
	glm::vec3 scaler,translater;
	float sliderPosition,x1,x2,y1,y2,bandLength,bandStart,bandEnd,translationFactor;
	slider* next;
	slider(glm::vec3 translate, glm::vec3 scale, glm::vec3 bandcol, glm::vec3 slidercol, int num,int start)
	{
		if (first)
		{
			last->next = this;
			last = this;
		}
		else
		{
			first = this;
			last = this;
		}
		next = NULL;
		scaler = scale;
		translater = translate;
		bandColor = bandcol;
		sliderColor = slidercol;
		bandModel = glm::mat4(1.0);
		bandModel = glm::scale(bandModel,scale);
		bandModel = glm::translate(bandModel, translate);
		id = num;
		held = false;
		sliderPosition = start;
		glm::vec4 tempVec1 = glm::vec4(-1, -0.05, 0.0, 1.0);
		glm::vec4 tempVec2 = glm::vec4(1, +0.05, 0.0, 1.0);
		tempVec1 = bandModel * tempVec1;
		tempVec2 = bandModel * tempVec2;
		bandStart = (tempVec1.x + 1) * scr_width / 2;
		bandEnd = (tempVec2.x + 1) * scr_width / 2 ;
		bandLength = bandEnd - bandStart;
		translationFactor = (((sliderPosition-0.5) * bandLength) / (scr_width / 4));
		sliderModel = glm::mat4(1.0);
		sliderModel = glm::scale(sliderModel, glm::vec3(scale.y, scale.y, scale.z));
		sliderModel = glm::translate(sliderModel, glm::vec3(translate.x + translationFactor, translate.y, translate.z));
		tempVec1 = glm::vec4(-0.15, -0.15, 0.0, 1.0);
		tempVec2 = glm::vec4(0.15, +0.15, 0.0, 1.0);
		tempVec1 = sliderModel * tempVec1;
		tempVec2 = sliderModel * tempVec2;
		x1 = (tempVec1.x + 1) * scr_width / 2;
		y1 = (1 - tempVec1.y) * scr_height / 2;
		x2 = (tempVec2.x + 1) * scr_width / 2;
		y2 = (1 - tempVec2.y) * scr_height / 2;
	}
	void renderBand()
	{
		glUniformMatrix4fv(modelLocMenu, 1, GL_FALSE, glm::value_ptr(bandModel));
		glUniform3f(buttonColorLoc, bandColor.x, bandColor.y, bandColor.z);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	void renderSlider()
	{
		glUniformMatrix4fv(modelLocMenu, 1, GL_FALSE, glm::value_ptr(sliderModel));
		if (held)
			glUniform3f(buttonColorLoc, sliderColor.x * 0.25, sliderColor.y * 0.25, sliderColor.z * 0.25);
		else if (this == currentSlider)
			glUniform3f(buttonColorLoc, sliderColor.x * 0.5, sliderColor.y * 0.5, sliderColor.z * 0.5);
		else
			glUniform3f(buttonColorLoc, sliderColor.x, sliderColor.y, sliderColor.z);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	static void renderObjects()
	{
		slider* innit = first;
		glBindVertexArray(VBOband);
		while (innit)
		{
			innit->renderBand();
			innit = innit->next;
		}
		innit = first;
		glBindVertexArray(VBOslider);
		while (innit)
		{
			innit->renderSlider();
			innit = innit->next;
		}
	}
	void checkCursor(double xPos, double yPos)
	{
		if (x1 <= xPos && xPos <= x2 && y2 <= yPos && yPos <= y1)
			currentSlider = this;
	}
	static void cursorChecks(double xPos, double yPos)
	{
		if (!currentSlider || !currentSlider->held)
		{
			currentSlider = NULL;
			slider* innit = first;
			if (innit)
			{
				do
				{
					innit->checkCursor(xPos, yPos);
					innit = innit->next;
				} while (innit && currentSlider != innit);

			}
		}
	}
	void sliderResize()
	{
		glm::vec4 tempVec1 = glm::vec4(-0.15, -0.15, 0.0, 1.0);
		glm::vec4 tempVec2 = glm::vec4(0.15, +0.15, 0.0, 1.0);
		tempVec1 = sliderModel * tempVec1;
		tempVec2 = sliderModel * tempVec2;
		x1 = (tempVec1.x + 1) * scr_width / 2;
		y1 = (1 - tempVec1.y) * scr_height / 2;
		x2 = (tempVec2.x + 1) * scr_width / 2;
		y2 = (1 - tempVec2.y) * scr_height / 2;
		tempVec1 = glm::vec4(-1, -0.05, 0.0, 1.0);
		tempVec2 = glm::vec4(1, +0.05, 0.0, 1.0);
		tempVec1 = bandModel * tempVec1;
		tempVec2 = bandModel * tempVec2;
		bandStart = (tempVec1.x + 1) * scr_width / 2;
		bandEnd = (tempVec2.x + 1) * scr_width / 2;
		bandLength = bandEnd - bandStart;
	}
	static void resizeSliders()
	{
		slider* innit = first;
		while (innit)
		{
			innit->sliderResize();
			innit = innit->next;
		}
	}
	void SLIDER_CLICK(double xPos)
	{
		sliderPosition = (xPos-bandStart) / bandLength;
		if (sliderPosition > 1) sliderPosition = 1;
		if (sliderPosition < 0) sliderPosition = 0;
		translationFactor = (((sliderPosition-0.5) * bandLength) / (scr_width / 4));
		sliderModel = glm::mat4(1.0);
		sliderModel = glm::scale(sliderModel, glm::vec3(scaler.y, scaler.y, scaler.z));
		sliderModel = glm::translate(sliderModel, glm::vec3(translater.x + translationFactor, translater.y, translater.z));
		glm::vec4 tempVec1 = glm::vec4(-0.15, -0.15, 0.0, 1.0);
		glm::vec4 tempVec2 = glm::vec4(0.15, +0.15, 0.0, 1.0);
		tempVec1 = sliderModel * tempVec1;
		tempVec2 = sliderModel * tempVec2;
		x1 = (tempVec1.x + 1) * scr_width / 2;
		y1 = (1 - tempVec1.y) * scr_height / 2;
		x2 = (tempVec2.x + 1) * scr_width / 2;
		y2 = (1 - tempVec2.y) * scr_height / 2;
		switch (id)
		{
		case 0:
			lightIntensity = sliderPosition*10;
		}
	}
};
slider* slider::first = NULL;
slider* slider::last = NULL;
slider* slider::currentSlider = NULL;
class material
{
public:
	static material* first, * last;
	string name;
	material* next;
	char specularTEX[64], diffuseTEX[64];
	unsigned int textureLoc;
	unsigned int specularTextureLoc;
	float diffuse, specular, ambient;
	material(const char* file, const char* speculartexture, float specularIntensity, float diffuseIntensity, float ambientIntensity,string matName)
	{
		if (first)
		{
			last->next = this;
			last = this;
		}
		else
		{
			first = this;
			last = this;
		}
		strcpy_s(specularTEX, speculartexture);
		strcpy_s(diffuseTEX, file);
		next = NULL;
		diffuse = diffuseIntensity;
		specular = specularIntensity;
		ambient = ambientIntensity;
		name = matName;
		glGenTextures(1, &textureLoc);
		glBindTexture(GL_TEXTURE_2D, textureLoc);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		int width, height, nrChannels;
		unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
		if (data)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, fmt2[nrChannels - 1], width, height, 0, fmt1[nrChannels - 1], GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture at location:" << file << std::endl;
		}
		glGenTextures(1, &specularTextureLoc);
		glBindTexture(GL_TEXTURE_2D, specularTextureLoc);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		data = stbi_load(speculartexture, &width, &height, &nrChannels, 0);
		if (data)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, fmt1[nrChannels -1], width, height, 0, fmt1[nrChannels -1], GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture at location:" << speculartexture << std::endl;
		}
	}
	void useMaterial()
	{
		glUniform1f(materialDiffuseIntensityLoc, diffuse);
		glUniform1f(materialSpecularIntensityLoc, specular);
		glUniform1f(materialAmbientIntensityLoc, ambient);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureLoc);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularTextureLoc);	
	}
	static material* findMaterial(string requiredName)
	{
		material* innit = first;
		while (innit && innit->name != requiredName)
			innit = innit->next;
		return innit;
	}
	void saveObject()
	{
		worldFile.write((char*)this, sizeof(material));
	}
	static void saveObjects()
	{
		material* innit = first;
		while (innit)
		{
			innit->saveObject();
			innit = innit->next;
		}
	}
	void innitMaterial()
	{
		glGenTextures(1, &textureLoc);
		glBindTexture(GL_TEXTURE_2D, textureLoc);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		int width, height, nrChannels;
		unsigned char* data = stbi_load(diffuseTEX, &width, &height, &nrChannels, 0);
		if (data)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, fmt2[nrChannels - 1], width, height, 0, fmt1[nrChannels - 1], GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture at location:" << diffuseTEX << std::endl;
		}
		glGenTextures(1, &specularTextureLoc);
		glBindTexture(GL_TEXTURE_2D, specularTextureLoc);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		data = stbi_load(specularTEX, &width, &height, &nrChannels, 0);
		if (data)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, fmt1[nrChannels - 1], width, height, 0, fmt1[nrChannels - 1], GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture at location:" << specularTEX << std::endl;
		}
	}
	static void deleteObject(int index)
	{
		material* pre=NULL, *innit = first;
		for (i = 0; i < index; i++)
		{
			pre = innit;
			innit = innit->next;
		}
		if (pre)
		{
			pre->next = innit->next;
			delete innit;
		}
		else
		{
			first = innit->next;
			delete innit;
		}
	}
};
material* material::first = NULL;
material* material::last = NULL;
material* materialDefault = NULL;
class object2D
{
public:
	static object2D* first;
	static object2D* last;
	glm::mat4 model;
	glm::mat4 normalMat;
	string matName;
	material* mater;
	object2D* next;
	object2D(glm::vec3 pos, glm::vec3 scale, float rotation, glm::vec3 axis,string materialName)
	{
		matName = materialName;
		mater = material::findMaterial(matName);
		if (!mater)
			mater = materialDefault;
		if (first)
		{
			last->next = this;
			last = this;
		}
		else
		{
			first = this;
			last = this;
		}
		next = NULL;
		model = glm::mat4(1.0);
		model = glm::rotate(model, rotation, axis);
		model = glm::translate(model, pos);
		model = glm::scale(model, scale);
		normalMat = glm::mat3(transpose(inverse(model)));
	}
	void renderObject()
	{
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(normalMat));
		mater->useMaterial();
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	static void renderObjects()
	{
		object2D* innit = first;
		while (innit)
		{
			innit->renderObject();
			innit = innit->next;
		}
	}
	void saveObject()
	{
		worldFile.write((char*)this, sizeof(object2D));
	}
	static void saveObjects()
	{
		object2D* innit = first;
		while (innit)
		{
			innit->saveObject();
			innit = innit->next;
		}
	}
	static void debugTest()
	{
		object2D* innit = first;
		while (innit)
		{
			std::cout << "objectFound"<<std::endl;
			innit = innit->next;
		}
	}
	void innitialiseMaterial()
	{
		mater = material::findMaterial(matName);
	}
	static void deleteObject(int index)
	{
		object2D* pre = NULL, * innit = first;
		for (i = 0; i < index; i++)
		{
			pre = innit;
			innit = innit->next;
		}
		if (pre)
		{
			pre->next = innit->next;
			delete innit;
		}
		else
		{
			first = innit->next;
			delete innit;
		}
	}
}; 
object2D* object2D::first = NULL;
object2D* object2D::last = NULL;
class objectCharacter
{
public:
	static objectCharacter* first;
	static objectCharacter* last;
	glm::vec3 pos, scale;
	glm::mat4 model;
	glm::mat4 normalMat;
	string matName;
	material* mater;
	objectCharacter* next;
	objectCharacter(glm::vec3 posi,glm::vec3 scalei,string materialName)
	{
		matName = materialName;
		mater = material::findMaterial(matName);
		if (!mater)
			mater = materialDefault;
		if (first)
		{
			last->next = this;
			last = this;
		}
		else
		{
			first = this;
			last = this;
		}
		next = NULL;
		model = glm::mat4(1.0);
		pos = posi;
		scale = scalei;
		normalMat = glm::mat4(1.0);
	}
	void renderObject()
	{
		glm::vec3 CharacterLookAt = cameraPos - pos;
		glm::normalize(CharacterLookAt);
		float characterYaw = atan2(CharacterLookAt.x, CharacterLookAt.z);
		float characterPitch = atan2(CharacterLookAt.y, sqrt(CharacterLookAt.z* CharacterLookAt.z+ CharacterLookAt.x * CharacterLookAt.x));
		model = glm::mat4(1.0);
		model = glm::translate(model, pos);
		model = glm::rotate(model, characterYaw, glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, characterPitch, glm::vec3(-1.0, 0.0, 0.0));
		model = glm::scale(model, scale);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		normalMat= glm::mat3(transpose(inverse(model)));
		glUniformMatrix4fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(normalMat));
		mater->useMaterial();
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	static void renderObjects()
	{
		objectCharacter* innit = first;
		while (innit)
		{
			innit->renderObject();
			innit = innit->next;
		}
	}
	void saveObject()
	{
		worldFile.write((char*)this, sizeof(objectCharacter));
	}
	static void saveObjects()
	{
		objectCharacter* innit = first;
		while (innit)
		{
			innit->saveObject();
			innit = innit->next;
		}
	}
	static void debugTest()
	{
		objectCharacter* innit = first;
		while (innit)
		{
			std::cout << "objectFound" << std::endl;
			innit = innit->next;
		}
	}
	void innitialiseMaterial()
	{
		mater = material::findMaterial(matName);
	}
	static void deleteObject(int index)
	{
		objectCharacter* pre = NULL, * innit = first;
		for (i = 0; i < index; i++)
		{
			pre = innit;
			innit = innit->next;
		}
		if (pre)
		{
			pre->next = innit->next;
			delete innit;
		}
		else
		{
			first = innit->next;
			delete innit;
		}
	}
};
objectCharacter* objectCharacter::first = NULL;
objectCharacter* objectCharacter::last = NULL;
class objectCuboid
{
public:
	static objectCuboid* first;
	static objectCuboid* last;
	glm::mat4 model;
	glm::mat4 normalMat;
	string matName;
	material* mater;
	objectCuboid* next;
	objectCuboid(glm::vec3 pos, glm::vec3 scale, float rotation, glm::vec3 axis, string materialName)
	{
		matName = materialName;
		mater = material::findMaterial(matName);
		if (!mater)
			mater = materialDefault;
		if (first)
		{
			last->next = this;
			last = this;
		}
		else
		{
			first = this;
			last = this;
		}
		next = NULL;
		model = glm::mat4(1.0);
		model = glm::rotate(model, rotation, axis);
		model = glm::translate(model, pos);
		model = glm::scale(model, scale);
		normalMat = glm::mat3(transpose(inverse(model)));
	}
	void renderObject()
	{
		glUniformMatrix4fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(normalMat));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mater->useMaterial();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	static void renderObjects()
	{
		objectCuboid* innit = first;
		while (innit)
		{
			innit->renderObject();
			innit = innit->next;
		}
	}
	void saveObject()
	{
		worldFile.write((char*)this, sizeof(objectCuboid));
	}
	static void saveObjects()
	{
		objectCuboid* innit = first;
		while (innit)
		{
			innit->saveObject();
			innit = innit->next;
		}
	}
	static void debugTest()
	{
		objectCuboid* innit = first;
		while (innit)
		{
			std::cout << "objectFound" << std::endl;
			innit = innit->next;
		}
	}
	void innitialiseMaterial()
	{
		mater = material::findMaterial(matName);
	}
	static void deleteObject(int index)
	{
		objectCuboid* pre = NULL, * innit = first;
		int i;
		for (i = 0; i < index; i++)
		{
			pre = innit;
			innit = innit->next;
		}
		if (pre!=NULL)
		{
			(pre->next) = (innit->next);
		}
		else
		{
			first = innit->next;
		}
	}
};
objectCuboid* objectCuboid::first = NULL;
objectCuboid* objectCuboid::last = NULL;
class objectLight
{
public:
	static objectLight* first;
	static objectLight* last;
	static int count;
	glm::mat4 model;
	glm::vec3 lightColor;
	glm::vec3 lightPos;
	float constant, linear, quadratic;
	objectLight* next;
	objectLight(glm::vec3 pos,glm::vec3 color,float con, float lin, float quad)
	{
		if (first)
		{
			last->next = this;
			last = this;
			count++;
		}
		else
		{
			first = this;
			last = this;
			count=1;
		}
		next = NULL;
		model = glm::mat4(1.0);
		model = glm::translate(model, pos);
		model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
		lightPos = pos;
		lightColor = color;
		constant = con;
		linear = lin;
		quadratic = quad;
	}
	void renderObject()
	{
		glUniformMatrix4fv(modelLocLight, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(lightColorLocLight, lightColor.x, lightColor.y, lightColor.z);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	static void renderObjects()
	{
		objectLight* innit = first;
		while (innit)
		{
			innit->renderObject();
			innit = innit->next;
		}
	}
	void saveObject()
	{
		worldFile.write((char*)this, sizeof(objectLight));
	}
	static void saveObjects()
	{
		objectLight* innit = first;
		while (innit)
		{
			innit->saveObject();
			innit = innit->next;
		}
	}
	static void debugTest()
	{
		objectLight* innit = first;
		while (innit)
		{
			std::cout << "objectFound" << std::endl;
			innit = innit->next;
		}
	}
	
	static void addLights()
	{
		glUniform1i(lightNumberLoc, count);
		glUniform1f(lightIntensityLoc, lightIntensity);
		objectLight* innit = first;
		i = 0;
		while(innit)
		{
			switch (i)
			{
			case 0:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[0].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[0].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[0].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[0].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[0].e"), innit->quadratic);
				break;
			case 1:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[1].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[1].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[1].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[1].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[1].e"), innit->quadratic);
				break;
			case 2:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[2].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[2].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[2].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[2].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[2].e"), innit->quadratic);
				break;
			case 3:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[3].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[3].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[3].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[3].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[3].e"), innit->quadratic);
				break;
			case 4:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[4].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[4].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[4].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[4].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[4].e"), innit->quadratic);
				break;
			case 5:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[5].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[5].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[5].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[5].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[5].e"), innit->quadratic);
				break;
			case 6:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[6].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[6].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[6].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[6].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[6].e"), innit->quadratic);
				break;
			case 7:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[7].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[7].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[7].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[7].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[7].e"), innit->quadratic);
				break;
			case 8:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[8].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[8].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[8].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[8].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[8].e"), innit->quadratic);
				break;
			case 9:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[9].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[9].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[9].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[9].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[9].e"), innit->quadratic);
				break;
			case 10:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[10].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[10].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[10].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[10].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[10].e"), innit->quadratic);
				break;
			case 11:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[11].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[11].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[11].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[11].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[11].e"), innit->quadratic);
				break;
			case 12:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[12].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[12].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[12].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[12].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[12].e"), innit->quadratic);
				break;
			case 13:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[13].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[13].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[13].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[13].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[13].e"), innit->quadratic);
				break;
			case 14:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[14].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[14].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[14].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[14].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[14].e"), innit->quadratic);
				break;
			case 15:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[15].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[15].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[15].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[15].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[15].e"), innit->quadratic);
				break;
			case 16:
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[16].a"), innit->lightColor.x, innit->lightColor.y, innit->lightColor.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[16].b"), innit->lightPos.x, innit->lightPos.y, innit->lightPos.z);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[16].c"), innit->constant);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[16].d"), innit->linear);
				glUniform1f(glGetUniformLocation(shaderProgram, "pointLightArray[16].e"), innit->quadratic);
				break;
			}
			i++;
			innit = innit->next;
		}
	}
	static void deleteObject(int index)
	{
		objectLight* pre = NULL, * innit = first;
		for (i = 0; i < index; i++)
		{
			pre = innit;
			innit = innit->next;
		}
		if (pre)
		{
			pre->next = innit->next;
			delete innit;
		}
		else
		{
			first = innit->next;
			delete innit;
		}
	}
};
int objectLight::count = 0;
objectLight* objectLight::first = NULL;
objectLight* objectLight::last = NULL;
class button
{
public:
	static button* first;
	static button* last;
	static button* currentButton;
	int id;
	bool held;
	glm::vec3 buttonColor;
	float x1, x2, y1, y2;
	glm::mat4 model;
	button* next;
	button(glm::vec3 translate, glm::vec3 scale, glm::vec3 color, int num)
	{
		if (first)
		{
			last->next = this;
			last = this;
		}
		else
		{
			first = this;
			last = this;
		}
		next = NULL;
		buttonColor = color;
		model = glm::mat4(1.0);
		model = glm::scale(model, scale);
		model = glm::translate(model, translate);
		glm::vec4 tempVec1 = glm::vec4(-0.5, -0.5, 0.0, 1.0);
		glm::vec4 tempVec2 = glm::vec4(+0.5, +0.5, 0.0, 1.0);
		tempVec1 = model * tempVec1;
		tempVec2 = model * tempVec2;
		x1 = (tempVec1.x + 1) * scr_width / 2;
		y1 = (1 - tempVec1.y) * scr_height / 2;
		x2 = (tempVec2.x + 1) * scr_width / 2;
		y2 = (1 - tempVec2.y) * scr_height / 2;
		id = num;
		held = false;
	}
	void renderObject()
	{
		glUniformMatrix4fv(modelLocMenu, 1, GL_FALSE, glm::value_ptr(model));
		if (held)
			glUniform3f(buttonColorLoc, buttonColor.x * 0.25, buttonColor.y * 0.25, buttonColor.z * 0.25);
		else if (this == currentButton)
			glUniform3f(buttonColorLoc, buttonColor.x * 0.5, buttonColor.y * 0.5, buttonColor.z * 0.5);
		else
			glUniform3f(buttonColorLoc, buttonColor.x, buttonColor.y, buttonColor.z);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	static void renderObjects()
	{
		button* innit = first;
		while (innit)
		{
			innit->renderObject();
			innit = innit->next;
		}
	}
	void checkCursor(double xPos, double yPos)
	{
		if (x1 <= xPos && xPos <= x2 && y2 <= yPos && yPos <= y1)
			currentButton = this;
	}
	static void cursorChecks(double xPos, double yPos)
	{
		currentButton = NULL;
		button* innit = first;
		if (innit)
		{
			do
			{
				innit->checkCursor(xPos, yPos);
				innit = innit->next;
			} while (innit && currentButton != innit);
		}
	}
	void buttonResize()
	{
		glm::vec4 tempVec1 = glm::vec4(-0.5, -0.5, 0.0, 1.0);
		glm::vec4 tempVec2 = glm::vec4(+0.5, +0.5, 0.0, 1.0);
		tempVec1 = model * tempVec1;
		tempVec2 = model * tempVec2;
		x1 = (tempVec1.x + 1) * scr_width / 2;
		y1 = (1 - tempVec1.y) * scr_height / 2;
		x2 = (tempVec2.x + 1) * scr_width / 2;
		y2 = (1 - tempVec2.y) * scr_height / 2;
	}
	static void resizeButtons()
	{
		button* innit = first;
		while (innit)
		{
			innit->buttonResize();
			innit = innit->next;
		}
	}
	void BUTTON_CLICK()
	{
		activateLights = !activateLights;
		glUseProgram(shaderProgram);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[0].a"), 0.0,0.0,0.0);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[1].a"), 0.0, 0.0, 0.0);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[2].a"), 0.0, 0.0, 0.0);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[3].a"), 0.0, 0.0, 0.0);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[4].a"), 0.0, 0.0, 0.0);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[5].a"), 0.0, 0.0, 0.0);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[6].a"), 0.0, 0.0, 0.0);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[7].a"), 0.0, 0.0, 0.0);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[8].a"), 0.0, 0.0, 0.0);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLightArray[9].a"), 0.0, 0.0, 0.0);
	}
};
button* button::first = NULL;
button* button::last = NULL;
button* button::currentButton = NULL;
class objectTransparentCuboid
{
public:
	static objectTransparentCuboid* first;
	static objectTransparentCuboid* last;
	glm::mat4 model;
	glm::mat4 normalMat;
	glm::vec3 position;
	string matName;
	material* mater;
	objectTransparentCuboid* next;
	objectTransparentCuboid(glm::vec3 pos, glm::vec3 scale, float rotation, glm::vec3 axis, string materialName)
	{
		matName = materialName;
		mater = material::findMaterial(matName);
		if (!mater)
			mater = materialDefault;
		if (first)
		{
			last->next = this;
			last = this;
		}
		else
		{
			first = this;
			last = this;
		}
		next = NULL;
		model = glm::mat4(1.0);
		model = glm::rotate(model, rotation, axis);
		model = glm::translate(model, pos);
		model = glm::scale(model, scale);
		normalMat = glm::mat3(transpose(inverse(model)));
		position = pos;
	}
	void renderObject()
	{
		glUniformMatrix4fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(normalMat));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mater->useMaterial();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	static void renderObjects()
	{
		objectTransparentCuboid* y, * z, * x, * w, * a=first,*newFirst=NULL;
		z = NULL;
		glUniform1i(transparencyCheckLoc, 1);
		while (first != NULL)
		{
			x = y = first;
			w = NULL;
			a = y->next;
			while (y->next != NULL)
			{
				if (glm::distance(y->position, cameraPos) < glm::distance(y->next->position, cameraPos))
				{
					w = y;
					x = y->next;
				}
				y = y->next;
			}
			if (w != NULL)
			{
				w->next = x->next;
			}
			else
			{
				first = a;
			}
			if (z == NULL)
			{
				z = x;
				x = y = first;
				newFirst = z;
			}
			else
			{
				z->next = x;
				x = y = first;
				z = z->next;
			}
		}
		first = newFirst;
		z = NULL;
		objectTransparentCuboid* innit = first;
		while (innit)
		{
			innit->renderObject();
			innit = innit->next;
		}
		glUniform1i(transparencyCheckLoc, 0);
	}
	void saveObject()
	{
		worldFile.write((char*)this, sizeof(objectTransparentCuboid));
	}
	static void saveObjects()
	{
		objectTransparentCuboid* innit = first;
		while (innit)
		{
			innit->saveObject();
			innit = innit->next;
		}
	}
	static void debugTest()
	{
		objectTransparentCuboid* innit = first;
		while (innit)
		{
			std::cout << "objectFound" << std::endl;
			innit = innit->next;
		}
	}
	void innitialiseMaterial()
	{
		mater = material::findMaterial(matName);
	}
	static void deleteObject(int index)
	{
		objectTransparentCuboid* pre = NULL, * innit = first;
		for (i = 0; i < index; i++)
		{
			pre = innit;
			innit = innit->next;
		}
		if (pre)
		{
			pre->next = innit->next;
			delete innit;
		}
		else
		{
			first = innit->next;
			delete innit;
		}
	}
};
objectTransparentCuboid* objectTransparentCuboid::first = NULL;
objectTransparentCuboid* objectTransparentCuboid::last = NULL;
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(800, 600, "GameRenderEngine1", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwWindowHint(GLFW_SAMPLES, 4);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	shaderProgram=createShaderPrg(vertexShaderSource, fragmentShaderSource);
	shaderProgramLight=createShaderPrg(lightVertexShaderSource, lightFragmentShaderSource);
	shaderProgramMenu=createShaderPrg(menuVertexShaderSource, menuFragmentShaderSource);
	shaderProgramSkybox = createShaderPrg(skyboxVertexShaderSource, skyboxFragmentShaderSource);
	shaderProgramText = createShaderPrg(textVertexShaderSource, textFragmentShaderSource);
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	float textureVertices[] = {
		// Back face
		0.0f, 0.0f, // Bottom-left
		1.0f, 1.0f, // Top-right
		1.0f, 0.0f, // Bottom-right
		1.0f, 1.0f, // Top-right
		0.0f, 0.0f, // Bottom-left
		0.0f, 1.0f, // Top-left

		// Front face
		0.0f, 0.0f, // Bottom-left
		1.0f, 0.0f, // Bottom-right
		1.0f, 1.0f, // Top-right
		1.0f, 1.0f, // Top-right
		0.0f, 1.0f, // Top-left
		0.0f, 0.0f, // Bottom-left

		// Left face
		1.0f, 0.0f, // Top-right
		1.0f, 1.0f, // Top-left
		0.0f, 1.0f, // Bottom-left
		0.0f, 1.0f, // Bottom-left
		0.0f, 0.0f, // Bottom-right
		1.0f, 0.0f, // Top-right

		// Right face
		1.0f, 0.0f, // Top-left
		0.0f, 1.0f, // Bottom-right
		1.0f, 1.0f, // Top-right
		0.0f, 1.0f, // Bottom-right
		1.0f, 0.0f, // Top-left
		0.0f, 0.0f, // Bottom-left

		// Bottom face
		0.0f, 1.0f, // Top-right
		1.0f, 1.0f, // Top-left
		1.0f, 0.0f, // Bottom-left
		1.0f, 0.0f, // Bottom-left
		0.0f, 0.0f, // Bottom-right
		0.0f, 1.0f, // Top-right

		// Top face
		0.0f, 1.0f, // Top-left
		1.0f, 0.0f, // Bottom-right
		1.0f, 1.0f, // Top-right
		1.0f, 0.0f, // Bottom-right
		0.0f, 1.0f, // Top-left
		0.0f, 0.0f  // Bottom-left
	};
	float vertices[] = {
		// Back face (normal: 0, 0, -1)
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f, // Bottom-left
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f, // Top-right
		 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f, // Bottom-right
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f, // Top-right
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f, // Bottom-left
		-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f, // Top-left

		// Front face (normal: 0, 0, 1)
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, // Bottom-left
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, // Bottom-right
		 0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  1.0f, // Top-right
		 0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  1.0f, // Top-right
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  1.0f, // Top-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, // Bottom-left

		// Left face (normal: -1, 0, 0)
		-0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, // Top-right
		-0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, // Top-left
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, // Bottom-left
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, // Bottom-left
		-0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, // Bottom-right
		-0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, // Top-right

		// Right face (normal: 1, 0, 0)
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // Top-left
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // Bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // Top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // Bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // Top-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // Bottom-left

		 // Bottom face (normal: 0, -1, 0)
		 -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f, // Top-right
		  0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f, // Top-left
		  0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f, // Bottom-left
		  0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f, // Bottom-left
		 -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f, // Bottom-right
		 -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f, // Top-right

		 // Top face (normal: 0, 1, 0)
		 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, // Top-left
		  0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // Bottom-right
		  0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, // Top-right
		  0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // Bottom-right
		 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, // Top-left
		 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f  // Bottom-left
	};
	float vertices2D[] = {
		0.5f,0.5f,-0.5,0.0f,  0.0f, 1.0f,
		-0.5,-0.5,-0.5,0.0f,  0.0f, 1.0f,
		0.5f,-0.5,-0.5,0.0f,  0.0f, 1.0f,
		-0.5,0.5f,-0.5,0.0f,  0.0f, 1.0f,
		0.5f,0.5f,-0.5,0.0f,  0.0f, 1.0f,
		-0.5,-0.5,-0.5,0.0f,  0.0f, 1.0f
	};
	float textureVertices2D[] =
	{
		1.0,1.0,
		0.0,0.0,
		1.0,0.0,
		0.0,1.0,
		1.0,1.0,
		0.0,0.0
	};
	float sliderVertices[] =
	{
		0,0.15,0.0,
		0,-0.15,0.0,
		0.15,0,0.0,
		-0.15,0,0.0,
		0,0.15,0.0,
		0,-0.15,0.0,
	};
	float bandVertices[] =
	{
		1.0,0.05,0.001,
		-1.0,-0.05,0.001,
		1.0,-0.05,0.001,
		-1.0,0.05,0.001,
		1.0,0.05,0.001,
		-1.0,-0.05,0.001
	};
	unsigned int shadowDepthMapFBO;
	glGenFramebuffers(1, &shadowDepthMapFBO);
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int VBO3D, VAO3D,VBOTex3D;
	glGenVertexArrays(1, &VAO3D);
	glGenBuffers(1, &VBO3D);
	glGenBuffers(1, &VBOTex3D);
	glBindVertexArray(VAO3D);

	glBindBuffer(GL_ARRAY_BUFFER, VBO3D);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBOTex3D);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureVertices), textureVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	unsigned int VBOsky, VAOsky;
	glGenVertexArrays(1, &VAOsky);
	glGenBuffers(1, &VBOsky);
	glBindVertexArray(VAOsky);
	glBindBuffer(GL_ARRAY_BUFFER, VBOsky);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int VBO2D, VAO2D, VBOTex2D;
	glGenVertexArrays(1, &VAO2D);
	glGenBuffers(1, &VBO2D);
	glGenBuffers(1, &VBOTex2D);
	glBindVertexArray(VAO2D);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2D);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2D), vertices2D, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBOTex2D);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureVertices2D), textureVertices2D, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	unsigned int VAOlight;
	glGenVertexArrays(1, &VAOlight);
	glBindVertexArray(VAOlight);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3D);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int VAOmenu;
	glGenVertexArrays(1, &VAOmenu);
	glBindVertexArray(VAOmenu);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2D);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenVertexArrays(1, &VAOslider);
	glGenBuffers(1, &VBOslider);
	glBindVertexArray(VAOslider);
	glBindBuffer(GL_ARRAY_BUFFER, VBOslider);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sliderVertices), sliderVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenVertexArrays(1, &VAOband);
	glGenBuffers(1, &VBOband);
	glBindVertexArray(VAOband);
	glBindBuffer(GL_ARRAY_BUFFER, VBOband);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bandVertices), bandVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	unsigned int VAOTEXT, VBOTEXT;
	glGenVertexArrays(1, &VAOTEXT);
	glGenBuffers(1, &VBOTEXT);
	glBindVertexArray(VAOTEXT);
	glBindBuffer(GL_ARRAY_BUFFER, VBOTEXT);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_FRAMEBUFFER_SRGB); //This enables gamma correction, which looks awful without correcting the textures back to a linear format. 
	vector<std::string> faces
	{
		"assets/right.jpg",
			"assets/left.jpg",
			"assets/top.jpg",
			"assets/bottom.jpg",
			"assets/front.jpg",
			"assets/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);
	stbi_set_flip_vertically_on_load(true);
	material infinityFires("assets/InfinityFires.png", "assets/InfinityFiresSpecular.png", 1.0, 0.6, 0.2,"assets/InfinityFires");
	materialDefault = &infinityFires;
	/*material lightMaterialx("lightTexture.png", "lightTexture.png", 1.0, 1.0, 1.0, "light");
	material colours1("colours.png", "coloursSpecular.png", 1.0, 0.4, 0.1,"colours");
	material background("background398.png", "background398_specular.png", 1.0, 0.9, 0.5,"BlockmanGo");
	material redGlass("redtransGlass.png", "redtransGlassSpec.png", 1.0, 0.9, 0.5,"redGlass");
	material glass("transGlass.png", "transGlassSpec.png", 1.0, 0.9, 0.5,"MCGlass");
	objectCuboid MASTER_CUBE(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0, 2.0, 1.0), 0, glm::vec3(1.0f, 0.0f, 0.0f), "InfinityFires");
	objectCuboid cube2(glm::vec3(3.0f, 0.0f, 3.0f), glm::vec3(1.0, 1.0, 1.0), 0, glm::vec3(1.0f, 0.0f, 0.0f), "colours");
	objectTransparentCuboid cube3(glm::vec3(6.0f, 7.0f, 3.0f), glm::vec3(2.0, 2.0, 2.0), 0, glm::vec3(1.0f, 0.0f, 0.0f), "MCGlass");
	objectTransparentCuboid cube4(glm::vec3(9.0f, 7.0f, 3.0f), glm::vec3(1.0, 3.0, 1.0), 0, glm::vec3(1.0f, 0.0f, 0.0f), "redGlass");
	object2D floor(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(20.0, 20.0, 1.0), M_PI_2, glm::vec3(1.0f, 0.0f, 0.0f), "BlockmanGo");
	objectLight light1(glm::vec3(1, 1, 1), glm::vec3(0.90, 0.3, 0.3), 1.0, 0.1, 0.05);
	objectLight light2(glm::vec3(2.0, 1.5, -2), glm::vec3(1.0, 0.2, 0.5), 1.0, 0.0, 0.001);
	objectLight light3(glm::vec3(-1.5, 1.5, 9.5), glm::vec3(0.0, 1.0, 0.8), 1.0, 0.05, 0.01);
	objectLight light4(glm::vec3(0.0, -3, -1.5), glm::vec3(1.0, 1.0, 3.0), 1.0, 0.1, 0.05);
	button LIGHTS(glm::vec3(0.5, 0.5, 0.0), glm::vec3(0.5, 0.5, 1.0), glm::vec3(0.8, 0.8, 0.2), 0);
	slider lightIntensitySlider(glm::vec3(0.0, -0.5, 0.0), glm::vec3(0.2, 0.5, 1.0), glm::vec3(0.0, 0.3, 0.0), glm::vec3(0.9, 0.0, 0.0), 0, 0.1);
	objectCharacter cat(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(2.0, 2.0, 1.0), "InfinityFires");*/
	glUseProgram(shaderProgramSkybox);
	skyboxProjLoc = glGetUniformLocation(shaderProgramSkybox, "projection");
	skyboxViewLoc = glGetUniformLocation(shaderProgramSkybox, "view");
	skyboxTexLoc = glGetUniformLocation(shaderProgramSkybox, "skybox");
	glUseProgram(shaderProgram);
	transparencyCheckLoc=glGetUniformLocation(shaderProgram, "transparencyCheck");
	modelLoc = glGetUniformLocation(shaderProgram, "model");
	lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
	lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
	projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	veiwLoc = glGetUniformLocation(shaderProgram, "view");
	viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
	sunLightDirLoc = glGetUniformLocation(shaderProgram, "sunLightDir");
	materialSpecularIntensityLoc = glGetUniformLocation(shaderProgram, "materialSpecularIntensity");
	materialAmbientIntensityLoc = glGetUniformLocation(shaderProgram, "materialAmbientIntensity");
	materialDiffuseIntensityLoc = glGetUniformLocation(shaderProgram, "materialDiffuseIntensity");
	normalMatLoc = glGetUniformLocation(shaderProgram, "normalMatrix");
	lightNumberLoc = glGetUniformLocation(shaderProgram, "lightNumber");
	lightIntensityLoc = glGetUniformLocation(shaderProgram, "lightIntensity");
	glUniform1i(transparencyCheckLoc, 0);
	glUseProgram(shaderProgramLight);
	modelLocLight = glGetUniformLocation(shaderProgramLight, "modell");
	lightColorLocLight = glGetUniformLocation(shaderProgramLight, "lightColorl");
	projectionLocLight = glGetUniformLocation(shaderProgramLight, "projectionl");
	veiwLocLight = glGetUniformLocation(shaderProgramLight, "viewl");
	glUseProgram(shaderProgramMenu);
	modelLocMenu = glGetUniformLocation(shaderProgramMenu, "model");
	buttonColorLoc = glGetUniformLocation(shaderProgramMenu, "buttonColor");
	glUseProgram(shaderProgramText);
	textTextLoc = glGetUniformLocation(shaderProgramText, "text");
	textProjectionLoc = glGetUniformLocation(shaderProgramText, "projection");
	textColourLoc = glGetUniformLocation(shaderProgramText, "textColor");
	glm::mat4 view;
	glm::mat4 projection;
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return -1;
	}

	FT_Face face;
	if (FT_New_Face(ft, "C:/Windows/Fonts/arial.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return -1;
	}
	FT_Set_Pixel_Sizes(face, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}
	glm::mat4 projectionURMOM;
	while (!glfwWindowShouldClose(window))
	{
		currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (!menuOpen)
		{
			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			projection = glm::mat4(1.0f);
			projection = glm::perspective(fov, (float)scr_width / (float)scr_height, 0.1f, 100.0f);
		}
		processInput(window);
		glm::mat4 projectionURMOM = glm::ortho(0.0f, static_cast<float>(scr_height), 0.0f, static_cast<float>(scr_width));
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//NOTE: YOU CANNOT SET UNIFORMS FOR ANOTHER SHADER PROGRAM WHEN THAT SHADER IS NOT ACTIVE.
		glDepthMask(GL_FALSE);
		glUseProgram(shaderProgramSkybox);
		glUniformMatrix4fv(skyboxProjLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(view))));
		glBindVertexArray(VAOsky);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
		// ... draw rest of the scene
		glUseProgram(shaderProgram);
		glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
		glUniform3f(sunLightDirLoc, 0.5, 1.0, 0.5);
		glUniformMatrix4fv(veiwLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		if(activateLights) objectLight::addLights();
		glBindVertexArray(VAO2D);
		object2D::renderObjects();
		objectCharacter::renderObjects();
		glBindVertexArray(VAO3D); 
		objectCuboid::renderObjects();
		if (activateLights)
		{
			glUseProgram(shaderProgramLight);
			glUniformMatrix4fv(veiwLocLight, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projectionLocLight, 1, GL_FALSE, glm::value_ptr(projection));
			glBindVertexArray(VAOlight);
			objectLight::renderObjects();
		}
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO3D);
		objectTransparentCuboid::renderObjects();
		if (menuOpen)
		{
			glUseProgram(shaderProgramMenu);
			glBindVertexArray(VAOmenu);
			button::renderObjects();
			slider::renderObjects();
		}
		glUseProgram(shaderProgramText);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAOTEXT);
		glBindBuffer(GL_ARRAY_BUFFER, VBOTEXT);
		glUniformMatrix4fv(textProjectionLoc, 1, GL_FALSE, glm::value_ptr(projectionURMOM));
		std::string m = "meow";
		RenderText(m,200.0, 200.0, 1.0, glm::vec3(1.0, 1.0, 1.0));
		cooldown::updateCooldowns(deltaTime);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO3D);
	glDeleteBuffers(1, &VBO3D);
	glDeleteVertexArrays(1, &VAO2D);
	glDeleteBuffers(1, &VBO2D);
	glDeleteProgram(shaderProgram);
	glfwTerminate();
	return 0;
}
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		while (currentFile == "NULL")
		{
			std::cout << "Enter file name: ";
			std::cin >> currentFile;
		}
		worldFile.open(currentFile + "_CUBOIDS", std::ios::out | std::ios::binary);
		objectCuboid::saveObjects();
		worldFile.close();
		worldFile.open(currentFile + "_2D", std::ios::out | std::ios::binary);
		object2D::saveObjects();
		worldFile.close();
		worldFile.open(currentFile + "_lights", std::ios::out | std::ios::binary);
		objectLight::saveObjects();
		worldFile.close();
		worldFile.open(currentFile + "_TRANSCUBE", std::ios::out | std::ios::binary);
		objectTransparentCuboid::saveObjects();
		worldFile.close();
		worldFile.open(currentFile + "_CHARA", std::ios::out | std::ios::binary);
		objectCharacter::saveObjects();
		worldFile.close();
		worldFile.open(currentFile + "_MATER", std::ios::out | std::ios::binary);
		material::saveObjects();
		worldFile.close();
		std::cout << "File successfully saved" << std::endl;
		glfwSetWindowShouldClose(window, true);
	}
	cameraSpeed = static_cast<float>(cameraSpeedMultiplier * deltaTime);
	if(!menuOpen)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			cameraPos += cameraSpeed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			cameraPos -= cameraSpeed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			cameraPos += cameraSpeed * cameraUp;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
			cameraPos -= cameraSpeed * cameraUp;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		while(currentFile == "NULL")
		{
			std::cout << "Enter file name: ";
			std::cin >> currentFile;
			currentFile = "saves/" + currentFile + "/" + currentFile;
		} 
		worldFile.open(currentFile+"_CUBOIDS", std::ios::out | std::ios::binary);
		objectCuboid::saveObjects();
		worldFile.close();
		worldFile.open(currentFile + "_2D", std::ios::out | std::ios::binary);
		object2D::saveObjects();
		worldFile.close();
		worldFile.open(currentFile + "_lights", std::ios::out | std::ios::binary);
		objectLight::saveObjects();
		worldFile.close();
		worldFile.open(currentFile + "_TRANSCUBE", std::ios::out | std::ios::binary);
		objectTransparentCuboid::saveObjects();
		worldFile.close();
		worldFile.open(currentFile + "_CHARA", std::ios::out | std::ios::binary);
		objectCharacter::saveObjects();
		worldFile.close();
		worldFile.open(currentFile + "_MATER", std::ios::out | std::ios::binary);
		material::saveObjects();
		worldFile.close();
		std::cout << "File successfully saved" << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		do
		{
			std::cout << "Enter file name: ";
			std::cin >> currentFile;
			currentFile = "saves/" + currentFile + "/" + currentFile;
		}
		while (currentFile == "NULL");
		std::cout << "File successfully read" << std::endl;
		worldFile.open(currentFile + "_MATER", std::ios::in | std::ios::binary);
		material::first = NULL;
		material::last = NULL;
		worldFile.seekg(0, std::ios::end);
		worldFileEnd = worldFile.tellg();
		worldFile.seekg(0, std::ios::beg);
		while (worldFile.tellg() < worldFileEnd)
		{
			material* currentLight = new material("assets/InfinityFires.png", "assets/InfinityFiresSpecular.png", 1.0, 0.6, 0.2, "InfinityFires");
			worldFile.read((char*)currentLight, sizeof(material));
			currentLight->innitMaterial();
		}
		worldFile.close();
		worldFile.open(currentFile + "_CUBOIDS", std::ios::in | std::ios::binary);
		objectCuboid::first = NULL;
		objectCuboid::last = NULL;
		worldFile.seekg(0, std::ios::end);
		worldFileEnd = worldFile.tellg();
		worldFile.seekg(0, std::ios::beg);
		cout << "staring loop.";
		while(worldFile.tellg() < worldFileEnd)
		{
			objectCuboid* currentCuboid =  new objectCuboid(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0, 1.0, 1.0), 0, glm::vec3(1.0f, 0.0f, 0.0f), "");
			worldFile.read((char*)currentCuboid, sizeof(objectCuboid));
			currentCuboid->innitialiseMaterial();
		}
		worldFile.close();
		worldFile.open(currentFile + "_2D", std::ios::in | std::ios::binary);
		object2D::first = NULL;
		object2D::last = NULL;
		worldFile.seekg(0, std::ios::end);
		worldFileEnd = worldFile.tellg();
		worldFile.seekg(0, std::ios::beg);
		while(worldFile.tellg()<worldFileEnd)
		{
			object2D* current2D =  new object2D(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0, 1.0, 1.0), 0, glm::vec3(1.0f, 0.0f, 0.0f), "");
			worldFile.read((char*)current2D, sizeof(objectCuboid));
			current2D->innitialiseMaterial();
		}
		worldFile.close();
		worldFile.open(currentFile + "_lights", std::ios::in | std::ios::binary);
		objectLight::first = NULL;
		objectLight::last = NULL;
		worldFile.seekg(0, std::ios::end);
		worldFileEnd = worldFile.tellg();
		worldFile.seekg(0, std::ios::beg);
		while (worldFile.tellg() < worldFileEnd)
		{
			objectLight* currentLight = new objectLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0, 1.0, 1.0), 1.0,0.2,0.1);
			worldFile.read((char*)currentLight, sizeof(objectLight));
		}
		worldFile.close();
		worldFile.open(currentFile + "_CHARA", std::ios::in | std::ios::binary);
		objectCharacter::first = NULL;
		objectCharacter::last = NULL;
		worldFile.seekg(0, std::ios::end);
		worldFileEnd = worldFile.tellg();
		worldFile.seekg(0, std::ios::beg);
		while (worldFile.tellg() < worldFileEnd)
		{
			objectCharacter* currentLight = new objectCharacter(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0, 1.0, 1.0), "");
			worldFile.read((char*)currentLight, sizeof(objectCharacter));
			currentLight->innitialiseMaterial();
		}
		worldFile.close();
		worldFile.open(currentFile + "_TRANSCUBE", std::ios::in | std::ios::binary);
		objectTransparentCuboid::first = NULL;
		objectTransparentCuboid::last = NULL;
		worldFile.seekg(0, std::ios::end);
		worldFileEnd = worldFile.tellg();
		worldFile.seekg(0, std::ios::beg);
		while (worldFile.tellg() < worldFileEnd)
		{
			objectTransparentCuboid* currentLight = new objectTransparentCuboid(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0, 1.0, 1.0), 0, glm::vec3(1.0f, 0.0f, 0.0f), "");;
			worldFile.read((char*)currentLight, sizeof(objectTransparentCuboid));
			currentLight->innitialiseMaterial();
		}
		std::cout << "File successfully read" << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS&& tabCooldown.checkCooldown())
	{
		if (menuOpen)
		{
			menuOpen = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			menuOpen = true;
			firstMouse = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && createObejctCooldown.checkCooldown())
	{
		if (createObjectMenuOpen)
		{
			createObjectMenuOpen = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			createObjectMenuOpen = true;
			int objType;
			std::cout << "cuboid:1\n2D plane:2\nCharacter:3\nTransparent Cuboid:4\nLight:5\nmaterial:6\nEnter Object Type: ";
			std::cin >> objType;
			firstMouse = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			objectCuboid* cube = NULL;
			object2D* twod = NULL;
			objectCharacter* chara = NULL;
			objectTransparentCuboid* transcube = NULL;
			objectLight* light = NULL;
			material* matter = NULL;
			glm::vec3 cubeScale(0.0), cubePos(0.0), cubeAxis(0.0);
			string matName,diffTex,specTex;
			material* meow;
			float cubeAngle;
			switch (objType)
			{
			case 1:
				std::cout << "\nEnter Location: ";
				std::cin >> cubePos.x >> cubePos.y >> cubePos.z;
				std::cout << "\nEnter Scale: ";
				std::cin >> cubeScale.x >> cubeScale.y >> cubeScale.z;
				std::cout << "\nEnter angle: ";
				std::cin >> cubeAngle;
				std::cout << "\nEnter axis: ";
				std::cin >> cubeAxis.x >> cubeAxis.y >> cubeAxis.z;
				while (true)
				{
					cout << "Enter material name: ";
					cin >> matName;
					meow = material::findMaterial(matName);
					if (meow)
						break;
					cout << "Enter a valid material name" << endl;
				}
				cube=new objectCuboid(cubePos, cubeScale, cubeAngle, cubeAxis, matName);
				break;
			case 2:
				std::cout << "\nEnter Location: ";
				std::cin >> cubePos.x >> cubePos.y >> cubePos.z;
				std::cout << "\nEnter Scale: ";
				std::cin >> cubeScale.x >> cubeScale.y >> cubeScale.z;
				std::cout << "\nEnter angle: ";
				std::cin >> cubeAngle;
				std::cout << "\nEnter axis: ";
				std::cin >> cubeAxis.x >> cubeAxis.y >> cubeAxis.z;
				while (true)
				{
					cout << "Enter material name: ";
					cin >> matName;
					meow = material::findMaterial(matName);
					if (meow)
						break;
					cout << "Enter a valid material name" << endl;
				}
				twod=new object2D(cubePos, cubeScale, cubeAngle, cubeAxis, matName);
				break;
			case 3:
				std::cout << "\nEnter Location: ";
				std::cin >> cubePos.x >> cubePos.y >> cubePos.z;
				std::cout << "\nEnter Scale: ";
				std::cin >> cubeScale.x >> cubeScale.y >> cubeScale.z;
				while (true)
				{
					cout << "Enter material name: ";
					cin >> matName;
					meow = material::findMaterial(matName);
					if (meow)
						break;
					cout << "Enter a valid material name" << endl;
				}
				chara = new objectCharacter(cubePos, cubeScale, matName);
				break;
			case 4:
				std::cout << "\nEnter Location: ";
				std::cin >> cubePos.x >> cubePos.y >> cubePos.z;
				std::cout << "\nEnter Scale: ";
				std::cin >> cubeScale.x >> cubeScale.y >> cubeScale.z;
				std::cout << "\nEnter angle: ";
				std::cin >> cubeAngle;
				std::cout << "\nEnter axis: ";
				std::cin >> cubeAxis.x >> cubeAxis.y >> cubeAxis.z;
				while (true)
				{
					cout << "Enter material name: ";
					cin >> matName;
					meow = material::findMaterial(matName);
					if (meow)
						break;
					cout << "Enter a valid material name" << endl;
				}
				transcube = new objectTransparentCuboid(cubePos, cubeScale, cubeAngle, cubeAxis, matName);
				break;
			case 5:
				std::cout << "\nEnter Location: ";
				std::cin >> cubePos.x >> cubePos.y >> cubePos.z;
				std::cout << "\nEnter colour: ";
				std::cin >> cubeAxis.x >> cubeAxis.y >> cubeAxis.z;
				std::cout << "\nEnter attenuation constants: ";
				std::cin >> cubeScale.x >> cubeScale.y >> cubeScale.z;
				light = new objectLight(cubePos, cubeAxis, cubeScale.x,cubeScale.y,cubeScale.z);
				break;
			case 6:
				cout << "\nEnter the diffuse texture location: ";
				cin >> diffTex;
				cout << "\nEnter the specular texture location: ";
				cin >> specTex;
				cout << "\nEnter the specular, diffuse, and ambient intensities: ";
				cin >> cubePos.x >> cubePos.y >> cubePos.z;
				cout << "Enter material name: ";
				cin >> matName;
				matter = new material(diffTex.c_str(), specTex.c_str(), cubePos.x, cubePos.y, cubePos.z, matName);
			default:
				std::cout << "An object of the given index does not exist:";
			}
			createObjectMenuOpen = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && createObejctCooldown.checkCooldown())
	{
		int objType;
		std::cout << "cuboid:1\n2D plane:2\nCharacter:3\nTransparent Cuboid:4\nLight:5\nmaterial:6\nEnter Object Type: ";
		std::cin >> objType;
		objectCuboid* cube = NULL;
		object2D* twod = NULL;
		objectCharacter* chara = NULL;
		objectTransparentCuboid* transcube = NULL;
		objectLight* light = NULL;
		material* matter = NULL;
		glm::vec3 cubeScale(0.0), cubePos(0.0), cubeAxis(0.0);
		string matName, diffTex, specTex;
		material* meow;
		float cubeAngle;
		int wiory = 0;
		switch (objType)
		{
		case 1:
			cube = objectCuboid::first;
			while(cube!=NULL)
			{
				cout << wiory << ": Material:" << cube->matName<<endl;
				cube = cube->next;
				wiory++;
			}
			cout << "Enter the index to delete: ";
			cin >> wiory;
			cube = NULL;
			objectCuboid::deleteObject(wiory);
			cout << "Successfully deleted.";
			break;
		case 2:
			std::cout << "\nEnter Location: ";
			std::cin >> cubePos.x >> cubePos.y >> cubePos.z;
			std::cout << "\nEnter Scale: ";
			std::cin >> cubeScale.x >> cubeScale.y >> cubeScale.z;
			std::cout << "\nEnter angle: ";
			std::cin >> cubeAngle;
			std::cout << "\nEnter axis: ";
			std::cin >> cubeAxis.x >> cubeAxis.y >> cubeAxis.z;
			while (true)
			{
				cout << "Enter material name: ";
				cin >> matName;
				meow = material::findMaterial(matName);
				if (meow)
					break;
				cout << "Enter a valid material name" << endl;
			}
			twod = new object2D(cubePos, cubeScale, cubeAngle, cubeAxis, matName);
			break;
		case 3:
			std::cout << "\nEnter Location: ";
			std::cin >> cubePos.x >> cubePos.y >> cubePos.z;
			std::cout << "\nEnter Scale: ";
			std::cin >> cubeScale.x >> cubeScale.y >> cubeScale.z;
			while (true)
			{
				cout << "Enter material name: ";
				cin >> matName;
				meow = material::findMaterial(matName);
				if (meow)
					break;
				cout << "Enter a valid material name" << endl;
			}
			chara = new objectCharacter(cubePos, cubeScale, matName);
			break;
		case 4:
			std::cout << "\nEnter Location: ";
			std::cin >> cubePos.x >> cubePos.y >> cubePos.z;
			std::cout << "\nEnter Scale: ";
			std::cin >> cubeScale.x >> cubeScale.y >> cubeScale.z;
			std::cout << "\nEnter angle: ";
			std::cin >> cubeAngle;
			std::cout << "\nEnter axis: ";
			std::cin >> cubeAxis.x >> cubeAxis.y >> cubeAxis.z;
			while (true)
			{
				cout << "Enter material name: ";
				cin >> matName;
				meow = material::findMaterial(matName);
				if (meow)
					break;
				cout << "Enter a valid material name" << endl;
			}
			transcube = new objectTransparentCuboid(cubePos, cubeScale, cubeAngle, cubeAxis, matName);
			break;
		case 5:
			std::cout << "\nEnter Location: ";
			std::cin >> cubePos.x >> cubePos.y >> cubePos.z;
			std::cout << "\nEnter colour: ";
			std::cin >> cubeAxis.x >> cubeAxis.y >> cubeAxis.z;
			std::cout << "\nEnter attenuation constants: ";
			std::cin >> cubeScale.x >> cubeScale.y >> cubeScale.z;
			light = new objectLight(cubePos, cubeAxis, cubeScale.x, cubeScale.y, cubeScale.z);
			break;
		case 6:
			cout << "\nEnter the diffuse texture location: ";
			cin >> diffTex;
			cout << "\nEnter the specular texture location: ";
			cin >> specTex;
			cout << "\nEnter the specular, diffuse, and ambient intensities: ";
			cin >> cubePos.x >> cubePos.y >> cubePos.z;
			cout << "Enter material name: ";
			cin >> matName;
			matter = new material(diffTex.c_str(), specTex.c_str(), cubePos.x, cubePos.y, cubePos.z, matName);
		default:
			std::cout << "An object of the given index does not exist:";
		}
	}
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
	scr_width = width;
	scr_height = height;
	button::resizeButtons();
	glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (menuOpen)
	{
		button::cursorChecks(xposIn, yposIn);
		slider::cursorChecks(xposIn, yposIn);
		if (slider::currentSlider && slider::currentSlider->held == true)
		{
			slider::currentSlider->SLIDER_CLICK(xposIn);
		}
	}
	else
	{
		xpos = xposIn;
		ypos = yposIn;
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		xoffset = xpos - lastX;
		yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;
		xoffset *= mouseSensitivity;
		yoffset *= mouseSensitivity;
		yaw += xoffset;
		pitch += yoffset;
		if (pitch > 89.99f)
			pitch = 89.99f;
		if (pitch < -89.99f)
			pitch = -89.99f;
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);
	}
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cameraSpeedMultiplier += (float)yoffset*0.5;
	cameraSpeedMultiplier = (cameraSpeedMultiplier < 0) ? 0:cameraSpeedMultiplier;
}
unsigned int createShaderPrg(const char* vertexShaderS,const char* fragmentShaderS)
{
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderS, NULL);
	glCompileShader(vertexShader);
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderS, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	unsigned int shaderProgramm;
	shaderProgramm = glCreateProgram();
	glAttachShader(shaderProgramm, vertexShader);
	glAttachShader(shaderProgramm, fragmentShader);
	glLinkProgram(shaderProgramm);
	glGetProgramiv(shaderProgramm, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramm, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	return shaderProgramm;
}
void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods)
{
	if (menuOpen)
	{

		if (button::currentButton)
		{
			if (butt == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			{
				button::currentButton->BUTTON_CLICK();
			}
			else if (button::currentButton->held && butt == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
			{
				button::currentButton->held = false;
			}
		}
		else if (slider::currentSlider)
		{
			if (butt == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			{
				slider::currentSlider->held=true;
			}
			else if (slider::currentSlider->held && butt == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
			{
				slider::currentSlider->held = false;
			}
		}
	}
}
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
void RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
	glUniform3f(textColourLoc, color.x, color.y, color.z);
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
}
