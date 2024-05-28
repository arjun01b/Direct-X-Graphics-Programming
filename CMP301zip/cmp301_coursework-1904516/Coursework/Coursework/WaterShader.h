#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;


class WaterShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX wrld;
		XMMATRIX pov;
		XMMATRIX predict;
		XMMATRIX dirView;
		XMMATRIX dirPred;
		XMMATRIX SpotlightView;
		XMMATRIX SpotlightProjection;
	};


	struct LightBufferType
	{
		XMFLOAT4 diff_chroma[2];
		XMFLOAT4 amb_chroma[2];
		XMFLOAT4 dir_ill[2];
		XMFLOAT4 intensity_spec[2];
		XMFLOAT4 chroma_spec[2];
	};

	struct CamBufferType
	{
		XMFLOAT3 pos_cam;
		float cushion;
	};

	struct TimeBufferType
	{
		float dt;
		float height;
		float vel;
		float hz;

	};

public:
	WaterShader(ID3D11Device* device, HWND hwnd);
	~WaterShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light[2], float dt, float height, float hz, float vel, ID3D11ShaderResourceView* depthMap, ID3D11ShaderResourceView* depthMap2, FPCamera* camera, const XMMATRIX& lightViewMatrix, const XMMATRIX& lightProjMatrix, const XMMATRIX& SpotlightViewMatrix, const XMMATRIX& SpotlightProjMatrix);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* camBuffer;


};