#version 400 core

in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

out vec4 vOutputColour;		// The output colour

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

struct SpotLightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	vec3 direction;
	float exponent;
	float cutoff;
	bool isOn;
};


float rho = 0.01f;
vec3 fogColour = vec3(0.5f);
uniform bool isFoggy;

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

// Lights and materials passed in as uniform variables from client programme
uniform SpotLightInfo spotLight1;
uniform LightInfo light1; 
uniform MaterialInfo material1; 

uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
in vec3 worldPosition;

in vec4 p;
in vec3 n;

// This function implements the Phong shading model
// The code is based on the OpenGL 4.0 Shading Language Cookbook, Chapter 2, pp. 62 - 63, with a few tweaks. 
// Please see Chapter 2 of the book for a detailed discussion.
vec3 BlinnPhongModel(LightInfo light, vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(light.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 h = normalize(v + s);
	vec3 n = eyeNorm;
	vec3 ambient = light.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = light.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = light.Ls * material1.Ms * pow(max(dot(h, n), 0.0f), material1.shininess + eps);
	

	return ambient + diffuse + specular;

}

// Implementation taken from lab 6 code.
vec3 BlinnPhongSpotlightModel(SpotLightInfo spotlight, vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(spotlight.position - p));
	float angle = acos(dot(-s, spotlight.direction));
	float cutoff = radians(clamp(spotlight.cutoff, 0.0, 90.0));
	vec3 ambient = spotlight.La * material1.Ma;
	if (angle < cutoff) {
		float spotFactor = pow(dot(-s, spotlight.direction), spotlight.exponent);
		vec3 v = normalize(-p.xyz);
		vec3 h = normalize(v + s);
		float sDotN = max(dot(s, n), 0.0);
		vec3 diffuse = spotlight.Ld * material1.Md * sDotN;
		vec3 specular = vec3(0.0);
		if (sDotN > 0.0)
			specular = spotlight.Ls * material1.Ms * pow(max(dot(h, n), 0.0),
			material1.shininess);
		return ambient + spotFactor * (diffuse + specular);
	} else
			return ambient;
}



void main()
{

	vec3 vColour;

	// if spotlight.isOn is true, the spotlight is rendered.
	if(spotLight1.isOn)
	{
		vColour = BlinnPhongModel(light1, p, n) + BlinnPhongSpotlightModel(spotLight1,p,n);
	}
	else
	{
		vColour = BlinnPhongModel(light1, p, n);
	}

	if (renderSkybox) {
		vOutputColour = texture(CubeMapTex, worldPosition);

	} else {

		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, vTexCoord);	

		if (bUseTexture)
			vOutputColour = vTexColour * vec4(vColour, 1.0f);	// Combine object colour and texture 
		else
			vOutputColour = vec4(vColour, 1.0f);	// Just use the colour instead
	}

	// fog implementation taken from lecture 8.
	if(isFoggy)
	{
		float d = length( p.xyz );
		float w = exp(-rho*d);
		vOutputColour.rgb = mix(fogColour,vOutputColour.rgb, w);
	}
	
	
}
