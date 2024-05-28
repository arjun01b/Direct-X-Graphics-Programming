
#include "App1.h"

#define CLOSE_B 0.859f
#define DIST_B 0.990f
#define CLOSE_F 0.930f
#define DIST_F 0.945f


#define HEIGHT 0.150f
#define FREQ 2.543f
#define VEL 0.8f




#define COTTAGE_TRAN 49, 3, 49
#define COTTAGE_SIZE 0.49, 0.49, 0.49
#define TEA_TRAN 179, 29, 179
#define TEA_SIZE 0.29, 0.29, 0.29

App1::App1()
{
	water = nullptr;
	waterShader = nullptr;
	ill[0] = nullptr;
	ill[1] = nullptr;
	map_h = nullptr;
	hShader = nullptr;
	orthoMesh = nullptr;
	map_s = nullptr;
	cottage = nullptr;
	teapot = nullptr;
	textureShader = nullptr;
	depthShader = nullptr;
	man_w = nullptr;
	man_h = nullptr;
	shadowShader = nullptr;
	map_s_sec = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	textureMgr->loadTexture(L"pool", L"res/water.jpg");
	textureMgr->loadTexture(L"land", L"res/land.png");
	textureMgr->loadTexture(L"height", L"res/height.png");
	textureMgr->loadTexture(L"cottage", L"res/cottage.png");
	textureMgr->loadTexture(L"teapot", L"res/teapottex.jpg");

	
	int width_s = 4999;
	int height_s = 4999;
	int width_w = 99;
	int height_w = 99;

	horBloom = new HorizontalBloomShader(renderer->getDevice(), hwnd);
	vertBloom = new VerticalBloomShader(renderer->getDevice(), hwnd);


	hor = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	vert = new VerticalBlurShader(renderer->getDevice(), hwnd);

	cottage = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/cottage.obj");
	teapot = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/teapot.obj");

	water = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	waterShader = new WaterShader(renderer->getDevice(), hwnd);


	map_h = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	hShader = new HShader(renderer->getDevice(), hwnd);


	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	man_w = new ManipWater(renderer->getDevice(), hwnd);
	man_h = new HManip(renderer->getDevice(), hwnd);


	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);


	
	shadow_tex = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	hor_tex = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	vert_tex = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	tex_ult = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);


	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);
	map_s = new ShadowMap(renderer->getDevice(), width_s, height_s);
	map_s_sec = new ShadowMap(renderer->getDevice(), width_s, height_s);
	fade = new ShadowMap(renderer->getDevice(), screenWidth, screenHeight);


	
	//Light properties 1
	diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	ambient = { 0.3f, 0.3f, 0.3f, 1.0f };
	specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	specularPower = 128.0f;
	ill[0] = new Light;
	ill[0]->setAmbientColour(ambient.x, ambient.y, ambient.z, ambient.w);
	ill[0]->setDiffuseColour(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
	ill[0]->setSpecularColour(specular.x, specular.y, specular.z, specular.w);
	ill[0]->setSpecularPower(specularPower);
	ill[0]->setDirection(0.7f, -0.7f, 0.f);
	ill[0]->setPosition(-10.f, 20.f, 50.f);
	ill[0]->generateOrthoMatrix((float)width_w, (float)height_w, 0.1f, 100.f);


	//Light properties 2
	Spotdiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	Spotambient = { 0.3f, 0.3f, 0.3f, 1.0f };
	Spotspecular = { 1.0f, 1.0f, 1.0f, 1.0f };
	SpotspecularPower = 128.0f;
	ill[1] = new Light;
	ill[1]->setAmbientColour(Spotambient.x, Spotambient.y, Spotambient.z, Spotambient.w);
	ill[1]->setDiffuseColour(Spotdiffuse.x, Spotdiffuse.y, Spotdiffuse.z, Spotdiffuse.w);
	ill[1]->setSpecularColour(Spotspecular.x, Spotspecular.y, Spotspecular.z, Spotspecular.w);
	ill[1]->setSpecularPower(SpotspecularPower);
	ill[1]->setDirection(0.0f, -0.7f, 0.f);
	ill[1]->setPosition(50.f, 20.f, 50.f);
	ill[1]->generateOrthoMatrix((float)width_w, (float)height_w, 0.1f, 100.f);


	bloomMutiplier = 1.0f;
	brightThresh = 0.8f;
	foggy = false;
	bloomy = false;
	clk = 0.f;
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	


	if (man_w)
	{
		delete man_w;
		man_w = 0;
	}

	if (man_h)
	{
		delete man_h;
		man_h = 0;
	}

	if (water)
	{
		delete water;
		water = 0;
	}

	if (waterShader)
	{
		delete waterShader;
		waterShader = 0;
	}

	if (ill)
	{
		delete ill;
		ill[0] = 0;
		ill[1] = 0;
	}
	if (map_h)
	{
		delete map_h;
		map_h = 0;
	}

	if (hShader)
	{
		delete hShader;
		hShader = 0;
	}

	if (orthoMesh)
	{
		delete orthoMesh;
		orthoMesh = 0;
	}

	if (map_s)
	{
		delete map_s;
		map_s = 0;
	}
	if (map_s_sec)
	{
		delete map_s_sec;
		map_s_sec = 0;
	}

	if (cottage)
	{
		delete cottage;
		cottage = 0;
	}

	if (teapot)
	{
		delete teapot;
		teapot = 0;
	}

	if (textureShader)
	{
		delete textureShader;
		textureShader = 0;
	}

	if (depthShader)
	{
		delete depthShader;
		depthShader = 0;
	}


	if (shadowShader)
	{
		delete shadowShader;
		shadowShader = 0;
	}
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}



