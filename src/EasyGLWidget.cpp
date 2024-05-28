#include "EasyGLWidget.h"

#include <QOpenGLContext>
#include <QDateTime>
#include <QTimer>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace EasyGL;

struct Light
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 pos;
};

struct Material
{
    float ambient[3];
    float diffuse[3];
    float specular[3];
    float shininess;
};

static const char *vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 inPos;\n"
    "layout (location = 1) in vec3 inColor;\n"
    "out vec3 vertexColor;\n"
    "out vec3 vertexPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(inPos, 1.0);\n"
    "   vertexPos = vec3(model * vec4(inPos, 1.0));\n"
    "   vertexColor = inColor;\n"
    "}\n";

static const char *geometryShaderSource = 
    "#version 330 core\n"
    "layout (triangles) in;\n"
    "layout (triangle_strip, max_vertices = 3) out;\n"
    "in vec3 vertexColor[];\n"
    "in vec3 vertexPos[];\n"
    "out vec3 geometryColor;\n"
    "out vec3 geometryPos;\n"
    "out vec3 normalVec;\n"
    "void main()\n"
    "{\n"
    "   vec3 v1 = vertexPos[1] - vertexPos[0];\n"
    "   vec3 v2 = vertexPos[2] - vertexPos[0];\n"
    "   vec3 norm = normalize(cross(v2, v1));\n"
    "   for (int i = 0; i < gl_in.length(); i++){\n"
    "       gl_Position = gl_in[i].gl_Position;\n"
    "       geometryColor = vertexColor[i];\n"
    "       geometryPos = vertexPos[i];\n"
    "       normalVec = norm;\n"
    "       EmitVertex();\n"
    "   }\n"
    "   EndPrimitive();"
    "}\n";;

static const char *fragmentShaderSource = 
    "#version 330 core\n"
    "in vec3 geometryColor;\n"
    "in vec3 geometryPos;\n"
    "in vec3 normalVec;\n"
    "out vec4 fragmentColor;\n"
    "uniform vec3 cameraPos;\n"
    "struct Light{\n"
    "   vec3 ambient;\n"
    "   vec3 diffuse;\n"
    "   vec3 specular;\n"
    "   vec3 pos;\n"
    "};\n"
    "uniform Light light;\n"
    "struct Material{\n"
    "   vec3 ambient;\n"
    "   vec3 diffuse;\n"
    "   vec3 specular;\n"
    "   float shininess;\n"
    "};\n"
    "uniform Material material;\n"
    "uniform sampler2D inTexture;\n"
    "void main()\n"
    "{\n"
    "   vec3 lightVec = normalize(light.pos - geometryPos);\n"
    "   vec3 diffuse = material.diffuse * max(dot(normalVec, lightVec), 0.0f);\n"
    "   vec3 cameraVec = normalize(cameraPos - geometryPos);\n"
    "   vec3 reflectVec = reflect(-lightVec, normalVec);\n"
    "   vec3 specular = material.specular * pow(max(dot(cameraVec, reflectVec), 0.0), material.shininess);\n"
    "   vec3 fusion = material.ambient * light.ambient + diffuse * light.diffuse + specular * light.specular;\n"
    "   fragmentColor = vec4(fusion * geometryColor, 1.0f);\n"
    "}\n";


static const char *lightVertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 inPos;\n"
    "layout (location = 1) in vec3 inColor;\n"
    "out vec3 vertexColor;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(inPos, 1.0);\n"
    "   vertexColor = inColor;\n"
    "}\n";

static const char *lightfragmentShaderSource = 
    "#version 330 core\n"
    "in vec3 vertexColor;\n"
    "out vec4 fragmentColor;\n"
    "void main()\n"
    "{\n"
    "   fragmentColor = vec4(vertexColor, 1.0);\n"
    "}\n";

