// soft shadow map fragment program for 32 bits float texture
#version 110

varying vec4 io_vertexPos;

void main()
{
	float depth = io_vertexPos.z / io_vertexPos.w ;
	depth = depth * 0.5 + 0.5;

	// don't forget to move away from unit cube ([-1,1]) to [0,1] coordinate system
	float moment1 = depth;
	float moment2 = depth * depth;

	// adjusting moments (this is sort of bias per pixel) using partial derivative
	float dx = dFdx(depth);
	float dy = dFdy(depth);

	moment2 += 0.25*(dx*dx+dy*dy);

	gl_FragColor = vec4(moment1, moment2, 0.0, 0.0); 
}
