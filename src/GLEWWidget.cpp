#include <GL/glew.h>
#include "GLEWWidget.h"

static const char* vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 inPos;\n"
    "layout (location = 1) in vec3 inColor;\n"
    "out vec3 vertexColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(inPos, 1.0);\n"
    "   vertexColor = inColor;\n"
    "}\n";

static const char* fragmentShaderSource = 
    "#version 330 core\n"
    "in vec3 vertexColor;\n"
    "out vec4 fragmentColor;\n"
    "void main()\n"
    "{\n"
    "   fragmentColor = vec4(vertexColor, 1.0);\n"
    "}\n";

GLEWWidget::GLEWWidget(QWidget* parent):
    QOpenGLWidget{parent}
{

}

GLEWWidget::~GLEWWidget()
{

}


void GLEWWidget::initializeGL()
{
    glewInit();
}

void GLEWWidget::paintGL()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glUseProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
    //  --      坐标      --     --  颜色(RGB)  --
         0.0f,  0.5f,  0.0f,    1.0f, 1.0f, 0.0f,     // P1 点的坐标和颜色 
        -0.5f, -0.5f,  0.0f,    0.0f, 1.0f, 1.0f,     // P2
         0.5f, -0.5f,  0.0f,    1.0f, 0.0f, 1.0f,     // ...  
    };

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    unsigned int indices[] = {  
        0, 1, 2,    // 第一个三角形的顶点索引  
    };

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}

void GLEWWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

QSize GLEWWidget::sizeHint() const
{
    return QSize{320, 320};
}