// FROM: http://devernay.free.fr/cours/opengl/materials.html
static std::vector<Material> materials = {
    {0.0215, 0.1745, 0.0215, 0.07568, 0.61424, 0.07568, 0.633, 0.727811, 0.633, 0.6},
    {0.135, 0.2225, 0.1575, 0.54, 0.89, 0.63, 0.316228, 0.316228, 0.316228, 0.1},
    {0.05375, 0.05, 0.06625, 0.18275, 0.17, 0.22525, 0.332741, 0.328634, 0.346435, 0.3},
    {0.25, 0.20725, 0.20725, 1, 0.829, 0.829, 0.296648, 0.296648, 0.296648, 0.088},
    {0.1745, 0.01175, 0.01175, 0.61424, 0.04136, 0.04136, 0.727811, 0.626959, 0.626959, 0.6},
    {0.1, 0.18725, 0.1745, 0.396, 0.74151, 0.69102, 0.297254, 0.30829, 0.306678, 0.1},
    {0.329412, 0.223529, 0.027451, 0.780392, 0.568627, 0.113725, 0.992157, 0.941176, 0.807843, 0.21794872},
    {0.2125, 0.1275, 0.054, 0.714, 0.4284, 0.18144, 0.393548, 0.271906, 0.166721, 0.2},
    {0.25, 0.25, 0.25, 0.4, 0.4, 0.4, 0.774597, 0.774597, 0.774597, 0.6},
    {0.19125, 0.0735, 0.0225, 0.7038, 0.27048, 0.0828, 0.256777, 0.137622, 0.086014, 0.1},
    {0.24725, 0.1995, 0.0745, 0.75164, 0.60648, 0.22648, 0.628281, 0.555802, 0.366065, 0.4},
    {0.19225, 0.19225, 0.19225, 0.50754, 0.50754, 0.50754, 0.508273, 0.508273, 0.508273, 0.4},
    {0.0, 0.0, 0.0, 0.01, 0.01, 0.01, 0.50, 0.50, 0.50, .25},
    {0.0, 0.1, 0.06, 0.0, 0.50980392, 0.50980392, 0.50196078, 0.50196078, 0.50196078, .25},
    {0.0, 0.0, 0.0, 0.1, 0.35, 0.1, 0.45, 0.55, 0.45, .25},
    {0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.7, 0.6, 0.6, .25},
    {0.0, 0.0, 0.0, 0.55, 0.55, 0.55, 0.70, 0.70, 0.70, .25},
    {0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.60, 0.60, 0.50, .25},
    {0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.4, 0.4, 0.4, .078125},
    {0.0, 0.05, 0.05, 0.4, 0.5, 0.5, 0.04, 0.7, 0.7, .078125},
    {0.0, 0.05, 0.0, 0.4, 0.5, 0.4, 0.04, 0.7, 0.04, .078125},
    {.05, 0.0, 0.0, 0.5, 0.4, 0.4, 0.7, 0.04, 0.04, .078125},
    {0.05, 0.05, 0.05, 0.5, 0.5, 0.5, 0.7, 0.7, 0.7, .078125},
    {0.05, 0.05, 0.0, 0.5, 0.5, 0.4, 0.7, 0.7, 0.04, .078125},
};

static GLADapiproc GetProcAddress(const char *name)
{
    QOpenGLContext* ctx = QOpenGLContext::currentContext();
    return static_cast<GLADapiproc>(ctx->getProcAddress(name));
}

static float GetTime()
{
    return static_cast<float>(QDateTime::currentMSecsSinceEpoch() % 1000000 / 1000.0);
}

EasyGLWidget::EasyGLWidget(QWidget* parent):
    QOpenGLWidget{parent}
{
    QTimer* timer = new QTimer{this};
    connect(timer, &QTimer::timeout, [this](){
        update();
    });
    timer->start(20);
}

EasyGLWidget::~EasyGLWidget()
{
    
}

void EasyGLWidget::initializeGL()
{
    gladLoadGL(GetProcAddress);
}

