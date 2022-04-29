#version 400 core

// code reference: https://stackoverflow.com/questions/4579020/how-do-i-use-a-glsl-shader-to-apply-a-radial-blur-to-an-entire-scene

out vec4 FragColor;
  
in vec2 TexCoords;
in vec2 uv;

uniform sampler2D screenTexture;

const float sampleDist = 1.0f;
const float sampleStrength = 1f; 

uniform float speedStrength = 0f;

void main()
{ 
    float samples[10];
    samples[0] = -0.08;
    samples[1] = -0.05;
    samples[2] = -0.03;
    samples[3] = -0.02;
    samples[4] = -0.01;
    samples[5] =  0.01;
    samples[6] =  0.02;
    samples[7] =  0.03;
    samples[8] =  0.05;
    samples[9] =  0.08;

    vec2 dir = 0.5 - uv; 
    float dist = sqrt(dir.x*dir.x + dir.y*dir.y); 
    dir = dir/dist; 

    vec4 color = texture(screenTexture,uv); 
    vec4 sum = color;

    for (int i = 0; i < 10; i++)
        sum += texture(screenTexture, uv + dir * samples[i] * sampleDist );

    sum *= 1.0/11.0;
    float t = dist * (sampleStrength + speedStrength);
    t = clamp( t ,0.0,1.0);

    FragColor = mix( color, sum, t );
}

// base implementation of the 2D quad for use with the FBO.
// code reference: https://learnopengl.com/Advanced-OpenGL/Framebuffers
/*
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{ 

    FragColor = texture(screenTexture, TexCoords);
}
*/