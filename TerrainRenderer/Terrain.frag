#version 330

in float diffTerm;

void main() 
{
	gl_FragColor = vec4(diffTerm, diffTerm, diffTerm, 1.0);
}