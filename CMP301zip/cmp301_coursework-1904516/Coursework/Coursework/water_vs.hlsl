// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
//Texture2D texture0 : register(t0);
//SamplerState sampler0 : register(s0);

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

cbuffer TimeBuffer : register(b1)
{
	float dt;
	float height;
	float vel;
	float hz;
}

cbuffer CamBuffer : register(b2)
{
	float3 pos_cam;

}

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewVector : TEXCOORD1;
	float4 dir_ill : TEXCOORD2;
	float4 lightViewPos1 : TEXCOORD3;
	float3 worldPosition : TEXCOORD4;
};

float calc(float x, float z)
{
	float val = height * sin(hz * ((pow(x, 2) / 2.34) + pow((exp(-x)), 2) + pow(z, 8 / 5) + (dt * vel)));

	return val;
}

OutputType main(InputType input)
{
#define OFFSET 1.0f
	OutputType output;
	// find co-ordinates
	float3 top, right, bottom, left;
	// manipulate vertex co-ords
	input.position.y = calc(input.position.x, input.position.z);

	top = float4(input.position.x, calc(input.position.x, (input.position.z + OFFSET)), (input.position.z + OFFSET), input.position.w);
	right = float4((input.position.x + OFFSET), calc((input.position.x + OFFSET), input.position.z), input.position.z, input.position.w);
	bottom = float4(input.position.x, calc(input.position.x, (input.position.z - OFFSET)), (input.position.z - OFFSET), input.position.w);
	left = float4((input.position.x - OFFSET), calc((input.position.x - OFFSET), input.position.z), input.position.z, input.position.w);

	// calculate vectors
	float3 mid_top, mid_right, mid_bottom, mid_left;

	mid_top = normalize(top - input.position);
	mid_right = normalize(right - input.position);
	mid_bottom = normalize(bottom - input.position);
	mid_left = normalize(left - input.position);

	// calculate cross product
	float3 ne_cr, es_cr, sw_cr, wn_cr;

	ne_cr = cross(mid_top, mid_right);

	es_cr = cross(mid_right, mid_bottom);

	sw_cr = cross(mid_bottom, mid_left);

	wn_cr = cross(mid_left, mid_top);

	// normalize
	float3 nor1, nor2, nor3, nor4;
	nor1 = normalize(ne_cr);
	nor2 = normalize(es_cr);
	nor3 = normalize(sw_cr);
	nor4 = normalize(wn_cr);

	// calculate average
	float3 nor_mean;
	//average_normal = (crossNE + crossES + crossSW + crossWN) / 4;
	nor_mean = (nor1 + nor2 + nor3 + nor4) / 4;

	//set normals
	//input.normal.x = average_normal.x;
	//input.normal.y = average_normal.y;
	input.normal = nor_mean;

	// Calculate the position of the vertice as viewed by the light source.
	output.dir_ill = mul(input.position, worldMatrix);
	output.dir_ill = mul(output.dir_ill, lightViewMatrix);
	output.dir_ill = mul(output.dir_ill, lightProjectionMatrix);

	output.lightViewPos1 = mul(input.position, worldMatrix);
	output.lightViewPos1 = mul(output.lightViewPos1, SpotlightViewMatrix);
	output.lightViewPos1 = mul(output.lightViewPos1, SpotlightProjectionMatrix);

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
	output.normal = mul(input.normal, (float3x3) worldMatrix);
	output.normal = normalize(output.normal);

	float4 worldPosition = mul(input.position, worldMatrix);
	output.viewVector = pos_cam.xyz - worldPosition.xyz;
	output.viewVector = normalize(output.viewVector);

	output.worldPosition = mul(input.position, worldMatrix).xyz;

	return output;
}