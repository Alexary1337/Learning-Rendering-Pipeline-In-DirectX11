#ifndef _GUI_H_
#define _GUI_H_

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include "SynD3D.h"
#include "SynLight.h"

class SynGUI
{
public:
	SynGUI();
	~SynGUI();

	void Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND);

	void Render(SynD3D*,SynLight*);

private:
	ImVec4 m_diffuseColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 m_ambientColor = ImVec4(0.05f, 0.05f, 0.05f, 1.0f);
	ImVec4 m_specularColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

};

#endif