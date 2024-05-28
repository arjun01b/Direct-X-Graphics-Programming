#include "WaterShader.h"

WaterShader::WaterShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"water_vs.cso", L"water_ps.cso");
}


WaterShader::~WaterShader()
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

	if (timeBuffer)
	{
		timeBuffer->Release();
		timeBuffer = 0;
	}

	
	BaseShader::~BaseShader();
}


void WaterShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC timeBufferDesc;
	D3D11_BUFFER_DESC camBufferDesc;

	
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

	
	timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timeBufferDesc.ByteWidth = sizeof(TimeBufferType);
	timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timeBufferDesc.MiscFlags = 0;
	timeBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&timeBufferDesc, NULL, &timeBuffer);

	
	camBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	camBufferDesc.ByteWidth = sizeof(CamBufferType);
	camBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	camBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	camBufferDesc.MiscFlags = 0;
	camBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&camBufferDesc, NULL, &camBuffer);
}


void WaterShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, Light* light[2], float dt, float height, float hz, float vel, ID3D11ShaderResourceView* depthMap, ID3D11ShaderResourceView* depthMap2, FPCamera* camera, const XMMATRIX& lightViewMatrix, const XMMATRIX& lightProjMatrix, const XMMATRIX& SpotlightViewMatrix, const XMMATRIX& SpotlightProjMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj, tLightView, tLightProj, tspotLightView, tspotLightProj;


	
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	tLightView = XMMatrixTranspose(lightViewMatrix);
	tLightProj = XMMatrixTranspose(lightProjMatrix);
	tspotLightView = XMMatrixTranspose(SpotlightViewMatrix);
	tspotLightProj = XMMatrixTranspose(SpotlightProjMatrix);


	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->wrld = tworld;
	dataPtr->pov = tview;
	dataPtr->predict = tproj;
	dataPtr->dirView = tLightView;
	dataPtr->dirPred = tLightProj;
	dataPtr->SpotlightView = tspotLightView;
	dataPtr->SpotlightProjection = tspotLightProj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;

	int j = 0;
	while (j < 2)
	{
		lightPtr->dir_ill[j] = XMFLOAT4(light[j]->getDirection().x, light[j]->getDirection().y, light[j]->getDirection().z, 1.0);
		lightPtr->diff_chroma[j] = light[j]->getDiffuseColour();
		lightPtr->amb_chroma[j] = light[j]->getAmbientColour();
		lightPtr->chroma_spec[j] = light[j]->getSpecularColour();
		lightPtr->intensity_spec[j].x = light[j]->getSpecularPower();
		j++;;
	}
	lightPtr->intensity_spec[1].y = cosf(60 * 0.0174533);

	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	
	TimeBufferType* timeptr;
	deviceContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timeptr = (TimeBufferType*)mappedResource.pData;
	timeptr->dt = dt;
	timeptr->height = height;
	timeptr->hz = hz;
	timeptr->vel = vel;
	deviceContext->Unmap(timeBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &timeBuffer);


	CamBufferType* camptr;
	deviceContext->Map(camBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	camptr = (CamBufferType*)mappedResource.pData;
	camptr->pos_cam = camera->getPosition();
	deviceContext->Unmap(camBuffer, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &camBuffer);

	
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMap);
	deviceContext->PSSetShaderResources(2, 1, &depthMap2);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
}