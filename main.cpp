//
//  main.cpp
//  3D Object Drawing
//

//

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath> // For sin and cos
#include "shader.h"
#include "camera.h"
#include "basic_camera.h"

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
glm::mat4 RotationMatricesX(float theta);
glm::mat4 RotationMatricesY(float theta);
glm::mat4 RotationMatricesZ(float theta);


// settings
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 1000;

// modelling transform
float rotateAngle_X = -0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = -1.5;
float translate_Y = -2.0;
float translate_Z = 0.0;
float scale_X = 3.0;
float scale_Y = .5;
float scale_Z = 1.0;
float r = 0.0;
float r_fan = 0.0;

bool fanOn = false;
bool birdEyeView = false;

glm::vec3 cam = glm::vec3(-2.0f, 0.5f, -.5f);

glm::vec3 eye = glm::vec3(0.0f, 5.0f, 10.0f); // Camera's initial position in the world
glm::vec3 lookAtPoint = glm::vec3(0.0f, 0.0f, 0.0f); // The point the camera is looking at (origin)
glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f); // The 'up' direction for the camera

// camera
Camera camera(cam);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 2.0, eyeZ = -10.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);

glm::mat4 customLookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
    // Calculate the direction vectors
    glm::vec3 zAxis = glm::normalize(eye - center); // Forward vector
    glm::vec3 xAxis = glm::normalize(glm::cross(glm::normalize(up), zAxis)); // Right vector
    glm::vec3 yAxis = glm::cross(zAxis, xAxis); // Up vector

    // Create a 4x4 view matrix
    glm::mat4 view(1.0f);
    view[0][0] = xAxis.x;
    view[1][0] = xAxis.y;
    view[2][0] = xAxis.z;

    view[0][1] = yAxis.x;
    view[1][1] = yAxis.y;
    view[2][1] = yAxis.z;

    view[0][2] = zAxis.x;
    view[1][2] = zAxis.y;
    view[2][2] = zAxis.z;

    view[3][0] = -glm::dot(xAxis, eye);
    view[3][1] = -glm::dot(yAxis, eye);
    view[3][2] = -glm::dot(zAxis, eye);

    return view;
}


// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;
glm::mat4 rotate2 = glm::mat4(1.0f);

float radius = 10.0f; // Distance from the camera to the look-at point
float angle = 0.0f;

