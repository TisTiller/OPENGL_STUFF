#version 330 core
in vec2 gl_FragCoord;
uniform float Width;
uniform float Height;
out vec4 FragColor;
in vec3 col;
void main()
{
	FragColor = vec4(col.x, col.y, col.z, 1.0f);
}