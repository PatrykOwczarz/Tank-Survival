# Tank-Survival
Using an OpenGL based C++ graphics engine I have created a game which featured a tank following a Catmull-Rom spline. The primary focus of this project was to learn how graphics is rendered via the use of buffers and shaders. This project was done for my Computer Graphics module in my second year of university.

# Notable Algorithms and Learnings:
## Catmull-rom Spline and OpenGL Rendering:
- Created a track based on a set of control points. A Catmull-rom spline is a continuous spline.
- To work out the shape of the spline, an interpolation formula is used which takes 4 consecutive control points. This formula can be seen on line 17 in this file: https://github.com/PatrykOwczarz/Tank-Survival/blob/main/Tank%20Survival/OpenGLTemplate/CatmullRom.cpp
- Using this formula I have created a centreline based on the spline defined by the control points.
- To create an actual track I had to use the cross product to work out 2 points left and right of the centreline which are perpendicular and equidistant to the centreline.
- I then created a 2d track by creating a plane facing upwards by utilising the render pipeline that OpenGL uses.
- I stored each vertex in a VAO (Vertex Array Object) and each of those vertices was acted upon by a vertex shader seen here: https://github.com/PatrykOwczarz/Tank-Survival/blob/main/Tank%20Survival/OpenGLTemplate/resources/shaders/mainShader.vert
- The BlinnPhong Model was used for shading.
- I also implemented a fragment shader which was used after Rasterization: https://github.com/PatrykOwczarz/Tank-Survival/blob/main/Tank%20Survival/OpenGLTemplate/resources/shaders/mainShader.frag

## Camera and various shader effects:
- I have implemented the following camera angles: Top down view (birdseye view), First person which was looking just above the barrel of the tank and Third Person view which was behind the tank.
- These camera angles were based on the position of the tank which was moving at a steady speed along the spline, following the centreline defined by the above implementation.
- I also implemented raidal blur, fog and a night time mode with a spotlight attached to it.

# Areas of improvement:
- **Complexity of track** - I could make the track more interesting with more verticality and potentially slanting horizontally slightly to simulate a more realistic surface.
- Other than that I think I am happy with how this game turned out, considering it was a small project that I did for a University module.

# Demo Video:

Demo: https://www.youtube.com/watch?v=iNohO_cFge0
