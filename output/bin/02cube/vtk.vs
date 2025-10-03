#version 150
#ifdef GL_ES
#if __VERSION__ == 300
#define attribute in
#define varying out
#endif // 300
#else // GL_ES
#define highp
#define mediump
#define lowp
#if __VERSION__ == 150
#define attribute in
#define varying out
#endif
#endif // GL_ES


attribute vec4 vertexMC;

uniform mat4 MCDCMatrix;


void main()
{
    gl_Position = MCDCMatrix * vertexMC;
}
