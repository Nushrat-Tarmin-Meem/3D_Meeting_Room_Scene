//
//  main.cpp
//  3D Object Drawing
//
//  Created by Nazirul Hasan on 4/9/23.
//  modified by Badiuzzaman on 3/11/24.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "basic_camera.h"

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
bool fanon = false;
// draw object functions
void drawCube(Shader shaderProgram, unsigned int VAO, glm::mat4 parentTrans, float posX = 0.0, float posY = 0.0, float posz = 0.0, float rotX = 0.0, float rotY = 0.0, float rotZ = 0.0, float scX = 1.0, float scY = 1.0, float scZ = 1.0, float R = .5921f, float G = .1922f, float B = 0.0f);
void drawCubenotrans(Shader shaderProgram, unsigned int VAO, glm::mat4 parentTrans, float posX = 0.0, float posY = 0.0, float posz = 0.0, float rotX = 0.0, float rotY = 0.0, float rotZ = 0.0, float scX = 1.0, float scY = 1.0, float scZ = 1.0);
void drawcubechair(Shader shaderProgram, unsigned int VAO, glm::mat4 parentTrans, float posX = 0.0, float posY = 0.0, float posz = 0.0, float rotX = 0.0, float rotY = 0.0, float rotZ = 0.0, float scX = 1.0, float scY = 1.0, float scZ = 1.0);
void chair(Shader ourShader, unsigned int VAO, glm::mat4 identityMatrix, float scaleX, float scaleY, float scaleZ, float translateX, float translateY, float translateZ, float angle, float axisX, float axisY, float axisZ);
void coffeetable(Shader ourShader, unsigned int VAO, glm::mat4 identityMatrix);
void drawSofa(Shader  ourShader, glm::mat4 identityMatrix, unsigned int VAO, glm::mat4 Model_Modify);
void drawFan(Shader ourShader, glm::mat4 identityMatrix, unsigned int VAO, unsigned int VAO2, glm::mat4 Model_Center);



// Function to calculate the rotation matrix
glm::mat4 myRotate(const glm::mat4& baseMatrix, float angle, const glm::vec3& axis) {
    glm::vec3 normalizedAxis = glm::normalize(axis);

    float x = normalizedAxis.x;
    float y = normalizedAxis.y;
    float z = normalizedAxis.z;

    float cosTheta = cos(angle);
    float sinTheta = sin(angle);
    float oneMinusCosTheta = 1.0f - cosTheta;

    glm::mat4 rotationMatrix(1.0f);

    rotationMatrix[0][0] = cosTheta + x * x * oneMinusCosTheta;
    rotationMatrix[0][1] = x * y * oneMinusCosTheta - z * sinTheta;
    rotationMatrix[0][2] = x * z * oneMinusCosTheta + y * sinTheta;

    rotationMatrix[1][0] = y * x * oneMinusCosTheta + z * sinTheta;
    rotationMatrix[1][1] = cosTheta + y * y * oneMinusCosTheta;
    rotationMatrix[1][2] = y * z * oneMinusCosTheta - x * sinTheta;

    rotationMatrix[2][0] = z * x * oneMinusCosTheta - y * sinTheta;
    rotationMatrix[2][1] = z * y * oneMinusCosTheta + x * sinTheta;
    rotationMatrix[2][2] = cosTheta + z * z * oneMinusCosTheta;

    // Multiply the base matrix by the calculated rotation matrix
    return baseMatrix * rotationMatrix;
}


// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

