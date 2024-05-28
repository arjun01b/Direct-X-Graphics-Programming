#ifndef _SHADOWSHADER_H_
#define _SHADOWSHADER_H_

#include "DXF.h"

using namespace std;
using namespace DirectX;


class ShadowShader : public BaseShader
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
		XMFLOAT4 diffuseColour[2];
		XMFLOAT4 ambientColour[2];
		XMFLOAT4 lightDirection[2];
		XMFLOAT4 specularPower[2];
		XMFLOAT4 specularColour[2];
	};

	struct CamBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};
public:

	ShadowShader(ID3D11Device* device, HWND hwnd);
	~ShadowShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light[2], ID3D11ShaderResourceView* depthMap, ID3D11ShaderResourceView* depthMap2, FPCamera* camera, const XMMATRIX& lightViewMatrix, const XMMATRIX& lightProjMatrix, const XMMATRIX& SpotlightViewMatrix, const XMMATRIX& SpotlightProjMatrix);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* camBuffer;
};

#endif