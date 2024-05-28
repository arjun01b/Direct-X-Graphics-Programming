// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"


#include "TextureShader.h"
#include "ShadowShader.h"
#include "DepthShader.h"
#include"HorizontalBloomShader.h"
#include"VerticalBloomShader.h"
#include"VerticalBlurShader.h"
#include"HorizontalBlurShader.h"
#include"HManip.h"
#include"ManipWater.h"
#include"WaterShader.h"
#include"HShader.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();
private:
	HShader* hShader;
	WaterShader* waterShader;
	TextureShader* textureShader;
	
	HorizontalBloomShader* horBloom;
	VerticalBloomShader* vertBloom;
    
	ManipWater* man_w;
	HorizontalBlurShader* hor;
	VerticalBlurShader* vert;
	Light* ill[2];
	XMFLOAT4 Spotdiffuse; XMFLOAT4 Spotambient; XMFLOAT4 Spotspecular; float SpotspecularPower;
	XMFLOAT4 diffuse; XMFLOAT4 ambient; XMFLOAT4 specular; float specularPower;
	float brightThresh;
	float bloomMutiplier;



	RenderTexture* shadow_tex;
	RenderTexture* hor_tex;
	RenderTexture* vert_tex;
	RenderTexture* tex_ult;

	PlaneMesh* map_h;
	PlaneMesh* water;
	Model* cottage;
	Model* teapot;

	float clk;
	bool foggy;
	bool bloomy;
	OrthoMesh* orthoMesh;

	ShadowShader* shadowShader;
	DepthShader* depthShader;
	ShadowMap* map_s;
	ShadowMap* map_s_sec;
	ShadowMap* fade;
	HManip* man_h;

protected:
	bool render();
	void passPrimD();
	void passSecD();
	void passShadow();
	void passUlt();
	void gui();

	void effBlur();
	void blurHor();
	void blurVert();
	void bloomHor();
	void bloomVert();


};

#endif