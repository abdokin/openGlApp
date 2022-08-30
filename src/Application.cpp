#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};
static ShaderProgramSource PareShader(const std::string& filePath)
{
    std::ifstream stream(filePath);
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream,line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                // set vertex
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                // set fragment
                type = ShaderType::FRAGMENT;

            }
        }
         else {
                ss[(int)type] << line << '\n';
            }
        
    }
    return {ss[0].str(),ss[1].str()};

}
static unsigned int CompileShader(unsigned int type,const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1,&src,nullptr);
    glCompileShader(id);
    // TODO:Error handling;
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char *)malloc(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex " : " fragment") << std::endl;
        std::cout << message << std::endl;
        return 0;
    }
    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    glAttachShader(program,vs);
    glAttachShader(program,fs);
    glLinkProgram(program);
    glValidateProgram(program);
    glDeleteProgram(vs);
    glDeleteProgram(fs);
    return program;
}
int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "first OpenGl App", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error!" << std::endl;
    };

    // gl version:
    std::cout << glGetString(GL_VERSION) << std::endl;
    // define vertex buffer 
    float positions[] = {
         -0.5f,-0.5f,
         0.5f,-0.5f,
         0.5f, 0.5f,
         -0.5f,0.5f,
    };
    unsigned int indices[] = {
          0,1,2,
          2,3,0
    };
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    // bind the buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // specify the data;
    glBufferData(GL_ARRAY_BUFFER, 6 *2* sizeof(float), positions, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    // bind the buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    // specify the data;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 2 * sizeof(float), indices, GL_STATIC_DRAW);


    // shader 
    ShaderProgramSource source = PareShader("res/shaders/Basic.shader");
     unsigned int shader = CreateShader(source.VertexSource,source.FragmentSource);
    
    glUseProgram(shader);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}