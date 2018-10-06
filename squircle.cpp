#include <iostream>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLShaderProgram>
#include <QtQuick/qquickwindow.h>

#include "squircle.h"

const char* vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
const char* fragmentShader1Source = "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "void main()\n"
                                    "{\n"
                                    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                    "}\n\0";
const char* fragmentShader2Source = "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "void main()\n"
                                    "{\n"
                                    "   FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
                                    "}\n\0";

Squircle::Squircle()
    : m_renderer(nullptr)
{
    connect(this, &QQuickItem::windowChanged, this,
        &Squircle::handleWindowChanged);
}

SquircleRenderer::~SquircleRenderer() { delete m_program; }

void Squircle::handleWindowChanged(QQuickWindow* win)
{
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &Squircle::sync,
            Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &Squircle::cleanup,
            Qt::DirectConnection);
        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        //win->setPersistentOpenGLContext(true);
        win->setClearBeforeRendering(false);
    }
}

void Squircle::sync()
{
    if (!m_renderer) {
        m_renderer = new SquircleRenderer();
        connect(window(), &QQuickWindow::beforeRendering, m_renderer,
            &SquircleRenderer::paint, Qt::DirectConnection);
    }
    // m_renderer->setT(m_t);
    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setWindow(window());
}

void Squircle::cleanup()
{
    if (m_renderer) {
        delete m_renderer;
        m_renderer = nullptr;
    }
}

void SquircleRenderer::paint()
{
    if (!m_program) {
        initializeOpenGLFunctions();

        m_program = new QOpenGLShaderProgram();
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex,
            "attribute highp vec4 vertices;"
            "varying highp vec2 coords;"
            "void main() {"
            "    gl_Position = vertices;"
            "    coords = vertices.xy;"
            "}");
        m_program->addCacheableShaderFromSourceCode(
            QOpenGLShader::Fragment,
            "uniform lowp float t;"
            "varying highp vec2 coords;"
            "void main() {"
            "    lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), "
            "4.));"
            "    i = smoothstep(t - 0.8, t + 0.8, i);"
            "    i = floor(i * 20.) / 20.;"
            "    gl_FragColor = vec4(coords * .5 + .5, i, i);"
            "}");

        m_program->bindAttributeLocation("vertices", 0);
        m_program->link();
    }
    m_program->bind();
    m_program->enableAttributeArray(0);

    float values[] = { -1, -1, 1, -1, -1, 1, 1, 1 };
    m_program->setAttributeArray(0, GL_FLOAT, values, 2);
    // m_program->setUniformValue("t", (float)m_t);

    //////////////////////////////////////////////////////////////
    // build and compile our shader program
    // ------------------------------------
    // we skipped compile log checks this time for readability (if you do
    // encounter issues, add the compile-checks! see previous code samples)
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShaderOrange = glCreateShader(GL_FRAGMENT_SHADER); // the first fragment shader that
        // outputs the color orange
    unsigned int fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER); // the second fragment shader that
        // outputs the color yellow
    unsigned int shaderProgramOrange = glCreateProgram();
    unsigned int shaderProgramYellow = glCreateProgram(); // the second shader program
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glShaderSource(fragmentShaderOrange, 1, &fragmentShader1Source, nullptr);
    glCompileShader(fragmentShaderOrange);
    glShaderSource(fragmentShaderYellow, 1, &fragmentShader2Source, nullptr);
    glCompileShader(fragmentShaderYellow);
    // link the first program object
    glAttachShader(shaderProgramOrange, vertexShader);
    glAttachShader(shaderProgramOrange, fragmentShaderOrange);
    glLinkProgram(shaderProgramOrange);
    // then link the second program object using a different fragment shader (but
    // same vertex shader) this is perfectly allowed since the inputs and outputs
    // of both the vertex and fragment shaders are equally matched.
    glAttachShader(shaderProgramYellow, vertexShader);
    glAttachShader(shaderProgramYellow, fragmentShaderYellow);
    glLinkProgram(shaderProgramYellow);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float firstTriangle[] = {
        -0.9f, -0.5f, 0.0f, // left
        -0.0f, -0.5f, 0.0f, // right
        -0.45f, 0.5f, 0.0f, // top
    };
    float secondTriangle[] = {
        0.0f, -0.5f, 0.0f, // left
        0.9f, -0.5f, 0.0f, // right
        0.45f, 0.5f, 0.0f // top
    };
    unsigned int VBOs[2], VAOs[2];
    // glGenVertexArrays(2, VAOs); // we can also generate multiple VAOs or
    // buffers at the same time
    glGenBuffers(2, VBOs);
    // first triangle setup
    // --------------------
    // glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle,
        GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
        (void*)nullptr); // Vertex attributes stay the same
    glEnableVertexAttribArray(0);

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());
    glDisable(GL_DEPTH_TEST);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // now when we draw the triangle we first use the vertex and orange fragment
    // shader from the first program
    glUseProgram(shaderProgramOrange);
    // draw the first triangle using the data from our first VAO
    // glBindVertexArray(VAOs[0]);
    glDrawArrays(GL_TRIANGLES, 0, 3); // this call should output an orange triangle

    ///////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////
    //    glDisable(GL_DEPTH_TEST);
    //    glClearColor(0, 0, 0, 1);
    //    glClear(GL_COLOR_BUFFER_BIT);

    //    // build and compile our shader program
    //    // ------------------------------------
    //    // vertex shader
    //    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //    glShaderSource(vertexShader, 1, &vertexShaderSource, 0);
    //    glCompileShader(vertexShader);
    //    // check for shader compile errors
    //    int success;
    //    char infoLog[512];
    //    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    //    if (!success) {
    //        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    //        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
    //                  << infoLog << std::endl;
    //    }
    //    // fragment shader
    //    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //    glShaderSource(fragmentShader, 1, &fragmentShader1Source, 0);
    //    glCompileShader(fragmentShader);
    //    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    //    if (!success) {
    //        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
    //        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
    //                  << infoLog << std::endl;
    //    }
    //    // link shaders
    //    GLuint shaderProgram = glCreateProgram();
    //    glAttachShader(shaderProgram, vertexShader);
    //    glAttachShader(shaderProgram, fragmentShader);
    //    glLinkProgram(shaderProgram);
    //    // check for linking errors
    //    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    //    if (!success) {
    //        glGetProgramInfoLog(shaderProgram, 512, 0, infoLog);
    //        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
    //                  << infoLog << std::endl;
    //    }
    //    glDeleteShader(vertexShader);
    //    glDeleteShader(fragmentShader);
    //    float vertices[] = {
    //        -0.5f, -0.5f, 0.0f, // left
    //        0.5f, -0.5f, 0.0f, // right
    //        0.0f, 0.5f, 0.0f // top
    //    };

    //    unsigned int VBO;
    //    glGenBuffers(1, &VBO);
    //    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
    //    glEnableVertexAttribArray(0);
    //    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //////////////////////////////////////////////////////////////

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

    m_program->disableAttributeArray(0);
    m_program->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    m_window->resetOpenGLState();
}
