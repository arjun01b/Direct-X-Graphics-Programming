Texture2D texture0 : register(t0);
Texture2D depthMapTexture : register(t1);
Texture2D depthMapTexture1 : register(t2);

SamplerState sampler0 : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
    float4 diffuseColour[2];
    float4 ambientColour[2];
    float4 lightDirection[2];
    float4 specularPower[2];
    float4 specularColour[2];
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewVector : TEXCOORD1;
    float4 lightViewPos : TEXCOORD2;
    float4 lightViewPos1 : TEXCOORD3;
    float3 worldPosition : TEXCOORD4;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

float4 calcSpecular(float3 lightDir, float3 normal, float3 viewVector, float4 specularColour, float specularPower)
{
	// blinn-phong specular calculation
    float3 halfway = normalize(lightDir + viewVector);
    float specularIntensity = pow(max(dot(normal, halfway), 0.0f), specularPower);
    return saturate(specularColour * specularIntensity);
}
// Is the gemoetry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
	// Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}


float4 main(InputType input) : SV_TARGET
{
    float depthValue;
    float lightDepthValue;
    float shadowMapBias = 0.005f;
    float4 textureColour;
    float4 lightColour = float4(0.f, 0.f, 0.f, 1.f);

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    textureColour = texture0.Sample(sampler0, input.tex);

	// Calculate the projected texture coordinates.
    float2 pTexCoord1 = getProjectiveCoords(input.lightViewPos);
    float2 pTexCoord2 = getProjectiveCoords(input.lightViewPos1);
	
	
    float4 dirColor = float4(0.f, 0.f, 0.f, 1.f);
    float4 spotColor = float4(0.f, 0.f, 0.f, 1.f);

    float3 spotlightPosition = float3(50.f, 20.f, 50.f);
	
    float3 lightVector = normalize(spotlightPosition - input.worldPosition);
	

    float angle = dot(normalize(-lightDirection[1].xyz), lightVector);
	
    {
        float4 specularColour1 = calcSpecular(normalize(-lightDirection[0].xyz), input.normal, input.viewVector, specularColour[0], specularPower[0].x);
        dirColor = calculateLighting(-lightDirection[0].xyz, input.normal, diffuseColour[0]);
        if (angle > specularPower[1].y)
        {
            float4 specularColour2 = calcSpecular(normalize(-lightDirection[1].xyz), input.normal, input.viewVector, specularColour[1], specularPower[1].x);
            spotColor = calculateLighting(-lightDirection[1].xyz, input.normal, diffuseColour[1]);
        }
        else
        {
            spotColor = ambientColour[1];
        }
        lightColour = lerp(dirColor, spotColor, 0.5);
    }
    
    // Shadow test. Is or isn't in shadow
    if (!hasDepthData(pTexCoord1) || isInShadow(depthMapTexture, pTexCoord1, input.lightViewPos, shadowMapBias))
    {

        lightColour -= float4(0.3, 0.3, 0.3, 1);
    }

    if (!hasDepthData(pTexCoord2) || isInShadow(depthMapTexture1, pTexCoord2, input.lightViewPos1, shadowMapBias))
    {

        lightColour -= float4(0.3, 0.3, 0.3, 1);
    }
    
    float4 resultambient = lerp(ambientColour[0], ambientColour[1], 0.5);

    
    lightColour = saturate(lightColour + resultambient);
    return saturate(lightColour) * textureColour;
}
