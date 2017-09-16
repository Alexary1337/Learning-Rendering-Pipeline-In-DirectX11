#include "synlight.h"

SynLight::SynLight()
{

}

SynLight::SynLight(const SynLight& other)
{

}

SynLight::~SynLight()
{

}

void SynLight::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void SynLight::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void SynLight::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x, y, z);
	return;
}

void SynLight::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void SynLight::SetSpecularPower(float power)
{
	m_specularPower = power;
	return;
}

D3DXVECTOR4 SynLight::GetAmbientColor()
{
	return m_ambientColor;
}

D3DXVECTOR4 SynLight::GetDiffuseColor()
{
	return m_diffuseColor;
}

D3DXVECTOR3 SynLight::GetDirection()
{
	return m_direction;
}

D3DXVECTOR4 SynLight::GetSpecularColor()
{
	return m_specularColor;
}

float SynLight::GetSpecularPower()
{
	return m_specularPower;
}