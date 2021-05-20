#version 400

in vec2 texCoord ; // the same out name in vertex shader
in vec4 FragWorldPos ;
in vec3 NormalWorld ;

uniform vec3 drawingColor ;  // glColor3f


struct Light {
   vec3 position ;  // point light source position in world coordinate system.
   vec3 ambient  ;
   vec3 color    ;  // diffuse light property
   vec3 specular ;
} ;

uniform Light light ;

struct Material {
    sampler2D diffuse ;
    sampler2D alpha ;
    sampler2D specular ;
    float shiny ;
} ;

uniform Material material ;
uniform vec3 cameraPos ;

out vec4 fragColor ;

uniform bool specularEnable ; 


void main() {
  if ( texture(material.alpha, texCoord).r > 0.9) discard ; // skip the current fragment as if it doesn't exist
  // fragColor = vec4(drawingColor, 1.0) * (vec4(1.0) - texture(diff, texCoord)); // texture(diff, texCoord) ; 

  // 1. Ambient component
  vec3 ambientColor = light.ambient * texture(material.diffuse, texCoord).rgb ;

  // 2. Diffuse component
  vec3 L = normalize(light.position - FragWorldPos.xyz) ;
  vec3 N = normalize(NormalWorld) ;
  float diffCoeff = max(dot(N,L), 0.0) ;
  vec3 diffuseColor = light.color * diffCoeff * texture(material.diffuse, texCoord).rgb ;

  // 3. Specular component
  vec3 V = normalize(cameraPos - FragWorldPos.xyz) ;
  vec3 R = reflect(-L, N) ;
  float specCoeff = pow(max(dot(V,R),0), material.shiny) ;
  vec3 specularColor = specCoeff * light.specular ;

  // apply specular mapping ?
  if ( specularEnable ) {
     specularColor *= texture(material.specular, texCoord).rgb ;
  }

  // Color of the fragment
  fragColor = vec4(ambientColor + diffuseColor + specularColor , 1.0) ; 
  
}