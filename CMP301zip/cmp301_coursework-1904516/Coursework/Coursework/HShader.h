#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;


class HShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 chroma_diff[2];
		XMFLOAT4 chroma_amb[2];
		XMFLOAT4 dir_ill[2];
		XMFLOAT4 spec_int[2];
		XMFLOAT4 spec_chroma[2];
	};
	struct CamBufferType
	{
		XMFLOAT3 pos_cam;
		float cushion;
	};
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
		XMMATRIX SpotlightView;
		XMMATRIX SpotlightProjection;
	};

public:
	HShader(ID3D11Device* device, HWND hwnd);
	~HShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light[2], ID3D11ShaderResourceView* heightmap, ID3D11ShaderResourceView* depthMap, ID3D11ShaderResourceView* depthMap2, FPCamera* camera, const XMMATRIX& lightViewMatrix, const XMMATRIX& lightProjMatrix, const XMMATRIX& SpotlightViewMatrix, const XMMATRIX& SpotlightProjMatrix);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* camBuffer;
	ID3D11Buffer* lightBuffer;
};


