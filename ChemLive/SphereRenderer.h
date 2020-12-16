#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "ShaderStructures.h"
#include <pplawait.h>

namespace DirectX
{
	class SphereRenderer
	{
	public:
		SphereRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		concurrency::task<void> CreateDeviceDependentResourcesAsync();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();

	private:
		void LoadVertexShader(const std::vector<byte>& fileData);
		void LoadPixelShader(const std::vector<byte>& fileData);
		void CreateSphere();
		void Rotate(float radians);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		winrt::com_ptr<ID3D11Buffer>		m_vertexBuffer;
		winrt::com_ptr<ID3D11Buffer>		m_constantBuffer;
		winrt::com_ptr<ID3D11Buffer>		m_indexBuffer;
		winrt::com_ptr<ID3D11InputLayout>	m_inputLayout;
		winrt::com_ptr<ID3D11VertexShader>	m_vertexShader;
		winrt::com_ptr<ID3D11PixelShader>	m_pixelShader;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32_t	m_indexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		bool	m_tracking;
		float	m_degreesPerSecond;
		double	m_lastTime;
		double	m_xPos;
	};
}