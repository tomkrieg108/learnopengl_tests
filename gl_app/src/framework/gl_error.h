#pragma once

#include <GLFW/glfw3.h> //for APIENTRY

#include "base.h"

void GLClearError();
bool GLLogCall(char* function, char* file, int line);

//https://learnopengl.com/In-Practice/Debugging
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
    GLsizei length, const char* message, const void* userParam);

#ifdef DEBUG
    #define GLCALL(x) GLClearError();x;ASSERT(GLLogCall(#x, __FILE__, __LINE__)) //wrap around all gl func calls
#else
    #define GLCALL(x) x
#endif




/*
typedef void (* DEBUGPROC)(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam);
*/