// modelling transform
float rotateAngle_X = 45.0;
float rotateAngle_Y = 45.0;
float rotateAngle_Z = 45.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 0.0, eyeZ = 3.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _APPLE_
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 4208: Computer Graphics Laboratory", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    Shader constantShader("vertexShader.vs", "fragmentShaderV2.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cube_vertices[] = {
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,

        0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f
    };
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };
    float cylinder_vertix[] = {
    1,0.5,0,
    1,0,0,
    0.913,0.5,0.406,
    0.913,0,0.406,
    0.669,0.5,0.743,
    0.669,0,0.743,
    0.309,0.5,0.951,
    0.309,0,0.951,
    -0.104,0.5,0.994,
    -0.104,0,0.994,
    -0.5,0.5,0.866,
    -0.5,0,0.866,
    -0.809,0.5,0.588,
    -0.809,0,0.588,
    -0.978,0.5,0.208,
    -0.978,0,0.208,
    -0.978,0.5,-0.208,
    -0.978,0,-0.208,
    -0.809,0.5,-0.588,
    -0.809,0,-0.588,
    -0.5,0.5,-0.866,
    -0.5,0,-0.866,
    -0.104,0.5,-0.994,
    -0.104,0,-0.994,
    0.309,0.5,-0.951,
    0.309,0,-0.951,
    0.669,0.5,-0.743,
    0.669,0,-0.743,
    0.913,0.5,-0.406,
    0.913,0,-0.406,
    };

    unsigned int cylinder_indices[] = {
        // Top cap
        1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,

        // Bottom cap
        0,28,26,24,22,20,18,16,14,12,10,8,6,4,2,

        //side bar
         0, 2, 1,   2, 3, 1,
         2, 4, 3,   4, 5, 3,
         4, 6, 5,   6, 7, 5,
         6, 8, 7,   8, 9, 7,
         8, 10, 9,  10, 11, 9,
         10, 12, 11, 12, 13, 11,
         12, 14, 13, 14, 15, 13,
         14, 16, 15, 16, 17, 15,
         16, 18, 17, 18, 19, 17,
         18, 20, 19, 20, 21, 19,
         20, 22, 21, 22, 23, 21,
         22, 24, 23, 24, 25, 23,
         24, 26, 25, 26, 27, 25,
         26, 28, 27, 28, 29, 27,
         28, 0, 29,
         0, 1, 29
    };


    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    unsigned int VBO2, VAO2, EBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glGenBuffers(1, &EBO2);

    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cylinder_vertix), cylinder_vertix, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cylinder_indices), cylinder_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    float speed = 0.0;
    //constantShader.use();

    // render loop


    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    ourShader.use();
    //constantShader.use();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(basic_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);
        //constantShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = basic_camera.createViewMatrix();
        ourShader.setMat4("view", view);
        //constantShader.setMat4("view", view);

        // Modelling Transformation
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

        //table
        coffeetable(ourShader, VAO, identityMatrix); //scaling of 1.5f 1:2
        //scaleX, scaleY, scaleZ, transX, transY, transZ, angle, axisX, axisY, axisZ
        //chair
        chair(ourShader, VAO, identityMatrix, .5f, 1.0f, 1.0f, 0.05f, -0.5 - .25f, -.014f - .0625f, 90.0f, 0.0f, 0.0f, 1.0f);
        chair(ourShader, VAO, identityMatrix, .5f, 2.0f, 1.0f, 0.0f, .5 + .25f, -.014f - .0625f, -90.0f, 0.0f, 0.0f, 1.0f);
        ////TV
        drawCube(ourShader, VAO, identityMatrix, 1.8f, translate_Y, 0.25f + 0.0725f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.04, 2.5, 2, 0.259, 0.235, 0.235);
        drawCube(ourShader, VAO, identityMatrix, 1.85f, translate_Y, 0.27f + 0.066f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.002, 2.4, 1.9, 0.012, 0.012, 0.012);
        ////TV Desk
        drawCube(ourShader, VAO, identityMatrix, .8f, translate_Y, 0.0625f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, 3, 1, 0.961, 0.957, 0.651);
        ////Desk Sticks
        drawCube(ourShader, VAO, identityMatrix, 1.3f, 0.25, 0.0625f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .0125, .0125, 1, 0.961, 0.953, 0.561);
        drawCube(ourShader, VAO, identityMatrix, 1.3f, -0.25, 0.0625f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .0125, .0125, 1, 0.961, 0.953, 0.561);
        //
        ////Sofa
        glm::mat4 modelCenter;
        modelCenter = glm::scale(identityMatrix, glm::vec3(0.7, 0.7, 1));

        modelCenter = identityMatrix;

        drawSofa(ourShader, identityMatrix, VAO, modelCenter);

        //Floor
        drawCube(ourShader, VAO, identityMatrix, 0.0f, 0.0f, -.11f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 8, 8, .1, 0.612f, 0.71f, 0.69f);
        ////Right wall
        drawCube(ourShader, VAO, identityMatrix, 1.8f, 0.0f, 0.18f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .1, 8, 4, 0.835, 0.835, 0.835);
        ////left wall
        drawCube(ourShader, VAO, identityMatrix, -1.8f, 0.0f, 0.18f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .1, 8, 4, 0.835, 0.835, 0.835);
        ////back wall
        drawCube(ourShader, VAO, identityMatrix, 0.0f, 1.8f, 0.18f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 8, .1, 4, 0.941, 0.937, 0.863);
        ////front wall
        //drawCube(ourShader, VAO, identityMatrix, 0.0f, -1.8f, 0.18f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 8, .1, 4, 0.863, 0.863, 0.863);
        //upperwall
        drawCube(ourShader, VAO, identityMatrix, 0.0f, 0.0f, 1.7f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 8, 8, .1, 0.612f, 0.71f, 0.69f);


        ////bookshelf
        drawCube(ourShader, VAO, identityMatrix, 0.0f, 1.0f, 1.0f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2, .5, .01, 0.31f, 0.439f, 0.424f);
        drawCube(ourShader, VAO, identityMatrix, 0.0f, 1.0f, 1.5f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2, .5, .01, 0.31f, 0.439f, 0.424f);
        //useless /drawCube(ourShader, VAO, identityMatrix, -1.7f, 1.4f, .16f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .0125, .0125, .01, 0.961, 0.957, 0.651);/

         //almari
        drawCube(ourShader, VAO, identityMatrix, -1.0f + .15f, 1.0f - .15f, .16f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, 1, 3, 0.365f, 0.741f, 0.694f);
        drawCube(ourShader, VAO, identityMatrix, -1.425f + .125f, 1.2f, .5f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.25, 0.25, 0.25, 0.0f, 0.0f, 0.0f);
        drawCube(ourShader, VAO, identityMatrix, -1.425f, 1.2f, .5f, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.25, 0.25, 0.25, 0.0f, 0.0f, 0.0f);
        drawCube(ourShader, VAO, identityMatrix, -1.625f - .0525f, 1.4f, .3, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .0125, 0.0125, 1, 0.0f, 0.0f, 0.0f);


        //fan
        / glm::mat4 rotationMatrix = glm::rotate(identityMatrix, glm::radians(speed), glm::vec3(0.0f, 0.0f, 1.0f)); /
            glm::mat4 rotationMatrix = myRotate(identityMatrix, glm::radians(speed), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 translationMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -0.5f));
        modelCenter = translationMatrix * rotationMatrix;
        drawFan(ourShader, identityMatrix, VAO, VAO2, modelCenter);
        if (fanon)
        {
            speed = speed + 1.0f;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) translate_Y += 0.01;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) translate_Y -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) translate_X += 0.01;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) translate_X -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) translate_Z += 0.01;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) translate_Z -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) scale_X += 0.01;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) scale_X -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) scale_Y += 0.01;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) scale_Y -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) scale_Z += 0.01;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) scale_Z -= 0.01;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        rotateAngle_X += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        rotateAngle_Y += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        rotateAngle_Z += 1;
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        eyeX += 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        eyeX -= 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        eyeZ += 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        eyeZ -= 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        eyeY += 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        eyeY -= 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        lookAtX += 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        lookAtX -= 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        lookAtY += 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        lookAtY -= 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {
        if (fanon)
            fanon = false;
        else
            fanon = true;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    basic_camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    basic_camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void drawCube(Shader shaderProgram, unsigned int VAO, glm::mat4 parentTrans, float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scX, float scY, float scZ, float R, float G, float B)
{
    shaderProgram.use();

    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, modelCentered, modelg;
    translateMatrix = glm::translate(parentTrans, glm::vec3(posX, posY, posZ));
    model = glm::scale(translateMatrix, glm::vec3(scX, scY, scZ));
    modelCentered = glm::translate(model, glm::vec3(-0.25, -0.25, -0.25));
    modelCentered = glm::translate(modelCentered, glm::vec3(posX, posY, posZ));
    shaderProgram.setMat4("model", modelCentered);
    shaderProgram.setVec4("color", glm::vec4(R, G, B, 1.0f));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
void drawCubenotrans(Shader shaderProgram, unsigned int VAO, glm::mat4 parentTrans, float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scX, float scY, float scZ)
{
    shaderProgram.use();

    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, modelCentered;
    translateMatrix = glm::translate(parentTrans, glm::vec3(posX, posY, posZ));
    model = glm::scale(translateMatrix, glm::vec3(scX, scY, scZ));
    modelCentered = glm::translate(model, glm::vec3(-0.25, -0.25, -0.25));

    shaderProgram.setMat4("model", modelCentered);
    shaderProgram.setVec4("color", glm::vec4(0.78f, 0.647f, 0.286f, 1.0f));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void drawcubechair(Shader shaderProgram, unsigned int VAO, glm::mat4 parentTrans, float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scX, float scY, float scZ)
{
    shaderProgram.use();

    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, modelCentered;
    translateMatrix = glm::translate(parentTrans, glm::vec3(posX, posY, posZ));
    model = glm::scale(translateMatrix, glm::vec3(scX, scY, scZ));
    modelCentered = glm::translate(model, glm::vec3(-0.25, -0.25, 0));

    shaderProgram.setMat4("model", modelCentered);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void chair(Shader ourShader, unsigned int VAO, glm::mat4 identityMatrix, float scaleX, float scaleY, float scaleZ, float translateX, float translateY, float translateZ, float angle, float axisX, float axisY, float axisZ) {

    // Create an identity matrix
    glm::mat4 translate_mat = glm::mat4(1.0f);

    // Apply the translation transformation
    translate_mat = glm::translate(translate_mat, glm::vec3(translateX, translateY, translateZ));

    // Create an identity matrix
    glm::mat4 scale_mat = glm::mat4(1.0f);

    float rotateAngle = glm::radians(angle - 180.0f); // Convert degrees to radians
    glm::mat4 rotate_mat = myRotate(glm::mat4(1.0f), rotateAngle, glm::vec3(axisX, axisY, axisZ));

    // Apply the scaling transformation
    scale_mat = glm::scale(scale_mat, glm::vec3(scaleX, scaleY, scaleZ));
    glm::mat4 model = translate_mat * rotate_mat * scale_mat * identityMatrix;

    //chair
    drawCube(ourShader, VAO, model, translate_X, translate_Y, .0625, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2, 1, .08);
    drawCubenotrans(ourShader, VAO, model, -0.5 + .0625, -0.25 + .0625, translate_Z, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .25, .25, .25);
    drawCubenotrans(ourShader, VAO, model, -0.5 + .0625, +0.25 - .0625, translate_Z, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .25, .25, .25);
    drawCubenotrans(ourShader, VAO, model, +0.5 - .0625, +0.25 - .0625, translate_Z, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .25, .25, .25);
    drawCubenotrans(ourShader, VAO, model, +0.5 - .0625, -0.25 + .0625, translate_Z, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .25, .25, .25);
    drawcubechair(ourShader, VAO, model, -.5 + .0125, translate_Y, 0.1 - .02, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .05, 1, 1);

}

void coffeetable(Shader ourShader, unsigned int VAO, glm::mat4 identityMatrix) {

    // Define the scaling factors
    float scaleX = 1.5f; // Scale along the X axis
    float scaleY = 1.5f; // Scale along the Y axis
    float scaleZ = 1.5f; // Scale along the Z axis

    //// Create an identity matrix
    glm::mat4 scale_mat = glm::mat4(1.0f);

    //// Apply the scaling transformation
    scale_mat = glm::scale(scale_mat, glm::vec3(scaleX, scaleY, scaleZ));
    glm::mat4 model = scale_mat * identityMatrix;

    drawCube(ourShader, VAO, model, translate_X, translate_Y, .0625, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2, 1, .08);
    drawCubenotrans(ourShader, VAO, model, -0.5 + .0625, -0.25 + .0625, translate_Z, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .25, .25, .25);
    drawCubenotrans(ourShader, VAO, model, -0.5 + .0625, +0.25 - .0625, translate_Z, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .25, .25, .25);
    drawCubenotrans(ourShader, VAO, model, +0.5 - .0625, +0.25 - .0625, translate_Z, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .25, .25, .25);
    drawCubenotrans(ourShader, VAO, model, +0.5 - .0625, -0.25 + .0625, translate_Z, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .25, .25, .25);


}

void drawSofa(Shader  ourShader, glm::mat4 identityMatrix, unsigned int VAO, glm::mat4 Model_Modify)
{
    glm::mat4 translateMatrix, rotationMatrix, scaleMatrix, modelCenter, model;

    //float rotateAngle = glm::radians(0.0f); // Convert degrees to radians

    float rotateAngle = glm::radians(0.0f); // Convert degrees to radians
    glm::mat4 rotate_mat = myRotate(glm::mat4(1.0f), rotateAngle, glm::vec3(0.0f, 0.0f, 1.0f));

    //sofa
    ourShader.use();

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2 - 1.2f, -0.5f, 0.0f - 0.0925f));

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.8, 2, .2));

    model = Model_Modify * rotate_mat * translateMatrix * scaleMatrix;

    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.929f, 0.929f, 0.875f, 1.0f));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    //sofa head
    ourShader.use();

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2 - 1.2f, -0.5f, 0.1f - 0.0925f));

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.3, 2, .5));

    model = Model_Modify * rotate_mat * translateMatrix * scaleMatrix;

    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.949f, 0.945f, 0.675f, 1.0f));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //sofa hand left side

    ourShader.use();

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2 - 1.2 + 0.15f, -0.5f, 0.1f - 0.0925f));

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.5, .3, .5));

    model = Model_Modify * rotate_mat * translateMatrix * scaleMatrix;

    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.761f, 0.757f, 0.639f, 1.0f));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    //sofa hand right side

    ourShader.use();

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2 - 1.2 + 0.15f, -0.5 + 1.0 - 0.15f, 0.1f - 0.0925f));

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.5, .3, .5));

    model = Model_Modify * rotate_mat * translateMatrix * scaleMatrix;

    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.761f, 0.757f, 0.639f, 1.0f));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}
