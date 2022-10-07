//weird janky fix for windows
void* __chk_fail = 0;


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

//#include <table.h>

//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

#include "table.h"

uint32_t VBO;
uint32_t VAO;
uint32_t EBO;
unsigned int shaderProgram;


const char* GetGLErrorStr(GLenum err)
{
    switch (err)
    {
    case GL_NO_ERROR:          return "No error";
    case GL_INVALID_ENUM:      return "Invalid enum";
    case GL_INVALID_VALUE:     return "Invalid value";
    case GL_INVALID_OPERATION: return "Invalid operation";
    /*case GL_STACK_OVERFLOW:    return "Stack overflow";
    case GL_STACK_UNDERFLOW:   return "Stack underflow";*/
    case GL_OUT_OF_MEMORY:     return "Out of memory";
    default:                   return "Unknown error";
    }
}

float camxr = 0.0f;
float camyr = 0.0f;
const float camRotSpeed = 0.003f;
const float camSpeed = 1.0f;
glm::vec3 camPos = glm::vec3(0.0f,0.0f,0.0f);
glm::vec3 camDir = glm::vec3(0.0f,0.0f,1.0f);
glm::vec3 camUp = glm::vec3(0.0f,1.0f,0.0f);
double lmx = 0.0f;
double lmy = 0.0f;
char firstMouse = 1;

const int chunkWidth = 40;
const int chunkHeight = 30;
const int chunksWidth = 1;
const int chunksLength = 1;
const float cubeSize = 2.0f;

typedef struct Chunk {
    float data[chunkWidth][chunkHeight][chunkWidth];
} Chunk;






void mouse_callback(GLFWwindow* window, double dmposx, double dmposy) {
    if (firstMouse) {
        lmx = dmposx;
        lmy = dmposy;
        firstMouse = 0;
    }
    
    float mposx=(float)(dmposx-lmx)*camRotSpeed;
    float mposy=(float)(dmposy-lmy)*camRotSpeed;
    lmx = dmposx;    
    lmy = dmposy;

    camxr += mposx;
    if (camxr > M_PI) {camxr -= M_PI*2;}
    if (camxr < -M_PI) {camxr += M_PI*2;}

    camyr -= mposy;
    if (camyr > M_PI_2) {camyr=M_PI_2*0.99;}
    if (camyr < -M_PI_2) {camyr=-M_PI_2*.99;}
}

Chunk* chunks[chunksWidth][chunksLength];

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        //cast ray
        glm::vec3 a = camPos/cubeSize/2.0f;
        for (int i = 0; i < 50; i++) {
            a+=camDir/cubeSize;
            int b = floor(a.x);
            if (b > 0 && b < chunkWidth) {
                int c = floor(a.z);
                if (c > 0 && c < chunkWidth) {
                    int d = floor(a.y);
                    if (d > 0 && d < chunkHeight) {
                        chunks[0][0]->data[b][d][c]=0.5f;
                    }
                }
            }
        }
    }
}

char shouldClose = 0;

const float edgeTable[12][3] = {
    {0.0, -1.0, -1.0},{1.0, -1.0, 0.0},{0.0, -1.0, 1.0},{-1.0, -1.0, 0.0},
    {0.0, 1.0, -1.0},{1.0, 1.0, 0.0},{0.0, 1.0, 1.0},{-1.0, 1.0, 0.0},
    {-1.0, 0.0, -1.0},{1.0, 0.0, -1.0},{1.0, 0.0, 1.0},{-1.0, 0.0, 1.0}
};

/*const float edgeTable[12][3] = {
        {1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{-1.0, 0.0, 0.0},{0.0, -1.0, 0.0},
        {1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{-1.0, 0.0, 0.0},{0.0, -1.0, 0.0},
        {0.0, 0.0, 1.0},{0.0, 0.0, 1.0},{ 0.0, 0.0, 1.0},{0.0,  0.0, 1.0}
};*/








