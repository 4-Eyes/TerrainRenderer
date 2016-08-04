#version 400

layout(vertices = 4) out;

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	gl_TessLevelOuter[0] = 6;
	gl_TessLevelOuter[1] = 6;
	gl_TessLevelOuter[2] = 6;
	gl_TessLevelOuter[3] = 6;
	gl_TessLevelInner[0] = 6;
	gl_TessLevelInner[1] = 6;
}