glm::mat4 rotateCameraAroundPoint()
{
    // Calculate the new camera position
    float cameraX = lookAtPoint.x + radius * cos(glm::radians(angle));
    float cameraZ = lookAtPoint.z + radius * sin(glm::radians(angle));
    glm::vec3 cameraPosition = glm::vec3(cameraX, 0.0f, cameraZ); // New position

    // Use a custom LookAt function to create the view matrix
    return customLookAt(cameraPosition, lookAtPoint, glm::vec3(0.0f, 1.0f, 0.0f));
}

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
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

        //Uper Plane
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

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //ourShader.use();

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


        // activate shader
        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);


        glm::mat4 view;

        // Bird's-eye view logic
        if (birdEyeView) {
            glm::vec3 birdEyePosition(0.0f, 10.0f, 0.0f); // Camera above the scene
            glm::vec3 birdEyeTarget(0.0f, 0.0f, 0.0f);    // Focus on origin
            glm::vec3 upVector(0.0f, 0.0f, 1.0f);        // Adjust the up vector
            view = customLookAt(birdEyePosition, birdEyeTarget, upVector);
        }
        // Rotate around a point logic
        else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            view = rotateCameraAroundPoint(); // Rotate around the look-at point
        }
        else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            view = customLookAt(eye, lookAtPoint, upVector);
        }
        else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
            view = customLookAt(eye, lookAtPoint, upVector);
        }
        else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            view = customLookAt(eye, lookAtPoint, upVector);
        }
        else {
            view = basic_camera.createViewMatrix();
        }

        // Set the single view matrix in the shader
        ourShader.setMat4("view", view);
        glm::mat4 identityMatrix = glm::mat4(1.0f);
        glm::mat4 globalTranslationMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;

        // Table up
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, 1.0f, 0.0f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(10.0f, 0.2f, 3.0f));
        model = globalTranslationMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.76, 0.56, 0.36, 1.0));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, 1.0f, 0.0f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(3.0f, 0.2f, 6.5f));
        model = globalTranslationMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.76, 0.56, 0.36, 1.0));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, 1.0f, 0.0f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(3.0f, 0.2f, 6.5f));
        model = globalTranslationMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.76, 0.56, 0.36, 1.0));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        // Table leg - 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.4f, 0.0f, 1.4f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.2f, 2.0f, 0.2f));
        model = globalTranslationMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.55, 0.40, 0.25, 1.0));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Table leg - 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.4f, 0.0f, 0.0f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.2f, 2.0f, 0.2f));
        model = globalTranslationMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.55, 0.40, 0.25, 1.0));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Table leg - 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, 0.0f, 1.4f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.2f, 2.0f, 0.2f));
        model = globalTranslationMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.55, 0.40, 0.25, 1.0));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Table leg - 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, 0.0f, 0.0f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.2f, 2.0f, 0.2f));
        model = globalTranslationMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.55, 0.40, 0.25, 1.0));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



        for (int i = 0; i < 4; ++i) {
            // Calculate the offset for each chair
            float offsetX = 2.5f + i * -1.2f; // 1.0 unit gap between each chair

            // Chair up
            translateMatrix = glm::translate(identityMatrix, glm::vec3(offsetX, 0.85f, -1.0f));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(1.5f, 1.45f, 0.2f));
            model = globalTranslationMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.75, 0.75, 0.75, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // Chair middle
            translateMatrix = glm::translate(identityMatrix, glm::vec3(offsetX, 0.75f, -1.0f));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(1.5f, 0.2f, 1.5f));
            model = globalTranslationMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.5, 0.5, 0.5, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // Chair leg middle
            translateMatrix = glm::translate(identityMatrix, glm::vec3(offsetX + 0.325f, 0.0f, -0.675f));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.2f, 1.5f, 0.2f));
            model = globalTranslationMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.2, 0.2, 0.2, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // Chair leg down 1
            translateMatrix = glm::translate(identityMatrix, glm::vec3(offsetX + 0.325f, 0.0f, -0.675f));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(1.2f, 0.1f, 0.2f));
            model = globalTranslationMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.1, 0.1, 0.1, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // Chair leg down 2
            translateMatrix = glm::translate(identityMatrix, glm::vec3(offsetX - 0.125f, 0.0f, -0.675f));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(1.2f, 0.1f, 0.2f));
            model = globalTranslationMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.1, 0.1, 0.1, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // Chair leg down 3
            translateMatrix = glm::translate(identityMatrix, glm::vec3(offsetX + 0.325f, 0.0f, -0.675f));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.2f, 0.1f, 1.2f));
            model = globalTranslationMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.1, 0.1, 0.1, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // Chair leg down 4
            translateMatrix = glm::translate(identityMatrix, glm::vec3(offsetX + 0.325f, 0.0f, -1.25f));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.2f, 0.1f, 1.2f));
            model = globalTranslationMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.1, 0.1, 0.1, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }



        for (int i = 0; i < 3; ++i) {
            // Calculate the offset for each chair along the Z-axis
            float offsetZ = -0.25f + i * 1.3f; // 1.2 unit gap between each chair along Z-axis

            // Chair up
            translateMatrix = glm::translate(identityMatrix, glm::vec3(4.25f, 0.85f, offsetZ));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.2f, 1.45f, 1.5f));
            model = globalTranslationMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.75, 0.75, 0.75, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // Chair middle
            translateMatrix = glm::translate(identityMatrix, glm::vec3(3.60f, 0.75f, offsetZ));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(1.5f, 0.2f, 1.5f));
            model = globalTranslationMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.5, 0.5, 0.5, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // Chair leg middle
            translateMatrix = glm::translate(identityMatrix, glm::vec3(3.92f, 0.0f, offsetZ ));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.2f, 1.5f, 0.2f));
            model = globalTranslationMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.2, 0.2, 0.2, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // Chair leg down 1
            translateMatrix = glm::translate(identityMatrix, glm::vec3(3.6f + 0.325f, 0.0f, offsetZ));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(1.2f, 0.1f, 0.2f));
            model = globalTranslationMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.1, 0.1, 0.1, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // Chair leg down 2
            translateMatrix = glm::translate(identityMatrix, glm::vec3(3.6f - 0.125f, 0.0f, offsetZ));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(1.2f, 0.1f, 0.2f));
            model = globalTranslationMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.1, 0.1, 0.1, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // Chair leg down 3
            translateMatrix = glm::translate(identityMatrix, glm::vec3(3.6f + 0.325f, 0.0f, offsetZ));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.2f, 0.1f, 1.2f));
            model = globalTranslationMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.1, 0.1, 0.1, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // Chair leg down 4
            translateMatrix = glm::translate(identityMatrix, glm::vec3(3.6f + 0.325f, 0.0f, offsetZ-0.6));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.2f, 0.1f, 1.2f));
            model = globalTranslationMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.1, 0.1, 0.1, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }







        // Floor
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.5f, 0.0f, 3.4f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(16.5f, -0.1f, -12.3f));
        model = globalTranslationMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.6, 0.8, 0.4, 1.0));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Wall-1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.7f, 0.0f, 3.4f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.1f, 8.0f, -12.3f));
        model = globalTranslationMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.68, 0.85, 1.0, 1.0));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Wall-2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.7f, 0.0f, 3.4f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(-16.5f, 8.0f, 0.1f));
        model = globalTranslationMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.55, 0.70, 0.90, 1.0));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        for (int i = 0; i < 15; ++i) {
            // Calculate the offset for each Porda
            float offsetX = 5.5f - i * 0.20f; // 0.75 unit gap between each Porda along X-axis

            // Porda-design
            translateMatrix = glm::translate(identityMatrix, glm::vec3(offsetX, 0.0f, 3.35f));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(-0.15f, 8.0f, 0.1f));
            model = globalTranslationMatrix * scaleMatrix;
            ourShader.setMat4("model", model);

            // Purple color for Porda
            ourShader.setVec4("color", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f)); // Standard purple
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.3f, 1.5f, 3.35f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(7.0f, 4.0f, 0.1f));
        model = globalTranslationMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Less dark ash color
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.2f, 1.6f, 3.34f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(6.6f, 3.6f, 0.1f));
        model = globalTranslationMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)); // Ash color
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        // Wall-3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.5f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.1f, 8.0f, -7.5f));
        model = globalTranslationMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.68, 0.85, 1.0, 1.0));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // door
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.5f, 0.0f, 3.4f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.1f, 8.0f, -5.0f));
        model = globalTranslationMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.54f, 0.27f, 0.07f, 1.0f)); // Medium brown
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        // Ceiling-Fan
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1, 4.0f, 1.0f));
        scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.30f, -1.66f, -0.30f));
        model = globalTranslationMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.2, 0.2, 0.2, 1));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        if (fanOn) {
            // Blade-1
            translateMatrix = glm::translate(identityMatrix, glm::vec3(1, 3.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, 0.3f, 0.3f));
            rotateYMatrix = RotationMatricesY(glm::radians(r_fan));
            model = globalTranslationMatrix * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.0f, 0.0, 0.0f)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-0.5, 0.0, 0.0f)) * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.0, 0.0, 0.5, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


            translateMatrix = glm::translate(identityMatrix, glm::vec3(1, 3.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, 0.3f, 0.3f));
            rotateYMatrix = RotationMatricesY(glm::radians(r_fan + 90));
            model = globalTranslationMatrix * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.0f, 0.0, 0.0f)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-0.5, 0.0, 0.0f)) * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.0, 0.0, 0.5, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            
            r_fan += 20;
        }


        else {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(1, 3.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, 0.3f, 0.3f));
            rotateYMatrix = RotationMatricesY(glm::radians(r_fan));
            model = globalTranslationMatrix * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.0f, 0.0, 0.0f)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-0.5, 0.0, 0.0f)) * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.0, 0.0, 0.5, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


            translateMatrix = glm::translate(identityMatrix, glm::vec3(1, 3.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, 0.3f, 0.3f));
            rotateYMatrix = RotationMatricesY(glm::radians(r_fan + 90));
            model = globalTranslationMatrix * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.0f, 0.0, 0.0f)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-0.5, 0.0, 0.0f)) * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.0, 0.0, 0.5, 1.0));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }


        //cylinder-1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.5f, 0.0f, -1.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 2.0f, 0.1f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
        model = globalTranslationMatrix * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5f, 0.f, 0.0f)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-0.5, 0.0, 0.0f)) * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.0f, 0.5f, 0.0f, 1.0f)); // Deep green color
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        int x = 1;
        for (int i = 1; i < 360; i++)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(4.5f, 0.0f, -1.8f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 2.0f, 0.1f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r - i), glm::vec3(0.0f, 1.0f, 0.0f));
            model = globalTranslationMatrix * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5f, 0.f, 0.0f)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-0.5, 0.0, 0.0f)) * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.0f, 0.5f, 0.0f, 1.0f)); // Deep green color
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        //cylinder-2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.2f, 0.0f, -1.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 2.0f, 0.1f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
        model = globalTranslationMatrix * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5f, 0.f, 0.0f)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-0.5, 0.0, 0.0f)) * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.0f, 0.5f, 0.0f, 1.0f)); // Deep green color
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        int y = 1;
        for (int i = 1; i < 360; i++)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.2f, 0.0f, -1.8f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 2.0f, 0.1f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r - i), glm::vec3(0.0f, 1.0f, 0.0f));
            model = globalTranslationMatrix * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5f, 0.f, 0.0f)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-0.5, 0.0, 0.0f)) * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(0.0f, 0.5f, 0.0f, 1.0f)); // Deep green color
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }




        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

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

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) translate_Y += 0.1;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) translate_Y -= 0.1;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) translate_X += 0.1;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) translate_X -= 0.1;
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) translate_Z += 0.1;
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) translate_Z -= 0.1;

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        fanOn = true;
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        fanOn = false;
    }


    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        glm::mat4 rotation = RotationMatricesX(1.0f * deltaTime);
        eye = glm::vec3(rotation * glm::vec4(eye, 1.0f)); // Rotate around X-axis
    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        glm::mat4 rotation = RotationMatricesY(1.0f * deltaTime);
        eye = glm::vec3(rotation * glm::vec4(eye, 1.0f)); // Rotate around Y-axis
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        glm::mat4 rotation = RotationMatricesZ(1.0f * deltaTime);
        eye = glm::vec3(rotation * glm::vec4(eye, 1.0f)); // Rotate around Z-axis
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        birdEyeView = true;
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        birdEyeView = false;
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        angle += 50.0f * deltaTime; // Increment angle (speed adjusted by deltaTime)
        if (angle >= 360.0f) angle -= 360.0f; // Keep angle within [0, 360] range
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        eyeX += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        eyeY += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        eyeZ += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
glm::mat4 RotationMatricesX(float theta) {
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);
    return glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cosTheta, -sinTheta, 0.0f,
        0.0f, sinTheta, cosTheta, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

glm::mat4 RotationMatricesY(float theta) {
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);
    return glm::mat4(
        cosTheta, 0.0f, sinTheta, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        -sinTheta, 0.0f, cosTheta, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

glm::mat4 RotationMatricesZ(float theta) {
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);
    return glm::mat4(
        cosTheta, -sinTheta, 0.0f, 0.0f,
        sinTheta, cosTheta, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}