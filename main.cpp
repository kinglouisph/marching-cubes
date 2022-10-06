#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

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

float camxr = M_PI_2;
float camyr = 0.0f;
const float camRotSpeed = 0.003f;
glm::vec3 camPos = glm::vec3(0.0f,0.0f,0.0f);
glm::vec3 camDir = glm::vec3(0.0f,0.0f,1.0f);
double lmx = 0.0f;
double lmy = 0.0f;
char firstMouse 

char chunkWidth = 20;
char chunkHeight = 30;
typedef struct Chunk {
    float data[chunkWidth][chunkHeight][chunkWidth]//,
    //int x,
    //int y
} Chunk;

Chunk* chunks[1][1];

chunks[0][0]=(Chunk)malloc(sizeof(Chunk));
chunks[0][0]->data[1][1][1]=0.0f;


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

char shouldClose = 0;









int main() {
    uint32_t width=800;
    uint32_t height=600;

    const char* vertexShaderSrc = "#version 330 core\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    //"uniform float time;\n"
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
    //glfwSetMouseButtonCallback(window, mouse_button_callback);

    

    int modelLocation = glGetUniformLocation(shaderProgram, "model");
    int viewLocation = glGetUniformLocation(shaderProgram, "view");
    int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
    int colorLocation = glGetUniformLocation(shaderProgram, "color");
    //int timeLocation = glGetUniformLocation(shaderProgram, "time");

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    float color[] = {1.0f,0.0f,0.0f};

    projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 500.0f);
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    
    

    //shader stuff
    glUseProgram(shaderProgram);
    
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(squarePoints), squarePoints, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareInds), squareInds, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    

    

   

    while (!(glfwWindowShouldClose(window) || shouldClose)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camPos = camPos + camDir;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camPos = camPos + camDir;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            
        }
        
        
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            shouldClose = true;
        }
        
        
        //render
        glClearColor(0.20f, 0.5f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < 0; i++) {
            //glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(a->model));
            glUniform3f(colorLocation, 0.9f,0.95f,0.3f);

            //glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);
        }
        
        
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        usleep(1000000/100); //100fps
    }


    return 0;
}