#include <iostream>
#include <iomanip>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <vector>
#include <string>
#include "Vertex.h"
#include "ShaderHelper.h"

float f(float x) {
    return sin(x);
}

float df(float x, float h) {
    return (f(x + h) - f(x)) / h; // Newton's quotient
}

int main(int argc, char** argv) {
    bool renderSpiral = false;
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "-spiral") renderSpiral = true;
    }

    //      GLFW CONFIGURE       //
    GLFWwindow* window;
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(480, 480, "Compulsory 1", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to get process address\n";
        glfwTerminate();
        return -1;
    }

    //      FILE & VERTEX SETUP      //
    std::ofstream outFile("vertices.txt");
    std::ifstream inFile("vertices.txt");
    std::vector<Vertex2D> vertices;

    // Mathematical parameters - Task 1
    float a = -10.0f, b = 10.0f; // should be symmetrical
    unsigned int n = 100;
    float h = (b - a) / n;
    const unsigned int precision = 4; // Float precision of function values
    // Graphical parameters - Task 1
    float graphXScalar = 0.1f; // 1.0f / b
    float graphYScalar = 0.5f;
    float graphWidth = 4.0f;
    float graphColorIntensity = 5.0f;

    // First line contains vertex amount
    outFile << "Vertex amount: " << n << std::endl;
    // Find maximum slope for normalization
    float maxSlope = 0.0f;
    for (unsigned int i = 0; i <= n; i++) {
        float x = a + i * h;
        float slope = fabs(df(x, h));
        if (slope > maxSlope)
            maxSlope = slope;
    }   if (maxSlope == 0.0f) maxSlope = std::numeric_limits<float>::min(); // Avoid division by zero just in case
    // Proceed with main calculation and writing
    for (unsigned int i = 0; i <= n; i++) {
        float x = a + i * h;
        float y = f(x);
        float slope = df(x, h);

        // Normalize slope value between 0 and 1
        float normalizedSlope = fabs(slope) / maxSlope;

        // Color based on change in progressing x value
        float r = slope < 0 ? normalizedSlope : 0.0f; // Red = decrease in y
        float g = slope > 0 ? normalizedSlope : 0.0f; // Green = increase in y
        float b = 0.0f; // Unused

        outFile << std::fixed << std::setprecision(precision) << x << " " << y << " " << r << " " << g << " " << b << std::endl;
    }
    outFile.close();

    // Read vertex data from file
    inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Skip first line
    for (unsigned int i = 0; i <= n; ++i) {
        float x, y, r, g, b;
        inFile >> x >> y >> r >> g >> b;
        vertices.push_back(Vertex2D(x * graphXScalar, y * graphYScalar, std::fmin(r * graphColorIntensity, 1.0), std::fmin(g * graphColorIntensity, 1.0), b));
    }
    inFile.close();

    std::vector<Vertex2D> originalVertices = vertices;

    // Spiral parameters
    const float radius = 0.5f;
    const float heightIncrement = 0.1f;
    const unsigned int spiralPoints = 500;
    const float tStart = 0.0f;
    const float tEnd = 10.0f * 3.14159f * 2; // 10 turns
    const float tStep = (tEnd - tStart) / spiralPoints;
    const unsigned int spiralPrecision = 4; // Float precision of function values
    const float graphicalYOffset = -1.0f;
    float spiralWidth = 6.0f;


    std::ofstream spiralOutFile("spiral_vertices.txt");
    std::ifstream spiralInFile("spiral_vertices.txt");
    std::vector<Vertex3D> spiralVertices;

    spiralOutFile << "Vertex amount: " << spiralPoints << std::endl;
    // Main calculation and output
    for (unsigned int i = 0; i < spiralPoints; ++i) {
        float t = tStart + i * tStep;
        float x = radius * cos(t);
        float z = radius * sin(t);
        float y = heightIncrement * t; // y remains upwards

        // Alternating colors
        float r = (i % 2 == 0) ? 1.0f : 0.0f;
        float g = (i % 2 != 0) ? 1.0f : 0.0f;
        float b = 0.0f; // Unused

        spiralOutFile << std::fixed << std::setprecision(spiralPrecision) << x << " " << y << " " << z << " " << r << " " << g << " " << b << std::endl;
    }
    spiralOutFile.close();

    // Read vertex data from file
    spiralInFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Skip first line
    for (unsigned int i = 0; i <= spiralPoints; ++i) {
        float x, y, z, r, g, b;
        spiralInFile >> x >> y >> z >> r >> g >> b;
        spiralVertices.push_back(Vertex3D(x, y + graphicalYOffset, z, r, g, b));
    }
    spiralInFile.close();

    //      SHADER SETUP        //
    // Load shader source -- reads from current folder
    std::string vertexShaderSourceString2D = std::string(ShaderHelper::readShader(TYPE_VERTEX, DIMENSION_2D));
    std::string vertexShaderSourceString3D = std::string(ShaderHelper::readShader(TYPE_VERTEX, DIMENSION_3D));
    std::string fragmentShaderSourceString = std::string(ShaderHelper::readShader(TYPE_FRAGMENT));

    const char* vertexShaderSource2D = vertexShaderSourceString2D.c_str();
    const char* vertexShaderSource3D = vertexShaderSourceString3D.c_str();
    const char* fragmentShaderSource = fragmentShaderSourceString.c_str();

    // Create both shader programs with their compiled shaders then prepare objects
    GLuint shaderProgram = ShaderHelper::createProgram(vertexShaderSource2D, fragmentShaderSource);
    GLuint VAO, VBO;
    ShaderHelper::setupObjects(VAO, VBO, vertices, 2);

    GLuint spiralShaderProgram = ShaderHelper::createProgram(vertexShaderSource3D, fragmentShaderSource);
    GLuint VAO2, VBO2;
    ShaderHelper::setupObjects(VAO2, VBO2, spiralVertices, 3);

    GLuint rotationLocation = glGetUniformLocation(spiralShaderProgram, "rotationAngle");

    //      RENDER LOOP     //
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        float factor = static_cast<float>(1 - (ypos / 480)); // y pos -> color intensity or spiral rotation

        if (renderSpiral) {
            glUseProgram(spiralShaderProgram);              // Use the appropriate shader program
            glUniform1f(rotationLocation, factor);          // Update rotation uniform
            glLineWidth(spiralWidth);                        // Set graph width

            // Render Spiral
            glBindVertexArray(VAO2);
            glDrawArrays(GL_LINE_STRIP, 0, spiralVertices.size());
        }
        else {
            // Update VBO with new vertex data for the graph
            for (unsigned int i = 0; i < vertices.size(); i++) {
                vertices[i].r = fmax(originalVertices[i].r * factor, 0.0f); // Scale original red color
                vertices[i].g = fmax(originalVertices[i].g * factor, 0.0f); // Scale original green color
            }
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2D), vertices.data(), GL_DYNAMIC_DRAW);

            // Render Graph
            glUseProgram(shaderProgram);                     // Use the appropriate shader program
            glBindVertexArray(VAO);                          // Bind VAO for the graph
            glLineWidth(graphWidth);                         // Set graph width
            glDrawArrays(GL_LINE_STRIP, 0, vertices.size()); // Render the graph
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Deallocate resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteProgram(spiralShaderProgram);

    glfwTerminate();
    return 0;
}