void App1::passPrimD()
{
	
	map_s->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext()); //


	ill[0]->generateViewMatrix();
	XMMATRIX lightViewMatrix = ill[0]->getViewMatrix();
	XMMATRIX lightProjectionMatrix = ill[0]->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	
	worldMatrix = renderer->getWorldMatrix(); //Water
	water->sendData(renderer->getDeviceContext());
	man_w->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"pool"), ill[0], clk, HEIGHT, FREQ, VEL);
	man_w->render(renderer->getDeviceContext(), water->getIndexCount());

	
	worldMatrix = XMMatrixTranslation(0.f, -2.86, 0.f); //HeightMap
	map_h->sendData(renderer->getDeviceContext());
	man_h->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"land"), ill[0], textureMgr->getTexture(L"height"));
	man_h->render(renderer->getDeviceContext(), map_h->getIndexCount());


	
	worldMatrix = XMMatrixTranslation(COTTAGE_TRAN); //cottage
	XMMATRIX scaleMatrix = XMMatrixScaling(COTTAGE_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	cottage->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), cottage->getIndexCount());

	
	worldMatrix = XMMatrixTranslation(TEA_TRAN); //Teapot
	scaleMatrix = XMMatrixScaling(TEA_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	teapot->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), teapot->getIndexCount());


	
	renderer->setBackBufferRenderTarget(); //Buffer is target
	renderer->resetViewport();
}
void App1::passSecD()
{
	
	map_s_sec->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext()); //

	
	ill[1]->generateViewMatrix(); //light
	XMMATRIX lightViewMatrix = ill[1]->getViewMatrix();
	XMMATRIX lightProjectionMatrix = ill[1]->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	
	worldMatrix = renderer->getWorldMatrix(); //Water
	water->sendData(renderer->getDeviceContext());
	man_w->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"pool"), ill[1], clk, HEIGHT, FREQ, VEL);
	man_w->render(renderer->getDeviceContext(), water->getIndexCount());

	
	worldMatrix = XMMatrixTranslation(0.f, -2.86, 0.f); //heightmap
	map_h->sendData(renderer->getDeviceContext());
	man_h->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"land"), ill[1], textureMgr->getTexture(L"height"));
	man_h->render(renderer->getDeviceContext(), map_h->getIndexCount());


	
	worldMatrix = XMMatrixTranslation(COTTAGE_TRAN); //cottage
	XMMATRIX scaleMatrix = XMMatrixScaling(COTTAGE_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	cottage->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), cottage->getIndexCount());

	
	worldMatrix = XMMatrixTranslation(TEA_TRAN); //teapot
	scaleMatrix = XMMatrixScaling(TEA_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	teapot->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), teapot->getIndexCount());


	
	renderer->setBackBufferRenderTarget(); //backbuffer
	renderer->resetViewport();
}

