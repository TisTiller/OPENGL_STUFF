#version 330 core
in vec2 gl_FragCoord;
uniform float Width;
uniform float Height;
uniform float blue;
out vec4 FragColor;
void main()
{
	FragColor = vec4(gl_FragCoord.x/Width, gl_FragCoord.y/Height, blue, 1.0f);
}