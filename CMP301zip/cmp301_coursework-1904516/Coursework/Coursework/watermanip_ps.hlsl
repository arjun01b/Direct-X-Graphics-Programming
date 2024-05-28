struct InputType
{

	float4 pos_manip : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
	float manip_val;

	manip_val = input.pos_manip.z / input.pos_manip.w;
	return float4(manip_val, manip_val, manip_val, 1.0f);

}