void EasyGLWidget::paintGL()
{
    glEnable(GL_DEPTH_TEST);
    // 光源
    VertexShader lightVertexShader{lightVertexShaderSource};
    FragmentShader lightFragmentShader{lightfragmentShaderSource};
    ShaderProgram lightShaderProgram;
    lightShaderProgram.attach(lightVertexShader);
    lightShaderProgram.attach(lightFragmentShader);
    lightShaderProgram.link();
    lightShaderProgram.use();

    glm::vec3 lightColor{1.0f, 1.0f, 1.0f};
    Light light{
        0.2f*lightColor,
        0.5f*lightColor,
        1.0f*lightColor,
    };
    float lightVertices[] = {
        // ---- 位置 ----        ---   颜色   ---
        -0.1f,  0.0f,  0.0f,    lightColor[0], lightColor[1], lightColor[2],
         0.1f,  0.0f,  0.0f,    lightColor[0], lightColor[1], lightColor[2],

         0.0f, -0.1f,  0.0f,    lightColor[0], lightColor[1], lightColor[2],
         0.0f,  0.1f,  0.0f,    lightColor[0], lightColor[1], lightColor[2],

         0.0f,  0.0f, -0.1f,    lightColor[0], lightColor[1], lightColor[2],
         0.0f,  0.0f,  0.1f,    lightColor[0], lightColor[1], lightColor[2],
    };

    VertexBuffer lightVertexBuffer;
    lightVertexBuffer.setData(sizeof(lightVertices), lightVertices, VertexBuffer::Usage::StreamDraw);

    VertexArray lightVertexArray;
    lightVertexArray.attribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0);
    lightVertexArray.attribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(sizeof(float) * 3));

    // 图形
    VertexShader vertexShader{vertexShaderSource};
    GeometryShader geometryShader{geometryShaderSource};
    FragmentShader fragmentShader{fragmentShaderSource};
    ShaderProgram shaderProgram;
    shaderProgram.attach(vertexShader);
    shaderProgram.attach(geometryShader);
    shaderProgram.attach(fragmentShader);
    shaderProgram.link();
    shaderProgram.use();

    //   4 --- 5
    //  /|    /|
    // 0 --- 1 |  
    // | 7 --| 6
    // |/    |/
    // 3 --- 2

    float vertices[] = {
        // ---- 位置 ----      - 颜色 -
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
    };

    unsigned int indices[] = {  
        0, 1, 2, // first triangle
        0, 2, 3,  // second triangle

        5, 4, 7,
        5, 7, 6,

        1, 5, 6,
        1, 6, 2,

        4, 0, 3,
        4, 3, 7,
    
        4, 5, 1,
        4, 1, 0,

        3, 2, 6,
        3, 6, 7,
    };

    VertexBuffer vertexBuffer;
    vertexBuffer.setData(sizeof(vertices), vertices, VertexBuffer::Usage::StreamDraw);

    VertexArray vertexArray;
    vertexArray.attribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0);
    vertexArray.attribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(sizeof(float) * 3));

    IndexBuffer indexBuffer;
    indexBuffer.setData(sizeof(indices), indices, IndexBuffer::Usage::StaticDraw);

    // 世界坐标
    std::vector<glm::vec3> cubePositions = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f),
    };

    // 摄像机
    Camera camera{glm::vec3{0.0f, 0.0f, 10.0f}};
    
    // 绘图
    float aspect = static_cast<float>(width()) / static_cast<float>(height());
    glm::mat4 projection = camera.projection(aspect);

    glClearColor(light.ambient[0], light.ambient[1], light.ambient[2], 0.1f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 绘制光源
    lightShaderProgram.use();
    lightVertexArray.bind();
    lightShaderProgram.setMatrix<4>("view", glm::value_ptr(camera.view())); // 摄像机 View
    lightShaderProgram.setMatrix<4>("projection", glm::value_ptr(projection));  // 摄像机投影
    float radius = 3.0f;
    light.pos = {
        radius*glm::sin(GetTime()), 
        0.0f, 
        radius*glm::cos(GetTime())
    };

    glm::mat4 lightModel{1.0f};
    lightModel = glm::translate(lightModel, light.pos); // 移动到世界坐标
    lightShaderProgram.setMatrix<4>("model", glm::value_ptr(lightModel));
    glDrawArrays(GL_LINES, 0, 6);

    // 绘制图形
    shaderProgram.use();
    vertexArray.bind();
    shaderProgram.setMatrix<4>("view", glm::value_ptr(camera.view()));      // 摄像机 View
    shaderProgram.setMatrix<4>("projection", glm::value_ptr(projection));   // 摄像机投影
    shaderProgram.setVector<3>("cameraPos", glm::value_ptr(camera.pos()));  // 设置 view 坐标计算镜面光照
    shaderProgram.setVector<3>("light.pos", glm::value_ptr(light.pos));
    shaderProgram.setVector<3>("light.ambient", glm::value_ptr(light.ambient));             // 设置环境光
    shaderProgram.setVector<3>("light.diffuse", glm::value_ptr(light.diffuse));             // 设置漫反射光
    shaderProgram.setVector<3>("light.specular", glm::value_ptr(light.specular));           // 设置镜面反射光
    for (size_t i = 0; i < cubePositions.size(); i++)
    {
        size_t index = i % materials.size();
        shaderProgram.setVector<3>("material.ambient", materials[index].ambient);       // 设置环境光系数
        shaderProgram.setVector<3>("material.diffuse", materials[index].diffuse);       // 设置漫反射系数
        shaderProgram.setVector<3>("material.specular", materials[index].specular);     // 设置镜面反射系数
        shaderProgram.setValue("material.shininess", materials[index].shininess * 128); // 设置反光度
        glm::mat4 model{1.0f};
        model = glm::translate(model, cubePositions[i]); // 移动到世界坐标
        model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f)); // 随便加点角度
        model = glm::rotate(model, GetTime(), glm::vec3(0.5f, 1.0f, 0.0f)); // 动画
        shaderProgram.setMatrix<4>("model", glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

void EasyGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

QSize EasyGLWidget::sizeHint() const
{
    return QSize{640, 640};
}