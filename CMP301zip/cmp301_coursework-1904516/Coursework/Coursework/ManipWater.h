#pragma once


#include"DXF.h"

using namespace std;
using namespace DirectX;


class ManipWater : public BaseShader
{
private:


	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};

	struct TimeBufferType
	{
		float dt;
		float height;
		float vel;
		float hz;
	};

public:
	ManipWater(ID3D11Device* device, HWND hwnd);
	~ManipWater();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light, float deltaTime, float amplitude, float freq, float speed);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;


};

