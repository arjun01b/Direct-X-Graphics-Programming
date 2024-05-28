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
    float4 int_spec[2];
    float4 chroma_spec[2];
};

struct InputType
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 nor : NORMAL;
    float3 vec_pov : TEXCOORD1;
    float4 dir_pos : TEXCOORD2;
    float4 lightViewPos1 : TEXCOORD3;
    float3 worldPosition : TEXCOORD4;

};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 int_calc(float3 dir_ill, float3 nor, float4 diff)
{
    float power = saturate(dot(nor, dir_ill));
    float4 chroma = saturate(diff * power);
    return chroma;
}

float4 calcSpecular(float3 lightDir, float3 nor, float3 vec_pov, float4 chroma_spec, float int_spec)
{
    // blinn-phong specular calculation
    float3 mid = normalize(lightDir + vec_pov);
    float spec_int = pow(max(dot(nor, mid), 0.0f), int_spec);
    return saturate(chroma_spec * spec_int);
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
    textureColour = texture0.Sample(sampler0, (input.tex * 10));

    // Calculate the projected texture coordinates.
    float2 pTexCoord1 = getProjectiveCoords(input.dir_pos);
    float2 pTexCoord2 = getProjectiveCoords(input.lightViewPos1);


    float4 dirColor = float4(0.f, 0.f, 0.f, 1.f);
    float4 spotColor = float4(0.f, 0.f, 0.f, 1.f);

    float3 spotlightPosition = float3(50.f, 20.f, 50.f);

    float3 lightVector = normalize(spotlightPosition - input.worldPosition);


    float angle = dot(normalize(-dir_ill[1].xyz), lightVector);

    {
        float4 chroma_spec1 = calcSpecular(normalize(-dir_ill[0].xyz), input.nor, input.vec_pov, chroma_spec[0], int_spec[0].x);
        dirColor = int_calc(-dir_ill[0].xyz, input.nor, chroma_diff[0]);
        if (angle > int_spec[1].y)
        {
            float4 chroma_spec2 = calcSpecular(normalize(-dir_ill[1].xyz), input.nor, input.vec_pov, chroma_spec[1], int_spec[1].x);
            spotColor = int_calc(-dir_ill[1].xyz, input.nor, chroma_diff[1]);
        }
        else
        {
            spotColor = chroma_amb[1];
        }
        lightColour = lerp(dirColor, spotColor, 0.5);
    }
    // Shadow test. Is or isn't in shadow
    if (!hasDepthData(pTexCoord1) || isInShadow(depthMapTexture, pTexCoord1, input.dir_pos, shadowMapBias))
    {

        lightColour -= float4(0.3, 0.3, 0.3, 1);
    }

     if (!hasDepthData(pTexCoord2) || isInShadow(depthMapTexture1, pTexCoord2, input.lightViewPos1, shadowMapBias))
     {

        lightColour -= float4(0.3, 0.3, 0.3, 1);
    }


    float4 finalambient = lerp(chroma_amb[0], chroma_amb[1], 0.5);


    lightColour = saturate(lightColour + finalambient);
    return saturate(lightColour) * textureColour;
}