int main() {
    
    for (int i = 0; i < chunksWidth; i++) {
        for (int ii = 0; ii < chunksLength; ii++) {
            chunks[i][ii]=(Chunk*)malloc(sizeof(Chunk));
            
            /*for (int cx = 0; cx < chunkWidth; cx++) {
                for (int cy = 0; cy < chunkHeight; cy++) {
                    for (int cz = 0; cz < chunkWidth; cz++) {
                        chunks[i][ii]->data[cx][0][cz]=0.5;
                    }
                }
            }*/
        }
    }
    chunks[0][0]->data[3][2][3]=0.5;
    
    glm::mat4 baseModel = glm::scale(glm::vec3(cubeSize,cubeSize,cubeSize)) * glm::mat4(1.0f);
    
    




    uint32_t width=600;
    uint32_t height=600;

    const char* vertexShaderSrc = "#version 330 core\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "layout (location=0) in vec3 pos;\n"
    "void main(){\n"
    "   gl_Position=projection * view * model * vec4(pos,1);}\0";

    const char* fragmentShaderSrc = "#version 330 core\n"
    "out vec4 outColor;\n"
    "uniform vec3 color;\n"
    "void main(){outColor=vec4(color,1);}\0";


    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width,height,"test",NULL,NULL);
    if(window==NULL){
        printf("no window :(");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        printf("Failed to initialize GLAD\n");
        return -1;
    }    

    glViewport(0,0,width,height);

    int success;
    char infoLog[512];
    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSrc,NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
    if(!success){
        glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
        printf(infoLog);
        return 1;
    }

    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentShaderSrc,NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&success);
    if(!success){
        glGetShaderInfoLog(fragmentShader,512,NULL,infoLog);
        printf(infoLog);
        return 1;
    }



    shaderProgram=glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram,GL_LINK_STATUS,&success);
    
    
    if(!success){
        glGetProgramInfoLog(shaderProgram,512,NULL,infoLog);
        printf(infoLog);
        return 1;
    }


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    

    //capture mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
    glfwSetCursorPosCallback(window, mouse_callback);  
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    

    int modelLocation = glGetUniformLocation(shaderProgram, "model");
    int viewLocation = glGetUniformLocation(shaderProgram, "view");
    int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
    int colorLocation = glGetUniformLocation(shaderProgram, "color");
    //int timeLocation = glGetUniformLocation(shaderProgram, "time");

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    float color[] = {0.8f,0.8f,0.8f};

    projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 600.0f);
    

    
    

    //shader stuff
    glUseProgram(shaderProgram);
    
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(edgeTable), edgeTable, GL_STATIC_DRAW);

    unsigned char useless[15] = {0, 8, 3};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 15, useless, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glLineWidth(3.0f);

    glBindVertexArray(0);
    glUseProgram(0);
    
    

    
    

    

   

    while (!(glfwWindowShouldClose(window) || shouldClose)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camPos += camDir * camSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camPos -= glm::normalize(glm::cross(camDir, camUp)) * camSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camPos -= camDir * camSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camPos += glm::normalize(glm::cross(camDir, camUp)) * camSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            camPos.y -= camSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            camPos.y += camSpeed;
        }
        
        
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            shouldClose = true;
        }
        
        
        //render

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        camDir.x = cos(camxr) * cos(camyr);
        camDir.y = sin(camyr);
        camDir.z = sin(camxr) * cos(camyr);

        view = glm::lookAt(camPos, camPos + camDir, camUp);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        glUniform3f(colorLocation, color[0],color[1],color[2]);

        glClearColor(0.20f, 0.5f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        
        printf("\n");
        for (int chx = 0; chx < chunksWidth; chx++) {
            for (int chz = 0; chz < chunksLength; chz++) {
                for (int cx = 0; cx < chunkWidth-1; cx++) {
                    for (int cy = 0; cy < chunkHeight-1; cy++) {
                        for (int cz = 0; cz < chunkWidth-1; cz++) {
                            unsigned char a = 0;
                            if (chunks[chx][chz]->data[cx][cy][cz] > 0.0f){a|=1;}
                            if (chunks[chx][chz]->data[cx+1][cy][cz] > 0.0f){a|=2;}
                            if (chunks[chx][chz]->data[cx+1][cy][cz+1] > 0.0f){a|=4;}
                            if (chunks[chx][chz]->data[cx][cy][cz+1] > 0.0f){a|=8;}
                            if (chunks[chx][chz]->data[cx][cy+1][cz] > 0.0f){a|=16;}
                            if (chunks[chx][chz]->data[cx+1][cy+1][cz] > 0.0f){a|=32;}
                            if (chunks[chx][chz]->data[cx+1][cy+1][cz+1] > 0.0f){a|=64;}
                            if (chunks[chx][chz]->data[cx][cy+1][cz+1] > 0.0f){a|=128;}
                            
                            
                            int l = triangleConnectionTable[a][0];
                            if (l==0) {continue;}
                            
                            printf("%d %d %d %d\n",a,2*(chx*chunkWidth + cx),2*cy,2*(chz*chunkWidth + cz));

                            

                            glm::mat4 model = glm::translate(baseModel, glm::vec3(
                                2*(chx*chunkWidth + cx),
                                2*cy,
                                2*(chz*chunkWidth + cz)
                            ));
                            
                            glUniform3f(colorLocation, color[0],color[1],color[2]);
                            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
                            
                            
                            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 15, (&triangleConnectionTable[a][0])+1);
                            
                            
                            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                            glDrawElements(GL_TRIANGLES,l,GL_UNSIGNED_BYTE,0);
                            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                            glUniform3f(colorLocation, 0.0f,0.0f,0.0f);
                            glDrawElements(GL_TRIANGLES,l,GL_UNSIGNED_BYTE,0);
            }}}}
        }
        
        
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        usleep(1000000/100); //100fps
    }


    return 0;
}