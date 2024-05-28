// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D texture0 : register(t0);
Texture2D depthMapTexture : register(t1);
Texture2D depthMapTexture1 : register(t2);
SamplerState sampler0 : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
	float4 chroma_diff[2];
	float4 chroma_amb[2];
	float4 dir_ill[2];
	float4 specularPower[2];
	float4 specularColour[2];
};

struct InputType
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewVector : TEXCOORD1;
	float4 pos_ill : TEXCOORD2;
    float4 lightViewPos1 : TEXCOORD3;
    float3 worldPosition : TEXCOORD4;

};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 compLight(float3 dir_ill, float3 nor, float4 diff)
{
	float power = saturate(dot(nor, dir_ill));
	float4 chroma = saturate(diff * power);
	return chroma;
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
	
	
	float partShadowMap = 0.005f;
	float4 chroma_tex;
	

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
	chroma_tex = texture0.Sample(sampler0, (input.tex * 10));

	// Calculate the projected texture coordinates.
    float2 pPrim = getProjectiveCoords(input.pos_ill);
    float2 pSec = getProjectiveCoords(input.lightViewPos1);
	
	
    float4 dirColor = float4(0.f, 0.f, 0.f, 1.f);
    float4 spotColor = float4(0.f, 0.f, 0.f, 1.f);

    float3 spotlightPosition = float3(50.f, 20.f, 50.f);
	
    float3 lightVector = normalize(spotlightPosition - input.worldPosition);
	

    float angle = dot(normalize(-dir_ill[1].xyz), lightVector);
	
    float4 colour;
    {
        float4 specularColour1 = calcSpecular(normalize(-dir_ill[0].xyz), input.normal, input.viewVector, specularColour[0], specularPower[0].x);
        dirColor = compLight(-dir_ill[0].xyz, input.normal, chroma_diff[0]);
        if (angle >specularPower[1].y)
        {
            float4 specularColour2 = calcSpecular(normalize(-dir_ill[1].xyz), input.normal, input.viewVector, specularColour[1], specularPower[1].x);
            spotColor = compLight(-dir_ill[1].xyz, input.normal, chroma_diff[1]);
        }
        else
        {
            spotColor = chroma_amb[1];
        }
        colour = lerp(dirColor, spotColor, 0.5);
    }
	// Shadow test. Is or isn't in shadow
    if (!hasDepthData(pPrim) || isInShadow(depthMapTexture, pPrim, input.pos_ill, partShadowMap))
    {

        colour -= float4(0.3, 0.3, 0.3, 1);
    }
    if (angle > specularPower[1].y)
    {
        if (!hasDepthData(pSec) || isInShadow(depthMapTexture1, pSec, input.lightViewPos1, partShadowMap))
        {

            colour -= float4(0.3, 0.3, 0.3, 1);
        }
    }

    float4 finalambient = lerp(chroma_amb[0], chroma_amb[1], 0.5);

    
    colour = saturate(colour + finalambient);
    return saturate(colour) * chroma_tex;
}

