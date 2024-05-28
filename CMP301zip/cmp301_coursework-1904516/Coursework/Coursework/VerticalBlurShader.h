#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;

class VerticalBlurShader : public BaseShader
{
public:
	VerticalBlurShader(ID3D11Device* device, HWND hwnd);
	~VerticalBlurShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, float screenWidth, float close_f, float dist_f, float close_b, float dist_b);

private:
	struct ScreenType
	{
		float h_screen;
		XMFLOAT3 cushion;
	};

	struct DepthType
	{
		float close_f;
		float dist_f;
		float close_b;
		float dist_b;
	};

	void initShader(const wchar_t* vs, const wchar_t* ps);

	ID3D11SamplerState* depthSampleState;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* screenSizeBuffer;
	ID3D11Buffer* depthBlurBuffer;
};
