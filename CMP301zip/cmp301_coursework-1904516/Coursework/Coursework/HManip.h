#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;


class HManip : public BaseShader
{
private:


	struct MatrixBufferType
	{
		XMMATRIX wrld;
		XMMATRIX pov;
		XMMATRIX prediction;

	};

public:
	HManip(ID3D11Device* device, HWND hwnd);
	~HManip();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* tex, Light* ill, ID3D11ShaderResourceView* heightmap);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
};
