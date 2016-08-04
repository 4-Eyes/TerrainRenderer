#version 400
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform vec4 lightPos;
uniform mat4 mvpMatrix;

out float diffTerm;

void main()
{
	vec3 vertex1 = gl_in[0].gl_Position.xyz;
	vec3 vertex2 = gl_in[1].gl_Position.xyz;
	vec3 vertex3 = gl_in[2].gl_Position.xyz;

	vec3 normal = normalize(cross(vertex2 - vertex1, vertex3 - vertex1));
	for (int i = 0; i < gl_in.length(); i++)
	{
		vec3 lghtVec = normalize((mvpMatrix * lightPos).xyz - gl_in[i].gl_Position.xyz);
		diffTerm = max(dot(lghtVec, normal), 0.0f);
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
}