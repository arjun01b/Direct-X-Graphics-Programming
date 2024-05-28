struct InputType
{

	float4 pos_man : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
	float val_man;
// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
val_man = input.pos_man.z / input.pos_man.w;
return float4(val_man, val_man, val_man, 1.0f);

}