void App1::passShadow()
{
	
	shadow_tex->setRenderTarget(renderer->getDeviceContext());
	shadow_tex->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	
	ill[0]->generateViewMatrix();
	ill[1]->generateViewMatrix();

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	XMMATRIX lightViewMatrix = ill[0]->getViewMatrix();
	XMMATRIX lightProjectionMatrix = ill[0]->getOrthoMatrix();
	XMMATRIX spotlightViewMatrix = ill[1]->getViewMatrix();
	XMMATRIX spotlightProjectionMatrix = ill[1]->getOrthoMatrix();

	
	worldMatrix = renderer->getWorldMatrix(); //Heightmap
	water->sendData(renderer->getDeviceContext());
	waterShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"pool"), ill, clk, HEIGHT, FREQ, VEL, map_s->getDepthMapSRV(), map_s_sec->getDepthMapSRV(), camera, lightViewMatrix, lightProjectionMatrix, spotlightViewMatrix, spotlightProjectionMatrix);
	waterShader->render(renderer->getDeviceContext(), water->getIndexCount());

	
	worldMatrix = XMMatrixTranslation(0, -2.86, 0); //Water
	map_h->sendData(renderer->getDeviceContext());
	hShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"land"), ill, textureMgr->getTexture(L"height"), map_s->getDepthMapSRV(), map_s_sec->getDepthMapSRV(), camera, lightViewMatrix, lightProjectionMatrix, spotlightViewMatrix, spotlightProjectionMatrix);
	hShader->render(renderer->getDeviceContext(), map_h->getIndexCount());


	
	worldMatrix = XMMatrixTranslation(COTTAGE_TRAN); //Cottage
	XMMATRIX scaleMatrix = XMMatrixScaling(COTTAGE_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	cottage->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"cottage"), ill, map_s->getDepthMapSRV(), map_s_sec->getDepthMapSRV(), camera, lightViewMatrix, lightProjectionMatrix, spotlightViewMatrix, spotlightProjectionMatrix);
	shadowShader->render(renderer->getDeviceContext(), cottage->getIndexCount());

	
	worldMatrix = XMMatrixTranslation(TEA_TRAN); //Teapot
	scaleMatrix = XMMatrixScaling(TEA_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	teapot->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"teapot"), ill, map_s->getDepthMapSRV(), map_s_sec->getDepthMapSRV(), camera, lightViewMatrix, lightProjectionMatrix, spotlightViewMatrix, spotlightProjectionMatrix);
	shadowShader->render(renderer->getDeviceContext(), teapot->getIndexCount());


	
	renderer->setBackBufferRenderTarget(); //Back buffer as target
	renderer->resetViewport();
}


//PP
void App1::effBlur()
{
	
	fade->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext()); //Render to texture

	
	XMMATRIX worldMatrix = renderer->getWorldMatrix(); //Matrices from objects and cam
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	
	worldMatrix = renderer->getWorldMatrix(); //Water
	water->sendData(renderer->getDeviceContext());
	man_w->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"pool"), ill[0], clk, HEIGHT, FREQ, VEL);
	man_w->render(renderer->getDeviceContext(), water->getIndexCount());

	
	worldMatrix = XMMatrixTranslation(0.f, -2.86, 0.f); //HeightMap
	map_h->sendData(renderer->getDeviceContext());
	man_h->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"land"), ill[0], textureMgr->getTexture(L"height"));
	man_h->render(renderer->getDeviceContext(), map_h->getIndexCount());


	
	worldMatrix = XMMatrixTranslation(COTTAGE_TRAN); // Cottage
	XMMATRIX scaleMatrix = XMMatrixScaling(COTTAGE_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	cottage->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	depthShader->render(renderer->getDeviceContext(), cottage->getIndexCount());

	
	worldMatrix = XMMatrixTranslation(TEA_TRAN); //Teapot
	scaleMatrix = XMMatrixScaling(TEA_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	teapot->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	depthShader->render(renderer->getDeviceContext(), teapot->getIndexCount());

	
	renderer->setBackBufferRenderTarget(); //back buffer
	renderer->resetViewport();
}


void App1::blurHor()
{
	float screenSizeX = (float)hor_tex->getTextureWidth();

	hor_tex->setRenderTarget(renderer->getDeviceContext());
	hor_tex->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX baseViewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = hor_tex->getOrthoMatrix();

	
	renderer->setZBuffer(false); // Render Vertical Blur
	orthoMesh->sendData(renderer->getDeviceContext());
	hor->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, shadow_tex->getShaderResourceView(), fade->getDepthMapSRV(), screenSizeX, CLOSE_F, DIST_F, CLOSE_B, DIST_B);
	hor->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	
	renderer->setBackBufferRenderTarget();  //back to original back buffer
}


