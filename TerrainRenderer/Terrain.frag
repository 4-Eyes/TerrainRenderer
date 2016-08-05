#version 330

in float diffTerm;
in vec2 texCoords;
in float[4] weights;
uniform sampler2D snowSampler;
uniform sampler2D rockSampler;
uniform sampler2D grassSampler;
uniform sampler2D waterSampler;

out vec4 fragColour;

void main() 
{
	vec4 s = texture(snowSampler, texCoords);
	vec4 r = texture(rockSampler, texCoords);
	vec4 g = texture(grassSampler, texCoords);
	vec4 w = texture(waterSampler, texCoords);

	fragColour = vec4(diffTerm, diffTerm, diffTerm, 1.0) * (weights[0] * s + weights[1] * r + weights[2] * g + weights[3] * w);
}