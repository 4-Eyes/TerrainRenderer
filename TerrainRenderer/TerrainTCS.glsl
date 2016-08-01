#version 400

layout(vertices = 4) out;

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	gl_TessLevelOuter[0] = 1;
	gl_TessLevelOuter[1] = 1;
	gl_TessLevelOuter[2] = 1;
	gl_TessLevelOuter[3] = 1;
	gl_TessLevelInner[0] = 1;
	gl_TessLevelInner[1] = 1;
}