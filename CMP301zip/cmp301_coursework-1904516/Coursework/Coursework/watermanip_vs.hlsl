// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer TimeBuffer : register(b1)
{
	float dt;
	float height;
	float vel;
	float hz;
}

struct InputType
{
	float4 pos : POSITION;
	
};

struct OutputType
{
	float4 pos : SV_POSITION;
	float4 pos_manip : TEXCOORD0;
};

float compute(float a, float b)
{
	float j = height * sin(hz * ((pow(a, 2) / 2.34) + pow((exp(-a)), 2) + pow(b, 8 / 5) + (dt * vel)));

	return j;
}

OutputType main(InputType input)
{
	OutputType output;
	input.pos.y = compute(input.pos.a, input.pos.b);
	

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.pos = mul(input.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);

	
	output.pos_manip = output.pos;

	return output;
}