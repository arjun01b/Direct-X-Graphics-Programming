// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

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
    float3 pos_cam;
    
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
    float3 vec_pov : TEXCOORD1;
    float4 dir_pos : TEXCOORD2;
    float4 lightViewPos2 : TEXCOORD3;
    float3 worldPosition : TEXCOORD4;
};


OutputType main(InputType input)
{
    OutputType output;
    #define HEIGHT 20
	#define OFFSET 1.0f
	#define UV_OFFSET (1/185)
	
    float4 amp, norAmpA, norAmpB;
   
    float2 textSec, textTer;


    amp = texture0.SampleLevel(sampler0, input.tex, 0);
    input.pos.y = (amp.x * HEIGHT);


	textSec.x = input.tex.x + 0.01;
	textSec.y = input.tex.y;
	norAmpA = texture0.SampleLevel(sampler0, textSec, 0);

	textTer.x = input.tex.x;
	textTer.y = input.tex.y + 0.01;
	norAmpB = texture0.SampleLevel(sampler0, textTer, 0);

	input.nor.x = input.pos.y - (norAmpA.x * HEIGHT);
	input.nor.z = input.pos.y - (norAmpB.x * HEIGHT);
	input.nor.y = 1;


	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.pos = mul(input.pos, worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
    output.nor = mul(input.nor, (float3x3) worldMatrix);
    output.nor = normalize(output.nor);
	
	// Calculate the position of the vertice as viewed by the light source.
	output.dir_pos = mul(input.pos, worldMatrix);
	output.dir_pos = mul(output.dir_pos, lightViewMatrix);
	output.dir_pos = mul(output.dir_pos, lightProjectionMatrix);
    
    output.lightViewPos2 = mul(input.pos, worldMatrix);
    output.lightViewPos2 = mul(output.lightViewPos2, SpotlightViewMatrix);
    output.lightViewPos2 = mul(output.lightViewPos2, SpotlightProjectionMatrix);
    
    float4 worldPosition = mul(input.pos, worldMatrix);
    output.vec_pov = pos_cam.xyz - worldPosition.xyz;
    output.vec_pov = normalize(output.vec_pov);

    output.worldPosition = mul(input.pos, worldMatrix).xyz;
	
    return output;
}