void App1::blurVert()
{
	float screenSizeY = (float)vert_tex->getTextureHeight();

	vert_tex->setRenderTarget(renderer->getDeviceContext());
	vert_tex->clearRenderTarget(renderer->getDeviceContext(), 0.40f, 0.59f, 0.93f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX baseViewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = vert_tex->getOrthoMatrix();

	
	renderer->setZBuffer(false); //Render vertical blur
	orthoMesh->sendData(renderer->getDeviceContext());
	vert->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, hor_tex->getShaderResourceView(), fade->getDepthMapSRV(), screenSizeY, CLOSE_F, DIST_F, CLOSE_B, DIST_B);
	vert->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	
	renderer->setBackBufferRenderTarget(); //Back to original back buffer
}

void App1::bloomHor() //TODO: USE DIFFERENT TEXTURE THAN BLUR
{
	float screenSizeX = (float)hor_tex->getTextureWidth();

	hor_tex->setRenderTarget(renderer->getDeviceContext());
	hor_tex->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX baseViewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = hor_tex->getOrthoMatrix();


	renderer->setZBuffer(false); // Render Vertical Blur
	//TODO: Pass bloom amount and brightness thershold to shader
	orthoMesh->sendData(renderer->getDeviceContext());
    horBloom->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, shadow_tex->getShaderResourceView(), fade->getDepthMapSRV(), screenSizeX, CLOSE_F, DIST_F, CLOSE_B, DIST_B,bloomMutiplier,brightThresh);
	horBloom->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);


	renderer->setBackBufferRenderTarget();  //back to original back buffer
}

void App1::bloomVert() //TODO: USE DIFFERENT TEXTURE THAN BLUR
{
	float screenSizeY = (float)vert_tex->getTextureHeight();

	vert_tex->setRenderTarget(renderer->getDeviceContext());
	vert_tex->clearRenderTarget(renderer->getDeviceContext(), 0.40f, 0.59f, 0.93f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX baseViewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = vert_tex->getOrthoMatrix();


	renderer->setZBuffer(false); //Render vertical blur
	//TODO: Pass bloom amount and brightness thershold to shader
	orthoMesh->sendData(renderer->getDeviceContext());
	vertBloom->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, hor_tex->getShaderResourceView(), fade->getDepthMapSRV(), screenSizeY, CLOSE_F, DIST_F, CLOSE_B, DIST_B,bloomMutiplier,brightThresh);
	vertBloom->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);


	renderer->setBackBufferRenderTarget(); //Back to original back buffer
}


void App1::passUlt()
{
	//Render texture scen
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  //2d render
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix(); //position of camera
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	renderer->setZBuffer(false);


	//wireframe mode
	if (foggy || bloomy)
		tex_ult = vert_tex;
	else
		tex_ult = shadow_tex;
	if (!wireframeToggle) {
		orthoMesh->sendData(renderer->getDeviceContext());
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, tex_ult->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
		renderer->setZBuffer(true);
	}

	else
	{
		//Render water
		worldMatrix = renderer->getWorldMatrix();
		water->sendData(renderer->getDeviceContext());
		man_w->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"pool"), ill[0], clk, HEIGHT, FREQ, VEL);
		man_w->render(renderer->getDeviceContext(), water->getIndexCount());

		// Render HeightMap
		worldMatrix = XMMatrixTranslation(0.f, -2.86, 0.f);
		map_h->sendData(renderer->getDeviceContext());
		man_h->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"land"), ill[0], textureMgr->getTexture(L"height"));
		man_h->render(renderer->getDeviceContext(), map_h->getIndexCount());


		//Render cottage
		worldMatrix = XMMatrixTranslation(COTTAGE_TRAN);
		XMMATRIX scaleMatrix = XMMatrixScaling(COTTAGE_SIZE);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
		cottage->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
		depthShader->render(renderer->getDeviceContext(), cottage->getIndexCount());

		//Teapot render
		worldMatrix = XMMatrixTranslation(TEA_TRAN);
		scaleMatrix = XMMatrixScaling(TEA_SIZE);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
		teapot->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
		depthShader->render(renderer->getDeviceContext(), teapot->getIndexCount());
	}



	
	gui();
}


