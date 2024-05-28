#include "DXF.h"

using namespace std;
using namespace DirectX;

class HorizontalBloomShader : public BaseShader
{
public:
	HorizontalBloomShader(ID3D11Device* device, HWND hwnd);
	~HorizontalBloomShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, float screenWidth, float close_f, float dist_f, float close_b, float dist_b,float bloomMultiplier, float brightThres);

private:
	struct ScreenType
	{
		float w_screen;
		XMFLOAT3 cushion;
	};

	struct DepthType
	{
		float close_f;
		float dist_f;
		float close_b;
		float dist_b;
	};

	struct BloomType
	{
		float bloomMultiplier;
		float brightThres;
		XMFLOAT2 cushion;
	};

	void initShader(const wchar_t* vs, const wchar_t* ps);

	ID3D11SamplerState* depthSampleState;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* screenSizeBuffer;
	ID3D11Buffer* depthBloomBuffer;
	ID3D11Buffer* bloomBuffer;

};

