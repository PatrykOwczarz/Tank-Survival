#version 400 core

// code reference: https://stackoverflow.com/questions/4579020/how-do-i-use-a-glsl-shader-to-apply-a-radial-blur-to-an-entire-scene

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

out vec2 uv;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    TexCoords = aTexCoords;
    uv = (vec2( gl_Position.x, + gl_Position.y ) + vec2(1.0) ) / vec2(2.0);
    uv = vec2(uv.x, uv.y);
}  