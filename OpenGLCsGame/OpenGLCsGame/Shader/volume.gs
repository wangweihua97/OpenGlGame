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

vec3 LightPosition = (view * lightPos).xyz;

float EPSILON = 0.01;

/*bool facesLight( vec3 a, vec3 b, vec3 c )
{
  LightPosition = (view * lightPos).xyz;
  vec3 n = cross( b - a, c - a );
  vec3 da = LightPosition.xyz - a;
  vec3 db = LightPosition.xyz - b;
  vec3 dc = LightPosition.xyz - c;

  return dot(n, da) > 0 || dot(n, db) > 0 || dot(n, dc) > 0; 
}

void emitEdgeQuad( vec3 a, vec3 b ) {
  LightPosition = (view * lightPos).xyz;
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
}*/
struct sVSOutput
{         
    vec3 WorldPos;                                                                 
};
void EmitQuad(int StartIndex, sVSOutput StartVertex, int EndIndex, sVSOutput EndVertex)
{
    vec3 LightDir = normalize(StartVertex.WorldPos - lightPos.xyz);
    vec3 l = LightDir * EPSILON;
    gl_Position = projection * view * vec4((StartVertex.WorldPos + l), 1.0);
    EmitVertex();
    
    gl_Position = projection * view * vec4(LightDir, 0.0);
    EmitVertex();

    LightDir = normalize(EndVertex.WorldPos - lightPos.xyz);
    l = LightDir * EPSILON;
    gl_Position = projection * view * vec4((EndVertex.WorldPos + l), 1.0);
    EmitVertex();
    
    gl_Position = projection * view * vec4(LightDir, 0.0);
    EmitVertex();

    EndPrimitive();            
}



void main()
{
	/*gl_Position = projection * view * vec4(VPosition[0] ,1.0);
    EmitVertex();
    gl_Position = projection * view * vec4(VPosition[1] ,1.0);
    EmitVertex();
    gl_Position = projection * view * vec4(VPosition[2] ,1.0);
    EmitVertex();
    EndPrimitive();*/
    vec3 e1 = VPosition[2] - VPosition[0];
    vec3 e2 = VPosition[4] - VPosition[0];
    vec3 e3 = VPosition[1] - VPosition[0];
    vec3 e4 = VPosition[3] - VPosition[2];
    vec3 e5 = VPosition[4] - VPosition[2];
    vec3 e6 = VPosition[5] - VPosition[0];

    
    vec3 Normal = cross(e1,e2);
    vec3 LightDir = lightPos.xyz - VPosition[0];

    if (dot(Normal, LightDir) > 0.000001) {

        struct sVSOutput StartVertex, EndVertex;

        Normal = cross(e3,e1);

        if (dot(Normal, LightDir) <= 0) {
            StartVertex.WorldPos = VPosition[0];
            EndVertex.WorldPos   = VPosition[2];
            EmitQuad(0, StartVertex, 2, EndVertex);
        }

        Normal = cross(e4,e5);
        LightDir = lightPos.xyz - VPosition[2];

        if (dot(Normal, LightDir) <= 0) {
            StartVertex.WorldPos = VPosition[2];
            EndVertex.WorldPos   = VPosition[4];
            EmitQuad(2, StartVertex, 4, EndVertex);
        }

        Normal = cross(e2,e6);
        LightDir = lightPos.xyz - VPosition[4];

        if (dot(Normal, LightDir) <= 0) {
            StartVertex.WorldPos = VPosition[4];
            EndVertex.WorldPos   = VPosition[0];
            EmitQuad(4, StartVertex, 0, EndVertex);
        }

        vec3 LightDir = (normalize(VPosition[0] - lightPos.xyz)) * EPSILON;
        gl_Position = projection * view * vec4((VPosition[0] + LightDir), 1.0);
        EmitVertex();

        LightDir = (normalize(VPosition[2] - lightPos.xyz)) * EPSILON;
        gl_Position = projection * view * vec4((VPosition[2] + LightDir), 1.0);
        EmitVertex();

        LightDir = (normalize(VPosition[4] - lightPos.xyz)) * EPSILON;
        gl_Position = projection * view * vec4((VPosition[4] + LightDir), 1.0);
        EmitVertex();
        EndPrimitive();
    }
}