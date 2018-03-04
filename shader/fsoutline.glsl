#version 400

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord; //phong

out vec4 FragColor;

uniform vec3 LightPos;      // phong
uniform vec3 LightColor;    // phong

uniform vec3 EyePos;
uniform vec3 DiffuseColor;  // phong
uniform vec3 SpecularColor; // phong
uniform vec3 AmbientColor;  // phong
uniform float SpecularExp;  // phong
uniform sampler2D DiffuseTexture;// phong

float sat( in float a)
{
    return clamp(a, 0.0, 1.0);
}

void main()
{
    vec4 DiffTex = texture( DiffuseTexture, Texcoord);
    if(DiffTex.a <0.3f) discard;
    vec3 N = normalize(Normal);
    vec3 L = normalize(LightPos-Position);
    vec3 E = normalize(Position-EyePos);
    vec3 R = reflect(-L,N);
    
    vec3 DiffuseComponent = LightColor * DiffuseColor * sat(dot(N,L));
    vec3 SpecularComponent = LightColor * SpecularColor * pow( sat(dot(R,E)), SpecularExp); //phonf

    float cosAngle = 1 - abs( dot(N,E));

    //Verwendet keine Textur
    //FragColor.rgb = Color(1,0,0)*pow(cosAngle, 4);
    //FragColor.a = 1;
    
    //Textur
    FragColor.rgb = pow(cosAngle, 0.8) * ((DiffuseComponent + AmbientColor) * DiffTex.rgb + SpecularComponent);
    FragColor.a = DiffTex.a;

}

