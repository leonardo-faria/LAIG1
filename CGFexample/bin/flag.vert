uniform float time;
varying vec4 coords;
uniform float v;

const float PI = 3.14159265358979;

void main() {

float offset = 0.0;
vec4 newCoord = vec4(gl_Vertex.x, gl_Vertex.y + (sin(gl_Vertex.x*v*PI + time*0.005*v )*0.2), gl_Vertex.z, 1.0);
gl_Position = gl_ModelViewProjectionMatrix * newCoord;

gl_FrontColor = vec4(0, 0, 0 ,0.0);
gl_TexCoord[0] = gl_MultiTexCoord0;
coords = newCoord ;
}