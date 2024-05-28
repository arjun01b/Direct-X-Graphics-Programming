// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
    matrix SpotlightViewMatrix;
    matrix SpotlightProjectionMatrix;

};

cbuffer CamBuffer : register(b1)
{
    float3 cameraPosition;
    float padding;
}

struct InputType
{
    float4 pos : POSITION;
    float2 tex : TEXCOORD0;
    float3 nor : NORMAL;
};

struct OutputType
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 nor : NORMAL;
    float3 viewVector : TEXCOORD1;
    float4 dirViewPos : TEXCOORD2;
    float4 lightViewPos2 : TEXCOORD3;
    float3 worldPosition : TEXCOORD4;
};


OutputType main(InputType input)
{
    OutputType output;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.pos = mul(input.pos, worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    // Calculate the position of the vertice as viewed by the light source.
    output.dirViewPos = mul(input.pos, worldMatrix);
    output.dirViewPos = mul(output.dirViewPos, lightViewMatrix);
    output.dirViewPos = mul(output.dirViewPos, lightProjectionMatrix);

    output.lightViewPos2 = mul(input.pos, worldMatrix);
    output.lightViewPos2 = mul(output.lightViewPos2, SpotlightViewMatrix);
    output.lightViewPos2 = mul(output.lightViewPos2, SpotlightProjectionMatrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    // Calculate the normal vector against the world matrix only and normalise.
    output.nor = mul(input.nor, (float3x3) worldMatrix);
    output.nor = normalize(output.nor);


    float4 worldPosition = mul(input.pos, worldMatrix);
    output.viewVector = cameraPosition.xyz - worldPosition.xyz;
    output.viewVector = normalize(output.viewVector);

    output.worldPosition = mul(input.pos, worldMatrix).xyz;

    return output;
}
