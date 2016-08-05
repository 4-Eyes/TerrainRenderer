#version 400

layout(quads, equal_spacing, ccw) in;
uniform mat4 mvpMatrix;
uniform sampler2D heightSampler;

void main() {
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec4 posn = (1-u) * (1-v) * gl_in[0].gl_Position
	          +    u  * (1-v) * gl_in[1].gl_Position
	          +    u  *    v  * gl_in[2].gl_Position
	          + (1-u) *    v  * gl_in[3].gl_Position;

	vec4 h = texture(heightSampler, vec2(posn.x/512.0,posn.z/512.0));
	posn.y = (h.r + h.g + h.b) * 15.0;
	gl_Position = posn;
}