#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 viewPos;

uniform int textureFlag;

uniform vec3 ambientLight;
uniform vec3 lightDirection;
uniform vec3 lightSourceColor;
uniform float lightSourceShiness;

uniform vec3 ambientModel;
uniform vec3 diffuseModel;
uniform vec3 specularModel;

float ShadowCalculation(vec4 fragPosLightSpace)
{

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);

    vec3 lightDir = normalize(-lightDirection);

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;

}

void main()
{           

    vec3 normal = normalize(fs_in.Normal);

    vec3 ambient = ambientLight * ambientModel;

    vec3 lightDir = normalize(-lightDirection);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * diffuseModel;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), lightSourceShiness);
    vec3 specular = spec * specularModel;    

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      

    vec3 Fragmentcolor = ambient + (1.0 - shadow) * ( diffuse + specular ) * lightSourceColor;

    if (textureFlag == 1 || textureFlag == 2 || textureFlag == 3){
        Fragmentcolor = Fragmentcolor * texture(diffuseTexture, fs_in.TexCoords).rgb;
    }

    FragColor = vec4(Fragmentcolor, 1.0);

}
