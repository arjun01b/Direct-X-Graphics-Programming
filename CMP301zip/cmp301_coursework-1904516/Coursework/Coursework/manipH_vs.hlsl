Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};



struct InputType
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;

};

struct OutputType
{
	float4 pos : SV_POSITION;
	float4 pos_man : TEXCOORD0;
};


OutputType main(InputType input)
{
	OutputType output;
#define AMPLITUDE 20
#define OFFSET 1.0f
#define UV_OFFSET (1/185)

	float4 mea;




	mea = texture0.SampleLevel(sampler0, input.tex, 0);
	input.pos.y = (mea.x * AMPLITUDE);



	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.pos = mul(input.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.pos_man = output.pos;

	return output;
}