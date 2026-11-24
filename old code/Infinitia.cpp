#define _USE_MATH_DEFINES
/* Define _USE_MATH_DEFINES before including math.h to expose these macro
 * definitions for common math constants.  These are placed under an #ifdef
 * since these commonly-defined names are not part of the C/C++ standards.
 */

 /* Definitions of useful mathematical constants
  * M_E        - e
  * M_LOG2E    - log2(e)
  * M_LOG10E   - log10(e)
  * M_LN2      - ln(2)
  * M_LN10     - ln(10)
  * M_PI       - pi
  * M_PI_2     - pi/2
  * M_PI_4     - pi/4
  * M_1_PI     - 1/pi
  * M_2_PI     - 2/pi
  * M_2_SQRTPI - 2/sqrt(pi)
  * M_SQRT2    - sqrt(2)
  * M_SQRT1_2  - 1/sqrt(2)
  */
#include <glfw3.h> /*Initialises the library to allow usage of OpenGL using the GLFW API*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <cmath> /*better version of math.h*/
#include <string.h>
#include <GL/GLU.h>/*glue is a C++ library that provides class wrappers for OpenGL 3 objects such as vertex buffer objects or shader programs*/
#include <GL/GL.h>/*standard opengl library*/
#include <string.h>
#include <iostream>
#include <fstream>
double CharacterXPosition =0, CharacterZPosition = 0, CharacterYPosition =0, CameraYaw = 0, CameraPitch = 0, xpos = 0, ypos = 0, fRate = 0.016666667, pXSpeed = 0, pYSpeed = 0, pZSpeed = 0, mousePos = 0,playerLookX,playerLookY,playerLookZ, playerLookXStart, playerLookXEnd, playerLookYStart, playerLookYEnd, playerLookZStart, playerLookZEnd,M_2PI=M_PI*2;
int k = 0, i = 1, windowLength = 800, windowHeight = 450, cXPos = 50, cYPos = 50, speed = 6, j = 0, rem = 0, chunkX = 0, chunkY = 0, chunkZ = 0, chunk[512] = { 0 }, chunkInitializationPosition = 0, blockIsLookingAt = -1, faceIsLookingAt = -1, playerLookAtShotestDistance = 6, playerBlockLookAtX, playerBlockLookAtY, playerBlockLookAtZ, rayCastBlockX=0,rayCastBlockY=0,rayCastBlockZ=0;
float A = 0.0, B = 0.0, C = -7.0, cRadius = -10.0;\
void raycast(float iniX, float iniY, float iniZ,float Pitch,float Yaw, int &X, int &Y, int &Z)
{
    float XRatio = 1 / (cos(Pitch) * cos(Yaw)), YRatio = 1 / sin(Pitch), ZRatio = 1 / (cos(Pitch) * sin(Yaw)),XDisplacement,YDisplacement,ZDisplacement;
    int XDir=1, YDir=1, ZDir=1;
    X = int(iniX);
    Y = int(iniY);
    Z = int(iniZ);
    if (XRatio < 0)
    {
        XDisplacement = XRatio * (floor(iniX)-iniX);
        XDir = -XDir;
        XRatio = -XRatio;
    }
    else
    {
        XDisplacement = XRatio * (ceil(iniX)-iniX);
    }
    if (YRatio < 0)
    {
        YDisplacement = YRatio * (floor(iniY)-iniY);
        YDir = -YDir;
        YRatio = -YRatio;
    }
    else
    {
        YDisplacement = YRatio * (ceil(iniY) - iniY);
    }
    if (ZRatio < 0)
    {
        ZDisplacement = ZRatio * (floor(iniZ)-iniZ);
        ZDir = -ZDir;
        ZRatio = -ZRatio;
    }
    else
    {
        ZDisplacement = ZRatio * (ceil(iniZ) - iniZ);
    }
    while(true)
    {
        if (XDisplacement <= YDisplacement)
        {
            if (XDisplacement <= ZDisplacement)
            {
                if (XDisplacement <= 5)
                {
                    X += XDir;
                    XDisplacement += XRatio;
                }
                else
                {
                    break;
                }
            }
            else
            {
                if (ZDisplacement <= 5)
                {
                    Z += ZDir;
                    ZDisplacement += ZRatio;
                }
                else
                {
                    break;
                }
            }

        }
        else if(YDisplacement<=ZDisplacement)
        {
            if (YDisplacement <= 5)
            {
                Y += YDir;
                YDisplacement += YRatio;
            }
            else
            {
                break;
            }
        }
        else
        {
            if (ZDisplacement <= 5)
            {
                Z += ZDir;
                ZDisplacement += ZRatio;
            }
            else
            {
                break;
            }
        }
        if (X <=7 && X >= 0 && Y <=7 && Y >= 0 && Z <=7 && Z >= 0)
        {
            if (chunk[Z + Y * 8 + X * 64] == 1)
            {
                break;
            }
        }
    }
}
void generateChunk()
{
    for (chunkInitializationPosition = 0; chunkInitializationPosition < 512; chunkInitializationPosition++)
    {
        chunk[chunkInitializationPosition] = 1;
    }
}
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cRadius -= yoffset/2.5;
}
static void initGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    glClearDepth(1.0f);                   
    glEnable(GL_DEPTH_TEST);   
    glDepthFunc(GL_LEQUAL);    
    glShadeModel(GL_SMOOTH);   
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  
}
void renderChunk()
{
    glColor3f(0.5f, 0.0f, 1.0f);
    for (chunkX=0; chunkX < 8; chunkX++) 
    {
        for (chunkY=0; chunkY < 8; chunkY++)
        {
            for (chunkZ=0; chunkZ < 8; chunkZ++)
            {
                if (chunk[(chunkZ + chunkY * 8 + chunkX * 64)] == 1)
                {
                    glTranslatef(chunkX, chunkY, chunkZ);
                    glBegin(GL_TRIANGLES);

                    // Back face (z = 0)
                    glVertex3f(0.0f, 0.0f, 0.0f);
                    glVertex3f(1.0f, 0.0f, 0.0f);
                    glVertex3f(1.0f, 1.0f, 0.0f);

                    glVertex3f(1.0f, 1.0f, 0.0f);
                    glVertex3f(0.0f, 1.0f, 0.0f);
                    glVertex3f(0.0f, 0.0f, 0.0f);

                    // Front face (z = 1)
                    glVertex3f(0.0f, 0.0f, 1.0f);
                    glVertex3f(1.0f, 1.0f, 1.0f);
                    glVertex3f(1.0f, 0.0f, 1.0f);

                    glVertex3f(1.0f, 1.0f, 1.0f);
                    glVertex3f(0.0f, 0.0f, 1.0f);
                    glVertex3f(0.0f, 1.0f, 1.0f);

                    // Left face (x = 0)
                    glVertex3f(0.0f, 0.0f, 0.0f);
                    glVertex3f(0.0f, 1.0f, 0.0f);
                    glVertex3f(0.0f, 1.0f, 1.0f);

                    glVertex3f(0.0f, 0.0f, 0.0f);
                    glVertex3f(0.0f, 1.0f, 1.0f);
                    glVertex3f(0.0f, 0.0f, 1.0f);

                    // Right face (x = 1)
                    glVertex3f(1.0f, 0.0f, 0.0f);
                    glVertex3f(1.0f, 1.0f, 1.0f);
                    glVertex3f(1.0f, 1.0f, 0.0f);

                    glVertex3f(1.0f, 0.0f, 0.0f);
                    glVertex3f(1.0f, 0.0f, 1.0f);
                    glVertex3f(1.0f, 1.0f, 1.0f);

                    // Bottom face (y = 0)
                    glVertex3f(0.0f, 0.0f, 0.0f);
                    glVertex3f(1.0f, 0.0f, 0.0f);
                    glVertex3f(1.0f, 0.0f, 1.0f);

                    glVertex3f(0.0f, 0.0f, 0.0f);
                    glVertex3f(1.0f, 0.0f, 1.0f);
                    glVertex3f(0.0f, 0.0f, 1.0f);

                    // Top face (y = 1)
                    glVertex3f(0.0f, 1.0f, 0.0f);
                    glVertex3f(1.0f, 1.0f, 1.0f);
                    glVertex3f(1.0f, 1.0f, 0.0f);

                    glVertex3f(0.0f, 1.0f, 0.0f);
                    glVertex3f(0.0f, 1.0f, 1.0f);
                    glVertex3f(1.0f, 1.0f, 1.0f);

                    glEnd();

                    glTranslatef(-chunkX, -chunkY, -chunkZ);
                }

            }
        }
    }
}
void renderTargetBlock()
{
    glColor3f(0.0f, 0.0f, 0.0f);
    glTranslatef(rayCastBlockX, rayCastBlockY, rayCastBlockZ);
    glBegin(GL_TRIANGLES);

    // Back face (z = 0)
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);

    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);

    // Front face (z = 1)
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);

    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);

    // Left face (x = 0)
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);

    // Right face (x = 1)
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);

    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);

    // Bottom face (y = 0)
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);

    // Top face (y = 1)
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);

    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);

    glEnd();

    glTranslatef(-rayCastBlockX, -rayCastBlockY, -rayCastBlockZ);
}
void character() 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cRadius * cos(CameraPitch) * cos(CameraYaw), cRadius * sin(CameraPitch), cRadius * cos(CameraPitch) * sin(CameraYaw), 0, 0, 0, 0, 1, 0);
    glBegin(GL_LINES); 
    glColor3f(1.0f, 0.0f, 0.0f); 
    glVertex3f(0.75f, -2.5f, 0.75f);
    glVertex3f(0.75f, -2.5f, -0.75f);
    glVertex3f(0.75f, -2.5f, 0.75f);
    glVertex3f(-0.75f, -2.5f, 0.75f);
    glVertex3f(-0.75f, -2.5f, -0.75f);
    glVertex3f(0.75f, -2.5f, -0.75f);
    glVertex3f(-0.75f, -2.5f, -0.75f);
    glVertex3f(-0.75f, -2.5f, 0.75f);
    glVertex3f(0.75f, 1.25f, 0.75f);
    glVertex3f(0.75f, 1.25f, -0.75f);
    glVertex3f(0.75f, 1.25f, 0.75f);
    glVertex3f(-0.75f, 1.25f, 0.75f);
    glVertex3f(-0.75f, 1.25f, -0.75f);
    glVertex3f(0.75f, 1.25f, -0.75f);
    glVertex3f(-0.75f, 1.25f, -0.75f);
    glVertex3f(-0.75f, 1.25f, 0.75f);
    glVertex3f(-0.75f, 1.25f, 0.75f);
    glVertex3f(-0.75f, -2.5f, 0.75f);
    glVertex3f(-0.75f, 1.25f, -0.75f);
    glVertex3f(-0.75f, -2.5f, -0.75f);
    glVertex3f(0.75f, 1.25f, -0.75f);
    glVertex3f(0.75f, -2.5f, -0.75f);
    glVertex3f(-0.75f, 1.25f, -0.75f);
    glVertex3f(-0.75f, -2.5f, -0.75f);
    glVertex3f(-0.75f, 1.25f, 0.75f);
    glVertex3f(-0.75f, -2.5f, 0.75f);
    glVertex3f(0.75f, 1.25f, 0.75f);
    glVertex3f(0.75f, -2.5f, 0.75f);
    glVertex3f(0.75f, 1.25f, -0.75f);
    glVertex3f(0.75f, -2.5f, -0.75f);
    glVertex3f(0.75f, 1.25f, 0.75f);
    glVertex3f(0.75f, -2.5f, 0.75f);
    glEnd();
    glTranslatef(-CharacterXPosition, -CharacterYPosition, -CharacterZPosition);
}
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping volume to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix           // Reset
    glLoadIdentity();// Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(60.0f, aspect, 0.01f,200.0f);
}
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    CameraPitch += (ypos - cYPos)/100;
    cYPos = ypos;
    CameraYaw += (xpos - cXPos)/100;
    cXPos = xpos;
    if (CameraPitch <= -M_PI_2)
        CameraPitch = -M_PI_2;
    if (CameraPitch >= M_PI_2)
        CameraPitch = M_PI_2;
    if (CameraYaw >= M_2PI)
        CameraYaw = (CameraYaw - M_2PI);
    if(CameraYaw < 0)
        CameraYaw = (CameraYaw + M_2PI);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    pXSpeed = cos(CameraYaw) * speed * (GLFW_RELEASE != glfwGetKey(window, GLFW_KEY_W)) + cos(CameraYaw + M_PI_2 )* speed * (GLFW_RELEASE != glfwGetKey(window, GLFW_KEY_D)) + cos(CameraYaw + M_PI) * speed * (GLFW_RELEASE != glfwGetKey(window, GLFW_KEY_S)) + cos(CameraYaw - M_PI_2) * speed * (GLFW_RELEASE != glfwGetKey(window, GLFW_KEY_A));
    pZSpeed = sin(CameraYaw) * speed * (GLFW_RELEASE != glfwGetKey(window, GLFW_KEY_W)) +sin(CameraYaw + M_PI_2)* speed * (GLFW_RELEASE != glfwGetKey(window, GLFW_KEY_D)) + sin(CameraYaw + M_PI) * speed * (GLFW_RELEASE != glfwGetKey(window, GLFW_KEY_S)) + sin(CameraYaw - M_PI_2) * speed * (GLFW_RELEASE != glfwGetKey(window, GLFW_KEY_A));
    pYSpeed =  speed * (GLFW_RELEASE != glfwGetKey(window, GLFW_KEY_SPACE)) + -speed * (GLFW_RELEASE != glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || !GLFW_RELEASE == glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT));
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) ==GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)!=GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) != GLFW_RELEASE) && glfwGetKey(window, GLFW_KEY_EQUAL) != GLFW_RELEASE)
        speed++;
    if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) != GLFW_RELEASE) && glfwGetKey(window, GLFW_KEY_MINUS) != GLFW_RELEASE)
        speed--;
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (rayCastBlockX <= 7 && rayCastBlockY <= 7 && rayCastBlockZ <= 7 && rayCastBlockX >= 0 && rayCastBlockY >= 0 && rayCastBlockZ >= 0)
        {
            chunk[(rayCastBlockZ + rayCastBlockY * 8 + rayCastBlockX * 64)] = 0;
        }
    }
}
int main(void)
{
    GLFWwindow* window;
    if (!glfwInit())
        return -1;
    window = glfwCreateWindow(windowLength, windowHeight, "Infinitia 0.01", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowPos(window, 0, 0);
    initGL();
    generateChunk();
    while (!glfwWindowShouldClose(window))
    {
        raycast(CharacterXPosition, CharacterYPosition, CharacterZPosition, CameraPitch, CameraYaw, rayCastBlockX, rayCastBlockY, rayCastBlockZ);
        glClear(GL_COLOR_BUFFER_BIT);
        reshape(windowLength, windowHeight);
        character();
        renderChunk();
        renderTargetBlock();
        glfwSwapBuffers(window);
        glfwWaitEventsTimeout(fRate);
        /*Create a game loop on another thread*/
        glfwSetKeyCallback(window, key_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetScrollCallback(window, scroll_callback);
        system("CLS");
        CharacterXPosition = CharacterXPosition + pXSpeed * fRate;
        CharacterYPosition = CharacterYPosition + pYSpeed * fRate;
        CharacterZPosition = CharacterZPosition + pZSpeed * fRate;
        printf("X:%f Y:%f Z:%f \nYaw %f \nPitch %f \nSpeed:%d\nZoom: %f\nraycastblock%d %d %d", CharacterXPosition, CharacterYPosition, CharacterZPosition, CameraYaw, CameraPitch,speed, cRadius*10,rayCastBlockX, rayCastBlockY, rayCastBlockZ);
    }
    glfwTerminate();
    return 0;
}
