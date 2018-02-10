//
//  OutlineShader.cpp
//  CGXcode
//
//  Created by Sandra Tieben on 10.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#include "OutlineShader.h"
#include "Constants.h"

//OutlineShader::OutlineShader(const std::string& vs, const std::string& fs) : VSFile(vs), FSFile(fs), Time(0)
//{
//	bool loaded = load(vs.c_str(), fs.c_str());
//	if(!loaded)
//		throw std::exception();
//	
//	ModelViewProjMat = getParameterID("ModelViewProjMat");
//	Eye = getParameterID("EyePos");
//	ModelMat = getParameterID("ModelMat");
//	NormalMat = getParameterID("NormalMat");
//	DiffuseTex = getParameterID("DiffuseTex");
//	Timer = getParameterID("Timer");
//}

OutlineShader::OutlineShader(bool LoadStaticShaderCode)
{
	if(!LoadStaticShaderCode)
		return;
	//ShaderProgram = createShaderProgram(VertexShaderCode, FragmentShaderCode);
	bool loaded = load(SHADER_DIRECTORY"vsoutline.glsl", SHADER_DIRECTORY"fsoutline.glsl");
	if (!loaded)
		throw std::exception();
	assignLocations();
	Timer = getParameterID("Timer");
	NormalMat = getParameterID("NormalMat");
}

OutlineShader::~OutlineShader()
{
	
}

void OutlineShader::activate(const BaseCamera& Cam) const
{
	BaseShader::activate(Cam);
	
	//Update uniforms if necessary
	if(UpdateState&DIFF_COLOR_CHANGED)
		glUniform3f(DiffuseColorLoc, DiffuseColor.R, DiffuseColor.G, DiffuseColor.B);
	if(UpdateState&AMB_COLOR_CHANGED)
		glUniform3f(AmbientColorLoc, AmbientColor.R, AmbientColor.G, AmbientColor.B);
	if(UpdateState&SPEC_COLOR_CHANGED)
		glUniform3f(SpecularColorLoc, SpecularColor.R, SpecularColor.G, SpecularColor.B);
	if(UpdateState&SPEC_EXP_CHANGED)
		glUniform1f(SpecularExpLoc, SpecularExp);
	
	Matrix MVP;;
	MVP = modelTransform();
	MVP = Cam.getViewMatrix() * MVP;
	MVP = Cam.getProjectionMatrix() * MVP;
	
	setParameter(ModelViewProjLoc, MVP);
	setParameter(ModelMatLoc, modelTransform());
	Matrix NormMat = modelTransform();
	
	NormMat.m03 = NormMat.m13 = NormMat.m23 = 0;
	NormMat.transpose();
	NormMat.invert();
	setParameter(NormalMat, NormMat);
	setParameter(EyePosLoc, Cam.position());
	setParameter(DiffuseTexLoc, 0);
	setParameter(Timer, Time);
	
	int TexSlotIdx = 0;
	DiffuseTexture->activate(TexSlotIdx++);
	if(UpdateState&DIFF_TEX_CHANGED && DiffuseTexture)
		glUniform1i(DiffuseTexLoc, TexSlotIdx-1);
	NormalTexture->activate(TexSlotIdx++);
	if (UpdateState&NORM_TEX_CHANGED && NormalTexture)
		glUniform1i(NormalTexLoc, TexSlotIdx-1);

	if(UpdateState&LIGHT_COLOR_CHANGED)
		glUniform3f(LightColorLoc, LightColor.R, LightColor.G, LightColor.B);
	if(UpdateState&LIGHT_POS_CHANGED)
		glUniform3f(LightPosLoc, LightPos.X, LightPos.Y, LightPos.Z);

	//Always update matrices
	Matrix ModelViewProj = Cam.getProjectionMatrix() * Cam.getViewMatrix() * modelTransform();
	glUniformMatrix4fv(ModelMatLoc, 1, GL_FALSE, modelTransform().m);
	glUniformMatrix4fv(ModelViewProjLoc, 1, GL_FALSE, ModelViewProj.m);

	Vector EyePos = Cam.position();
	glUniform3f(EyePosLoc, EyePos.X, EyePos.Y, EyePos.Z );

	for (int i = 0; i < MaxLightCount; ++i)
	{
		if (ShadowMapTexture[i] && (ShadowMapMatLoc[i] != -1))
		{
			ShadowMapTexture[i]->activate(TexSlotIdx);
			setParameter(ShadowMapTextureLoc[i], TexSlotIdx++);
			setParameter(ShadowMapMatLoc[i], ShadowMapMat[i]);
		}
	}

	UpdateState = 0x0;
	
}

