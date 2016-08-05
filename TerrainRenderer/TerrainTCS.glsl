#version 400

layout(vertices = 4) out;

uniform vec3 eyePos;

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	/*float high = 6;
	float low = 1;
	float dmin = 0;
	float dmax = 300;
	float d = min(abs(distance(gl_in[gl_InvocationID].gl_Position.xyz, eyePos)), dmax);

	highp int L = int(((d - dmin)/(dmax - dmin)) * (low - high) + high);
	
	gl_TessLevelOuter[0] = L;
	gl_TessLevelOuter[1] = L;
	gl_TessLevelOuter[2] = L;
	gl_TessLevelOuter[3] = L;
	gl_TessLevelInner[0] = L;
	gl_TessLevelInner[1] = L;
	*/
	gl_TessLevelOuter[0] = 6;
	gl_TessLevelOuter[1] = 6;
	gl_TessLevelOuter[2] = 6;
	gl_TessLevelOuter[3] = 6;
	gl_TessLevelInner[0] = 3;
	gl_TessLevelInner[1] = 3;
	
}