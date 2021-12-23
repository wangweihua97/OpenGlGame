#version 430

layout( triangles_adjacency ) in;
layout( triangle_strip, max_vertices = 18 ) out;

in vec3 VPosition[];

layout(std140 , binding = 0) uniform PV
{
	mat4 projection;
	mat4 view;
};

layout(std140 , binding = 1) uniform BaseLight
{
	vec4 lightDir;
	vec4 color;
	vec4 ambient;
    vec4 lightPos;
};

vec3 LightPosition;

float EPSILON = 0.01;

bool facesLight( vec3 a, vec3 b, vec3 c )
{
  vec3 n = cross( b - a, c - a );
  vec3 da = LightPosition.xyz - a;
  vec3 db = LightPosition.xyz - b;
  vec3 dc = LightPosition.xyz - c;

  return dot(n, da) > 0 || dot(n, db) > 0 || dot(n, dc) > 0; 
}

void emitEdgeQuad( vec3 a, vec3 b ) {
  vec3 LightDir = normalize(a - LightPosition.xyz); 
  vec3 deviation = LightDir * EPSILON;
  gl_Position = projection * vec4(a + deviation, 1);
  EmitVertex();
  
  gl_Position = projection * vec4(LightDir, 0);
  EmitVertex();

  LightDir = normalize(b - LightPosition.xyz); 
  deviation = LightDir * EPSILON;
  gl_Position = projection * vec4(b + deviation, 1);
  EmitVertex();

  gl_Position = projection * vec4(LightDir, 0);
  EmitVertex();
  EndPrimitive();
}

void main()
{
    LightPosition = (view * lightPos).xyz;
    if( facesLight(VPosition[0], VPosition[2], VPosition[4]) ) {
        if( ! facesLight(VPosition[0],VPosition[1],VPosition[2]) ) 
          emitEdgeQuad(VPosition[0],VPosition[2]);
        if( ! facesLight(VPosition[2],VPosition[3],VPosition[4]) ) 
          emitEdgeQuad(VPosition[2],VPosition[4]);
        if( ! facesLight(VPosition[4],VPosition[5],VPosition[0]) ) 
          emitEdgeQuad(VPosition[4],VPosition[0]);

		//FRONT CAP
		vec3 LightDir = normalize(VPosition[0] - LightPosition.xyz); 
		vec3 deviation = LightDir * EPSILON;
		gl_Position = projection * vec4(VPosition[0] + deviation, 1);
		EmitVertex();

		LightDir = normalize(VPosition[2] - LightPosition.xyz); 
		deviation = LightDir * EPSILON;
		gl_Position =  projection * vec4(VPosition[2] + deviation, 1);
		EmitVertex();

		LightDir = normalize(VPosition[4] - LightPosition.xyz); 
		deviation = LightDir * EPSILON;
		gl_Position =  projection * vec4(VPosition[4] + deviation, 1);
		EmitVertex();
	    EndPrimitive();
		
		//BACK CAP
		LightDir = normalize(VPosition[0] - LightPosition.xyz); 
		gl_Position = projection * vec4(LightDir, 0);
		EmitVertex();

		LightDir = normalize(VPosition[4] - LightPosition.xyz); 
		gl_Position =  projection * vec4(LightDir, 0);
		EmitVertex();

		LightDir = normalize(VPosition[2] - LightPosition.xyz); 
		gl_Position =  projection * vec4(LightDir, 0);
		EmitVertex();
	    EndPrimitive();
    }

}