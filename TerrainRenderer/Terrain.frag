#version 330

in float diffTerm;

void main() 
{
	vec4 colour = vec4(1.0, 0.5, 0.3, 1.0);
	gl_FragColor = colour * vec4(diffTerm, diffTerm, diffTerm, 1.0);
}