#include "shadowshader.h"


ShadowShader::ShadowShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"shadow_vs.cso", L"shadow_ps.cso");
}


ShadowShader::~ShadowShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
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
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	
	BaseShader::~BaseShader();
}


void ShadowShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC camBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);

	
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	
	camBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	camBufferDesc.ByteWidth = sizeof(CamBufferType);
	camBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	camBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	camBufferDesc.MiscFlags = 0;
	camBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&camBufferDesc, NULL, &camBuffer);

}


void ShadowShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, Light* light[2], ID3D11ShaderResourceView* depthMap, ID3D11ShaderResourceView* depthMap2, FPCamera* camera, const XMMATRIX& lightViewMatrix, const XMMATRIX& lightProjMatrix, const XMMATRIX& SpotlightViewMatrix, const XMMATRIX& SpotlightProjMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	
	XMMATRIX worldTran = XMMatrixTranspose(worldMatrix);
	XMMATRIX viewTran = XMMatrixTranspose(viewMatrix);
	XMMATRIX predTran = XMMatrixTranspose(projectionMatrix);
	XMMATRIX tLightViewMatrix = XMMatrixTranspose(lightViewMatrix);
	XMMATRIX tLightProjectionMatrix = XMMatrixTranspose(lightProjMatrix);
	XMMATRIX tspotLightView = XMMatrixTranspose(SpotlightViewMatrix);
	XMMATRIX tspotLightProj = XMMatrixTranspose(SpotlightProjMatrix);

	
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->wrld = worldTran;
	dataPtr->pov = viewTran;
	dataPtr->predict = predTran;
	dataPtr->dirView = tLightViewMatrix;
	dataPtr->dirPred = tLightProjectionMatrix;
	dataPtr->SpotlightView = tspotLightView;
	dataPtr->SpotlightProjection = tspotLightProj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	CamBufferType* camptr;
	deviceContext->Map(camBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	camptr = (CamBufferType*)mappedResource.pData;
	camptr->cameraPosition = camera->getPosition();
	deviceContext->Unmap(camBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &camBuffer);

	
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	for (int i = 0; i < 2; i++)
	{
		lightPtr->lightDirection[i] = XMFLOAT4(light[i]->getDirection().x, light[i]->getDirection().y, light[i]->getDirection().z, 1.0);
		lightPtr->diffuseColour[i] = light[i]->getDiffuseColour();
		lightPtr->ambientColour[i] = light[i]->getAmbientColour();
		lightPtr->specularColour[i] = light[i]->getSpecularColour();
		lightPtr->specularPower[i].x = light[i]->getSpecularPower();
	}
	lightPtr->specularPower[1].y = cosf(60 * 0.0174533);
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMap);
	deviceContext->PSSetShaderResources(2, 1, &depthMap2);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
}

