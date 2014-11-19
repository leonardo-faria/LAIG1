uniform sampler2D baseTexture;
varying vec4 coords;

void main()
{
	
	gl_FragColor = texture2D(baseTexture, gl_TexCoord[0].st+coords.y*(1.2,1.2,1.2,1.0));
}