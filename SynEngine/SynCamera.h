#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Common.h"

class SynCamera
{
public:
	SynCamera();
	SynCamera(const SynCamera&);
	~SynCamera();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);


	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();
	
	void Render();
	void GetViewMatrix(DirectX::XMMATRIX&);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	DirectX::XMMATRIX m_viewMatrix;
};

#endif