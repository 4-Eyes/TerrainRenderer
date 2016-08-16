#version 400
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform vec4 lightPos;
uniform mat4 mvpMatrix;

uniform float waterFraction;
uniform float grassFraction;
uniform float grassRockBlendFraction;
uniform float rockFraction;
uniform float rockSnowBlendFraction;

uniform float cellSize;
uniform float terrainHeight;

out float diffTerm;
out float[4] weights;
out vec2 texCoords;

void main()
{
	float height = cellSize * terrainHeight;
	float pointHeight;
	vec3 vertex1 = gl_in[0].gl_Position.xyz;
	vec3 vertex2 = gl_in[1].gl_Position.xyz;
	vec3 vertex3 = gl_in[2].gl_Position.xyz;

	vec3 normal = normalize(cross(vertex2 - vertex1, vertex3 - vertex1));
	for (int i = 0; i < gl_in.length(); i++)
	{
		vec3 lghtVec = normalize(lightPos.xyz - gl_in[i].gl_Position.xyz);
		diffTerm = max(dot(lghtVec, normal), 0.0f);
		// Work out texture weightings
		pointHeight = gl_in[i].gl_Position.y;
		if (pointHeight < height * waterFraction) {
			weights[0] = 0;
			weights[1] = 0;
			weights[2] = 0;
			weights[3] = 1;
		}
		else if (pointHeight >= height * waterFraction && pointHeight < height * grassFraction) {
			weights[0] = 0;
			weights[1] = 0;
			weights[2] = 1;
			weights[3] = 0;
		}
		else if (pointHeight >= height * grassFraction && pointHeight < height * grassRockBlendFraction) {
			weights[0] = 0;
			weights[1] = (-1.0 / (height * (grassRockBlendFraction - grassFraction))) * pointHeight + (1 + grassFraction / (grassRockBlendFraction - grassFraction));
			weights[2] = (1.0 / (height * (grassRockBlendFraction - grassFraction))) * pointHeight - grassFraction / (grassRockBlendFraction - grassFraction);
			weights[3] = 0;
		}
		else if (pointHeight >= height * grassRockBlendFraction && pointHeight < height * rockFraction) {
			weights[0] = 0;
			weights[1] = 1;
			weights[2] = 0;
			weights[3] = 0;
		} else if (pointHeight >= height * rockFraction && pointHeight < height * rockSnowBlendFraction) {
			weights[0] = (-1.0 / (height * (rockSnowBlendFraction - rockFraction))) * pointHeight + (1 + rockFraction / (rockSnowBlendFraction - rockFraction));
			weights[1] = (1.0 / (height * (rockSnowBlendFraction - rockFraction))) * pointHeight - rockFraction / (rockSnowBlendFraction - rockFraction);
			weights[2] = 0;
			weights[3] = 0;
		}
		else {
			weights[0] = 1;
			weights[1] = 0;
			weights[2] = 0;
			weights[3] = 0;		}
		// Work out texture coordinates;
		texCoords = vec2(i * 0.5, i % 2);

		gl_Position = mvpMatrix * gl_in[i].gl_Position;
		EmitVertex();
	}
}