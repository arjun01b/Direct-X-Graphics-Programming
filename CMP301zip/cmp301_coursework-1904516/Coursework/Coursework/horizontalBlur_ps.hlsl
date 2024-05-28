Texture2D sceneTexture : register(t0);
Texture2D depthTexture : register(t1);
SamplerState texSampler : register(s0);
SamplerState depthSampler : register(s1);

cbuffer ScreenSizeBuffer : register(b0)
{
	float w_screen;

}

cbuffer DepthBlurBuffer : register(b1)
{
	float close_f;
	float dist_f;
	float close_b;
	float dist_b;
}

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};


float4 main(InputType input) : SV_TARGET
{
	float fath = depthTexture.Sample(depthSampler, input.tex).r;
	int comp_blur = 0;

	if (close_f <= fath <= dist_f)
	{
		comp_blur = 0;
	}
	else if (fath < close_f)
	{
		float inst = (close_f - close_b) / 4.0f;

		comp_blur = 5;
		int j = 0;
		while (j < 4)
		{
			float low = close_f - (inst * (j + 1));
			float high = close_f - (inst * j);

			if (low < fath < high)
			{
				comp_blur = (j + 1);
			}
			j++;
		}
	}
	else if (fath > dist_f)
	{
		float inst = (dist_b - dist_f) / 4.0f;

		comp_blur = 5;

		int j = 0;
		while (j < 4)
		{
			float low = dist_f + (inst * j);
			float high = dist_f + (inst * (j + 1));

			if (low < fath < high)
			{
				comp_blur = (j + 1);
			}
			j++;
		}
	}

	float4 chroma;
	float mass[7];
	switch (comp_blur)
	{
		case 0:
			chroma = sceneTexture.Sample(texSampler, input.tex);
			break;
		case 1:
			mass[0] = 0.62518632f;
			mass[1] = 0.174616f;
			mass[2] = 0.0016547f;
			mass[3] = 0.0000162f;
			mass[4] = 0.0f;
			mass[5] = 0.0f;
			mass[6] = 0.0f;
			break;
		case 2:
			mass[0] = 0.3746845f;
			mass[1] = 0.2715642f;
			mass[2] = 0.05482f;
			mass[3] = 0.007f;
			mass[4] = 0.0004f;
			mass[5] = 0.00009f;
			mass[6] = 0.0f;
			break;
		case 3:
			mass[0] = 0.248615f;
			mass[1] = 0.216613f;
			mass[2] = 0.124648f;
			mass[3] = 0.0549f;
			mass[4] = 0.00646f;
			mass[5] = 0.001777f;
			mass[6] = 0.000085f;
			break;
		case 4:
			mass[0] = 0.200155f;
			mass[1] = 0.165484f;
			mass[2] = 0.120f;
			mass[3] = 0.08f;
			mass[4] = 0.024072f;
			mass[5] = 0.009068f;
			mass[6] = 0.002202f;
			break;
		case 5:
			mass[0] = 0.17f;
			mass[1] = 0.139476f;
			mass[2] = 0.12717f;
			mass[3] = 0.080380f;
			mass[4] = 0.046f;
			mass[5] = 0.020174f;
			mass[6] = 0.010099f;
			break;
		default:
			mass[0] = 1.0f;
			mass[1] = 0.0f;
			mass[2] = 0.0f;
			mass[3] = 0.0f;
			mass[4] = 0.0f;
			mass[5] = 0.0f;
			mass[6] = 0.0f;
			break;
	}


	chroma = float4(0.0f, 0.0f, 0.0f, 0.0f);


	float texel_mea = 1.0f / w_screen;

	int j = -6;
	while (j < 7)
	{
		int b = j;
		if (b < 0)
			b = -j;

		chroma += sceneTexture.Sample(texSampler, input.tex + float2(texel_mea * j, 0.0f)) * mass[b];
		j++;
	}


	chroma.a = 1.0f;

	return chroma;
}