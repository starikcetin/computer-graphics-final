#version 400
layout(location=0) in vec3 aPos ;   // aPos is attribute 0
layout(location=1) in vec3 aNorm ;  // aNorm is attribute 1 ( normal vector in model coordinate system)
layout(location=2) in vec2 aTex ;   // aTex is attribute 2

uniform mat4 proj ;
uniform mat4 view ;
uniform mat4 model ;

uniform float angle ;
uniform float time ;
uniform float maxOffset ;

out vec2 texCoord ;
out vec4 FragWorldPos ;
out vec3 NormalWorld ;

uniform bool animEnable ;

void main() {
 vec3 newPos = aPos ;

 if(animEnable) {
	newPos.x = sin(time / 10.0) * maxOffset + aPos.x ; // non-linear transformation
 }

 FragWorldPos = model * vec4(newPos, 1.0) ;  // vertex in world coordinate. ( since we calculate light color in world coordinate)
 NormalWorld = mat3(transpose(inverse(model))) * aNorm ;  // transformed normal vector in world coordinate system.
  gl_Position =  proj * view * model * vec4(newPos, 1.0)  ; 
  texCoord = aTex ;
}