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
	m_ambientColor = DirectX::XMFLOAT4(red, green, blue, alpha);
	return;
}

void SynLight::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = DirectX::XMFLOAT4(red, green, blue, alpha);
	return;
}

void SynLight::SetDirection(float x, float y, float z)
{
	m_direction = DirectX::XMFLOAT3(x, y, z);
	return;
}

void SynLight::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = DirectX::XMFLOAT4(red, green, blue, alpha);
	return;
}

void SynLight::SetSpecularPower(float power)
{
	m_specularPower = power;
	return;
}

DirectX::XMFLOAT4 SynLight::GetAmbientColor()
{
	return m_ambientColor;
}

DirectX::XMFLOAT4 SynLight::GetDiffuseColor()
{
	return m_diffuseColor;
}

DirectX::XMFLOAT3 SynLight::GetDirection()
{
	return m_direction;
}

DirectX::XMFLOAT4 SynLight::GetSpecularColor()
{
	return m_specularColor;
}

float SynLight::GetSpecularPower()
{
	return m_specularPower;
}