bool App1::render()
{
	ill[0]->setAmbientColour(ambient.x, ambient.y, ambient.z, ambient.w);
	ill[0]->setDiffuseColour(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
	ill[0]->setSpecularColour(specular.x, specular.y, specular.z, specular.w);
	ill[0]->setSpecularPower(specularPower);

	ill[1]->setAmbientColour(Spotambient.x, Spotambient.y, Spotambient.z, Spotambient.w);
	ill[1]->setDiffuseColour(Spotdiffuse.x, Spotdiffuse.y, Spotdiffuse.z, Spotdiffuse.w);
	ill[1]->setSpecularColour(Spotspecular.x, Spotspecular.y, Spotspecular.z, Spotspecular.w);
	ill[1]->setSpecularPower(SpotspecularPower);

	
	renderer->beginScene(0.0f, 0.0f, 0.0f, 1.0f);

	camera->update();

	clk += timer->getTime();

	passPrimD();
	passSecD();
	passShadow();

	if (foggy)
	{
		effBlur();

		blurHor();

		blurVert();

		
	}

	if (bloomy)
	{
		effBlur();

		bloomHor();
		
		bloomVert();

	}


	passUlt();

	
	renderer->endScene();

	return true;
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	//directional
	float fdiffuse[3];
	fdiffuse[0] = diffuse.x;
	fdiffuse[1] = diffuse.y;
	fdiffuse[2] = diffuse.z;

	float fambient[3];
	fambient[0] = ambient.x;
	fambient[1] = ambient.y;
	fambient[2] = ambient.z;

	float fspecular[3];
	fspecular[0] = specular.x;
	fspecular[1] = specular.y;
	fspecular[2] = specular.z;

	//spotlight
	float fSdiffuse[3];
	fSdiffuse[0] = Spotdiffuse.x;
	fSdiffuse[1] = Spotdiffuse.y;
	fSdiffuse[2] = Spotdiffuse.z;

	float fSambient[3];
	fSambient[0] = Spotambient.x;
	fSambient[1] = Spotambient.y;
	fSambient[2] = Spotambient.z;

	float fSspecular[3];
	fSspecular[0] = Spotspecular.x;
	fSspecular[1] = Spotspecular.y;
	fSspecular[2] = Spotspecular.z;



	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::Checkbox("Blur mode", &foggy);
	ImGui::Checkbox("Bloom mode", &bloomy);
	if (bloomy==true)
	{

		ImGui::SliderFloat("Brightness Threshold", &brightThresh, 0.f, 1.f);
		ImGui::SliderFloat("Bloom Intensity", &bloomMutiplier, 0.f, 5.f);


	}
	if (ImGui::CollapsingHeader("Directional Light Settings"))
	{
		ImGui::ColorEdit3("Diffuse", fdiffuse);
		ImGui::ColorEdit3("Ambient", fambient);
		ImGui::ColorEdit3("Specular", fspecular);
		ImGui::SliderFloat("Specular Power", &specularPower, 1.f, 150.f);
	}

	if (ImGui::CollapsingHeader("Spot Light Settings"))
	{
		ImGui::ColorEdit3("Diffuse", fSdiffuse);
		ImGui::ColorEdit3("Ambient", fSambient);
		ImGui::ColorEdit3("Specular", fSspecular);
		ImGui::SliderFloat("Specular Power", &SpotspecularPower, 1.f, 150.f);
	}




	diffuse.x = fdiffuse[0];
	diffuse.y = fdiffuse[1];
	diffuse.z = fdiffuse[2];

	ambient.x = fambient[0];
	ambient.y = fambient[1];
	ambient.z = fambient[2];

	specular.x = fspecular[0];
	specular.y = fspecular[1];
	specular.z = fspecular[2];

	Spotdiffuse.x = fSdiffuse[0];
	Spotdiffuse.y = fSdiffuse[1];
	Spotdiffuse.z = fSdiffuse[2];

	Spotambient.x = fSambient[0];
	Spotambient.y = fSambient[1];
	Spotambient.z = fSambient[2];

	Spotspecular.x = fSspecular[0];
	Spotspecular.y = fSspecular[1];
	Spotspecular.z = fSspecular[2];

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

