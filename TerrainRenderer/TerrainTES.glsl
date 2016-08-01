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

	//vec4 heightVec = texture(heightSampler, vec2(0,0));

	//float value = ((abs(heightVec.x) + abs(heightVec.y) + abs(heightVec.z)) / 2726894656) * 10;

	//posn.y = value;
	//if (height < 10000000) { 
	//	posn.y = 0;
	//}
    //posn.y = height;

	//posn.y = ;
	gl_Position = mvpMatrix * posn;
}

/*layout(quads, equal_spacing, ccw) in;
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
	vec4 tex = texture(heightSampler, vec2(0, 0));
	//posn.y = 1.0;
	gl_Position = mvpMatrix * posn;
}*/