void drawFan(Shader ourShader, glm::mat4 identityMatrix, unsigned int VAO, unsigned int VAO2, glm::mat4 Model_Center)
{
    glm::mat4 translateMatrix, rotationMatrix, scaleMatrix, model;

    // stike of fan

    ourShader.use();

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.025f, -0.025f, 1.625f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.1, .5f));
    model = translateMatrix * scaleMatrix;

    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.58f, 0.58f, 0.529f, 0.851f));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //fan

    ourShader.use();
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, 1.65));
    / rotationMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); /
        rotationMatrix = myRotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2, 0.2, 0.2));

    model = translateMatrix * rotationMatrix * scaleMatrix;

    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.49f, 0.467f, 0.443f, 1.0f));


    glBindVertexArray(VAO2);
    glDrawElements(GL_TRIANGLE_FAN, 15, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_TRIANGLE_FAN, 15, GL_UNSIGNED_INT, (const void*)(15 * sizeof(unsigned int)));
    glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_INT, (const void*)(30 * sizeof(unsigned int)));
    glDrawElements(GL_TRIANGLE_STRIP, 90, GL_UNSIGNED_INT, (const void*)(36 * sizeof(unsigned int)));
    //glDrawElements(GL_TRIANGLE_FAN, 30, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    // fan wing 1

    ourShader.use();

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.1f, 0.0f, 2.0625f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4, 1.2, 0.05));
    model = Model_Center * translateMatrix * scaleMatrix;

    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.4f, 0.4f, 0.365f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);



    // fan wing 2

    ourShader.use();

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.1f, 0.0f, 2.0625f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4, 1.2, 0.05));
    / rotationMatrix = glm::rotate(identityMatrix, glm::radians(120.0f), glm::vec3(0.0f, 0.0f, 1.0f)); /
        rotationMatrix = myRotate(identityMatrix, glm::radians(120.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = Model_Center * rotationMatrix * translateMatrix * scaleMatrix;

    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.4f, 0.4f, 0.365f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    // fan wing 3

    ourShader.use();

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.1f, 0.0f, 2.0625f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4, 1.2, 0.05));
    / rotationMatrix = glm::rotate(identityMatrix, glm::radians(240.0f), glm::vec3(0.0f, 0.0f, 1.0f)); /
        rotationMatrix = myRotate(identityMatrix, glm::radians(240.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = Model_Center * rotationMatrix * translateMatrix * scaleMatrix;

    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.4f, 0.4f, 0.365f, 1.0f));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}