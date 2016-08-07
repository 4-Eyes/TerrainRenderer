#version 400

layout(vertices = 4) out;

uniform vec3 eyePos;

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	
	float d;
	float high = 6;
	float low = 1;
	float dmin = 0;
	float dmax = 200;
	/*float dmin = abs(distance(gl_in[0].gl_Position.xyz, eyePos));
	float dmax = dmin;
	for (int i = 1; i < gl_in.length(); i++) {
		d = abs(distance(gl_in[i].gl_Position.xyz, eyePos));
		if (d < dmin) {
			dmin = d;
		}
		else if (d > dmax) {
			dmax = d;
		}
	}*/
	d = abs(distance(gl_in[gl_InvocationID].gl_Position.xyz, eyePos));

	highp int L = int(((d - dmin)/(dmax - dmin)) * (low - high) + high);
	
	if (L < 1) {
		gl_TessLevelOuter[0] = low;
		gl_TessLevelOuter[1] = low;
		gl_TessLevelOuter[2] = low;
		gl_TessLevelOuter[3] = low;
		gl_TessLevelInner[0] = low;
		gl_TessLevelInner[1] = low;
	}
	else {
		gl_TessLevelOuter[0] = L;
		gl_TessLevelOuter[1] = L;
		gl_TessLevelOuter[2] = L;
		gl_TessLevelOuter[3] = L;
		gl_TessLevelInner[0] = L;
		gl_TessLevelInner[1] = L;
	}
}