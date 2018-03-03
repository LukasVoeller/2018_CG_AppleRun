#version 400

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;

in vec3 Tangent;
in vec3 BiTangent;

out vec4 FragColor;

const int MAX_LIGHTS=14;
struct Light
{
	int Type;
	vec3 Color;
	vec3 Position;
	vec3 Direction;
	vec3 Attenuation;
	vec3 SpotRadius;
	int ShadowIndex;
};

uniform Lights 
{
	int LightCount;
	Light lights[MAX_LIGHTS];
};

uniform vec3 LightPos;
uniform vec3 LightColor;

uniform vec3 EyePos;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;
uniform sampler2D DiffuseTexture;
uniform	sampler2D NormalTexture;

float sat( in float a)
{
    return clamp(a, 0.0, 1.0);
}

void main()
{
	// Tangente, Bitangente + Normale als Matrix
	mat3 m = mat3(Tangent, -BiTangent, Normal);
	// NormalenMap als Textur laden
	vec4 NormalTex = texture( NormalTexture, Texcoord);
	// Normalenvektor aus der NormalenMap laden
	vec3 NormalMap = vec3(2* NormalTex.x -1, 2* NormalTex.y -1, 2* NormalTex.z -1);

	//Textur laden
	vec4 DiffTex = texture( DiffuseTexture, Texcoord);
	if(DiffTex.a <0.3f) discard;

	// FragColor initialisieren mit Ambient-Component
	FragColor = vec4(DiffTex.rgb * AmbientColor, DiffTex.a);
    
    // Normalenvektor durch Anwenden der Matrix auf den Vektor aus der NormalenMap
    vec3 N = normalize((m * NormalMap).xyz);
    vec3 E = normalize(EyePos-Position);
    
    vec3 ColorSum = vec3(0,0,0);

	// Alle Lichtquellen durchlaufen
	for(int i = 0; i<LightCount; ++i)
    {
        vec3 L;
        float Intensity = 1.0;
        
		// Direktionale Lichtquelle
		if (lights[i].Type == 1){
			L = normalize(-lights[i].Direction);	
		}
        // Punkt- oder Spotlight
        else
        {
			L = normalize(lights[i].Position - Position);
            float Dist = distance(lights[i].Position, Position);
            Intensity = 1.0/( lights[i].Attenuation.x + lights[i].Attenuation.y*Dist + lights[i].Attenuation.z*Dist*Dist);
		}

		// Lichtfarbe mit Intensity verrechnen
		vec3 lightIntensity = lights[i].Color * Intensity;

		//Spot-Lichtquellen
		if (lights[i].Type == 2){
			// Radien
			float innerRadius = lights[i].SpotRadius.x;
			float outerRadius = lights[i].SpotRadius.y;

			// Kosinus des Winkels zwischen Direction und Vektor zum Fragment
			float cosinus = dot(L, lights[i].Direction);
			float sigma = acos(-cosinus);

			// Intensity nach Formel
			// li = liVorher * (1-sat((sigma - phiInnen)/(phiAussen - phiInnen)))
			lightIntensity = lightIntensity * (1 - sat((sigma - innerRadius)/(outerRadius - innerRadius)));
		}
		// Halbvektor nach Blinn
		// Nicht (E + L) / 2, da E+L nicht immer normalisiert sind
        vec3 H = normalize(E + L);
        vec3 DiffuseComponent = lightIntensity * DiffuseColor * sat(dot(N,L));
        // Specular Component nach Blinn
        vec3 SpecularComponent = lightIntensity * SpecularColor  * pow( sat(dot(H,N)), SpecularExp);
        FragColor.rgb += (DiffuseComponent * DiffTex.rgb + SpecularComponent);
        
        //Phong
//        vec3 R = reflect(-L,N);
//        vec3 DiffuseComponent = LightColor * DiffuseColor * sat(dot(N,L));
//        vec3 SpecularComponent = LightColor * SpecularColor * pow( sat(dot(R,E)), SpecularExp);
//        FragColor = vec4((DiffuseComponent + AmbientColor)*DiffTex.rgb + SpecularComponent ,DiffTex.a);
	}
}

/*vec4 DiffTex = texture( DiffuseTexture, Texcoord);
    if(DiffTex.a <0.3f) discard;
    vec3 N = normalize(Normal);
    vec3 L = normalize(LightPos-Position);
    vec3 E = normalize(EyePos-Position);
    //vec3 R = reflect(-L,N);
    //Halbvektor
    vec3 H = normalize(E + L);
    vec3 DiffuseComponent = LightColor * DiffuseColor * sat(dot(N,L));
    //vec3 SpecularComponent = LightColor * SpecularColor * pow( sat(dot(R,E)), SpecularExp);
    vec3 SpecularComponent = LightColor * SpecularColor  * pow( sat(dot(H,N)), SpecularExp);
    FragColor = vec4((DiffuseComponent + AmbientColor)*DiffTex.rgb + SpecularComponent ,DiffTex.a);*/
