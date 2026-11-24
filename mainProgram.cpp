#include <glad.h>
#include <glfw3.h>
#include <conio.h> /*for and getch()*/
#include <cstdlib> /*for system clear*/
#include <fstream>
#include <fstream>//for file handling
#include <iostream> /*for std::cout and std::cin*/
#include <string>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
int i = 0, j = 0, k = 0,enemyEncounterReturn=0,scr_width=800,scr_height=600;
std::string eAK, js, itemNames[3] = {"Nothing","Wood","Stone"};
float mixingAmount = 0.2;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height); 
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec3 Normal;\n"
"out vec3 FragPos;\n"
"void main()\n"
"{\n"
"   gl_Position = projection*view*model*vec4(aPos, 1.0);\n"
"	FragPos = vec3(model * vec4(aPos, 1.0));\n"
"	Normal = mat3(transpose(inverse(model))) * aNormal;  \n"//Note that this inverse calculation is very cpu heavy
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 Normal;\n"
"in vec3 FragPos;\n"
"out vec4 FragColor;\n"
"uniform vec3 objectColor;\n"
"uniform vec3 lightColor;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"
"void main()\n"
"{\n"
"	vec3 norm = normalize(Normal);\n"
"	vec3 lightDir = normalize(lightPos - FragPos);\n"
"	float diff = max(dot(norm, lightDir), 0.0);\n"
"	vec3 diffuse = diff * lightColor;\n"
"	vec3 viewDir = normalize(viewPos - FragPos);\n"
"	vec3 reflectDir = reflect(-lightDir, norm);\n"
"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);\n"
"	vec3 specular = 1.0 * spec * lightColor;\n"
"   FragColor = vec4((lightColor*vec3(0.1)+diffuse+specular)*objectColor,1.0);\n"
"}\0";
const char* fragmentShaderSourceLight =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(1.0); // set all 4 vector values to 1.0\n"
"}\0";
struct spell //Linked list for character attacks and stuff
{
	int attack;
	int pp;
	spell* next;
};
struct option //linked list for room options
{
	std::string optionName;
	char optionType;
	std::string optionData;
	option* next;
};
void enterAnyKey()
{
	std::cout << "\n\nEnter any key to continue:";
	eAK = _getch();
	system("cls");
}
class entity
{
public:
	std::string entityName;
	float health, attack, defence;
	int loyalty, inventory[10];
	spell* spellFirst;
	spell* spellLast;
	entity(std::string n, float h, float a, float d, int l)
	{
		entityName = n;
		health = h;
		attack = a;
		defence = d;
		loyalty = l;
		spellFirst = NULL;
		spellLast = NULL;
		for (i = 0; i < 10; i++)
		{
			inventory[i] = 0;
		}
	}
	void addSpell(int spellAttack, int spellPP)
	{
		spell* newSpell = new spell;
		newSpell->attack = spellAttack;
		newSpell->pp = spellPP;
		newSpell->next = NULL;
		if (spellFirst == NULL)
		{
			spellFirst = newSpell;
			spellLast = spellFirst;
		}
		else
		{
			spellLast->next = newSpell;
			spellLast = newSpell;
		}
	}
	void displaySpells()
	{
		spell* currentSpell = spellFirst;
		while (currentSpell != NULL)
		{
			std::cout << "Damage: " << currentSpell->attack << "PP: " << currentSpell->pp;
			currentSpell = currentSpell->next;
		}
	}
	int inventoryAddItem(int itemID)
	{
		int inventoryFlag = 0;
		for (i = 0; i < sizeof(inventory) / 2 - 1; i++)
		{
			if (inventory[i] == 0)
			{
				inventory[i] = itemID;
				inventoryFlag = 1;
				break;
			}
		}
		return inventoryFlag;
	}
};
entity infinity("Infinity", 100, 0, 5, 0), sylvia("Sylvia", 500, 20, 30, -100);
bool enemyEncounter(entity urmom)
{
	int deathFlag = 1;
	int encounterChoiceVariable;
	system("cls");
	std::cout << "You Encountered an Enemy!";
	while (true)
	{
		std::cout << "\n1:Item\n2:Attack\n3:Check\nEnter your choice:";
		std::cin >> encounterChoiceVariable;
		if (encounterChoiceVariable == 1)
		{
			std::cout << "Inventory management";
		}
		else if (encounterChoiceVariable == 2)
		{
			urmom.health -= infinity.attack;
			std::cout << "You dealt %d damage to the enemy!", infinity.attack;
		}
		else if (encounterChoiceVariable == 3)
		{
			system("cls");
			urmom.displaySpells();
			enterAnyKey();
		}
		else
		{
			std::cout << "Invalid input";
			continue;
		}
		if (urmom.health <= 0)
		{
			break;
		}
		infinity.health -= urmom.attack;
		std::cout << "The enemy dealt %d damage!", urmom.attack;
		if (infinity.health <= 0)
		{
			deathFlag = -1;
			std::cout << "You died.";
			break;
		}
	}
	return deathFlag;
}
class room //class containing different features of each room
{
public:
	option* startingOption;
	option* lastOption;
	std::string roomName="Default_Room";
	room* previousRoom;
	room* nextRoom;
	room(std::string n, room* prevRoom)
	{
		roomName = n;
		previousRoom = prevRoom;
		prevRoom->nextRoom = this;
		nextRoom = NULL;
		lastOption = NULL;
		startingOption = NULL;
	}
	room(std::string n)
	{
		roomName = n;
		previousRoom = NULL;
		nextRoom = NULL;
		lastOption = NULL;
		startingOption = NULL;
	}
	void addOption(std::string optName,char optType,std::string optData)
	{
		option* newOption = new option;
		newOption->optionName = optName;
		newOption->optionType = optType;
		newOption->optionData = optData;
		newOption->next = NULL;
		if (startingOption == NULL)
		{
			startingOption=newOption;
			lastOption = startingOption;
		}
		else
		{
			lastOption->next = newOption;
			lastOption= newOption;
		}
	}
	void removeOption(option* currentOption)
	{
		option* prevOption = startingOption;
		while(prevOption->next!=currentOption)
		{	
			prevOption = prevOption->next;
		}
		prevOption->next = currentOption->next;
		delete currentOption;
	}
	room* display()
	{
		int choice;
		std::cout << roomName << "\n\n";
		i = 1;
		option* currentOption = startingOption;
		while (currentOption !=NULL)
		{
			std::cout << i << ". " << currentOption->optionName << "\n";
			currentOption = currentOption->next;
			i++;
		}
		std::cout << ":";
		std::cin >> choice;
		choice--;
		if (choice > i || choice < 0)
		{
			std::cout << "Enter a valid option.";
			return this;
		}
		else
		{
			currentOption = startingOption;
			for (i = choice; i > 0; i--)
			{
				currentOption = currentOption->next;
			}
			if (currentOption->optionType == 'a')
			{
				return nextRoom;
			}
			else if (currentOption->optionType == 'b')
			{
				system("cls");
				std::cout << currentOption->optionData;
				enterAnyKey();
				return this;
			}
			else if (currentOption->optionType == 'c')
			{
				system("cls");
				if (infinity.inventoryAddItem(std::stoi(currentOption->optionData)))
				{
					removeOption(currentOption);
					std::cout << "The " << itemNames[std::stoi(currentOption->optionData)] << " was added to your inventory";
					enterAnyKey();
				}
			}
			else if (currentOption->optionType == 'e')
			{
				switch (std::stoi(currentOption->optionData))
				{
				case 1:
					enemyEncounterReturn = enemyEncounter(sylvia);
					break;
				default:
					std::cout << "Error, character not found";
					enterAnyKey();
					return NULL;
				}
				switch (enemyEncounterReturn)
				{
				case 1:
					removeOption(currentOption);
					return this;
					break;
				case -1:
					return NULL;
					break;
				default:
					std::cout << "Error, enemyEncounterReturn returned unknown value.";
					enterAnyKey();
					return NULL;
				}
			}
		}
	}
};
int main()
{
	std::fstream save;
	int starting_choice, game_file_choice;
	/*
	while (true)
	{
		std::cout << "1.Start Game\n2.Settings\n3.Achivements\n4.Exit\n:";
		std::cin >> starting_choice;
		system("cls");
		if (starting_choice == 1)
		{
			std::cout << "1.Continue from previous save\n2.Start a new game\n:";
			std::cin >> game_file_choice;
			system("cls");
			if (game_file_choice == 1)
			{
			}
			else if (game_file_choice == 2)
			{
				room startingRoom("Hello World");
				startingRoom.addOption("Meow", 'b', "My first option");
				startingRoom.addOption("Leave", 'a', "");
				sylvia.addSpell(100, 10);
				sylvia.addSpell(200, 5);
				sylvia.addSpell(300, 2);
				room* playerLocation = &startingRoom;
				std::string a;
				break;
			}
			else
			{
				std::cout << "Enter a valid option.";
			}
		}
		else if (starting_choice == 2)
		{
			
		}
		else if (starting_choice == 3)
		{
			

		}
		else if (starting_choice == 4)
		{

		}
		else {
			std::cout << "Enter a valid option.";
		}
	}
	*/
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //core profile is modern OpenGL
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(800, 600, "Trinity", NULL, NULL);
	if (window == NULL)//checks if a glfw window has been successfully created or not.
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) //Checks if glad has been loaded or not
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// build and compile our shader program
	// ------------------------------------
	// vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	unsigned int fragmentShaderLight = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderLight, 1, &fragmentShaderSourceLight, NULL);
	glCompileShader(fragmentShaderLight);
	// check for shader compile errors
	glGetShaderiv(fragmentShaderLight, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderLight, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	unsigned int shaderProgramLight = glCreateProgram();
	glAttachShader(shaderProgramLight, vertexShader);
	glAttachShader(shaderProgramLight, fragmentShaderLight);
	glLinkProgram(shaderProgramLight);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glGetProgramiv(shaderProgramLight, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramLight, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(fragmentShaderLight);
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	unsigned int VBO,VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glm::vec3 lightPos(3.0f, 1.0f, 4.0f);
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glUseProgram(shaderProgram);
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(fov, (float)scr_width / (float)scr_height, 0.1f, 100.0f);
		unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		unsigned int camposLOC = glGetUniformLocation(shaderProgram, "viewPos");
		glUniform3f(camposLOC, cameraPos.x, cameraPos.y, cameraPos.z);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		processInput(window);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(VAO); 
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f,0.0f,0.0f));
		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		unsigned int objcolLOC = glGetUniformLocation(shaderProgram, "objectColor");
		unsigned int lightcolLOC = glGetUniformLocation(shaderProgram, "lightColor");
		unsigned int lightlocLoc = glGetUniformLocation(shaderProgram, "lightPos");
		glUniform3f(objcolLOC, 1.0f, 0.5f, 0.31f);
		glUniform3f(lightcolLOC, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightlocLoc, lightPos.x, lightPos.y, lightPos.z);
		glDrawArrays(GL_TRIANGLES, 0,36);
		glUseProgram(shaderProgramLight);
		projectionLoc = glGetUniformLocation(shaderProgramLight, "projection");
		viewLoc = glGetUniformLocation(shaderProgramLight, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		modelLoc = glGetUniformLocation(shaderProgramLight, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = static_cast<float>(2.5 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) //what to do when the window size changes
{
	scr_width = width;
	scr_height = height;
	glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

