#include "HorizontalBlurShader.h"



HorizontalBlurShader::HorizontalBlurShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"horizontalBlur_vs.cso", L"horizontalBlur_ps.cso");
}


HorizontalBlurShader::~HorizontalBlurShader()
{
	
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	
	if (depthSampleState)
	{
		depthSampleState->Release();
		depthSampleState = 0;
	}

	
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	
	if (screenSizeBuffer)
	{
		screenSizeBuffer->Release();
		screenSizeBuffer = 0;
	}

	
	if (depthBlurBuffer)
	{
		depthBlurBuffer->Release();
		depthBlurBuffer = 0;
	}

	
	BaseShader::~BaseShader();
}

void HorizontalBlurShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC screenBufferDesc;
	D3D11_BUFFER_DESC depthBufferDesc;

	
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	renderer->CreateSamplerState(&samplerDesc, &depthSampleState);

	
	screenBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	screenBufferDesc.ByteWidth = sizeof(ScreenType);
	screenBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	screenBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	screenBufferDesc.MiscFlags = 0;
	screenBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&screenBufferDesc, NULL, &screenSizeBuffer);

	
	depthBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	depthBufferDesc.ByteWidth = sizeof(DepthType);
	depthBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	depthBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	depthBufferDesc.MiscFlags = 0;
	depthBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&depthBufferDesc, NULL, &depthBlurBuffer);
}

void HorizontalBlurShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, float w_screen, float close_f, float dist_f, float close_b, float dist_b)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;


	
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &depthSampleState);

	
	ScreenType* screenSizePtr;
	deviceContext->Map(screenSizeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	screenSizePtr = (ScreenType*)mappedResource.pData;
	screenSizePtr->w_screen = w_screen;
	screenSizePtr->cushion = XMFLOAT3(0.0f, 0.0f, 0.0f);
	deviceContext->Unmap(screenSizeBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &screenSizeBuffer);

	
	DepthType* depthBlurPtr;
	deviceContext->Map(depthBlurBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	depthBlurPtr = (DepthType*)mappedResource.pData;
	depthBlurPtr->close_f = close_f;
	depthBlurPtr->dist_f = dist_f;
	depthBlurPtr->close_b = close_b;
	depthBlurPtr->dist_b = dist_b;
	deviceContext->Unmap(depthBlurBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &depthBlurBuffer);
}