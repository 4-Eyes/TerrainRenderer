#version 400

layout(quads, equal_spacing, ccw) in;
uniform mat4 mvpMatrix;
uniform sampler2D heightSampler;
vec4 posn;

void main() 
{
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	posn = (1-u) * (1-v) * gl_in[0].gl_Position
		 + u * (1-v) * gl_in[1].gl_Position
		 + u * v * gl_in[2].gl_Position
		 + (1-u) * v * gl_in[3].gl_Position;
	// Todo work out how to feed the height map position in and translate appropriately.
	// vec2 texCoord = vec2(0.0, 0.0);
	gl_Position = mvpMatrix * posn;
}