#pragma once

#include "pch.h"
#include "Atom.h"
#include "DeviceResources.h"
#include "HLSLStructures.h"
#include "StepTimer.h"
#include <pplawait.h>

using namespace DirectX;

namespace Simulation
{
	class SimulationRenderer
	{
	public:
		SimulationRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources, XMFLOAT3 boxDimensions);
		concurrency::task<void> CreateDeviceDependentResourcesAsync(XMFLOAT3 boxDimensions);
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Render(const std::vector<Atom*> &atoms, XMFLOAT3 boxDimensions);

		void UpdateBoxDimensions(XMFLOAT3 newBoxDimensions);		// Update the eye location if box dimensions change

	private:
		void LoadVertexShader(const std::vector<byte>& fileData);
		void LoadPixelShader(const std::vector<byte>& fileData);

		void UpdateViewMatrix();
		void CreateSphere();
		void CreateBox(XMFLOAT3 boxDimensions);
		void DrawBox();



		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources; 

		// Direct3D resources for cube geometry.
		winrt::com_ptr<ID3D11Buffer>		m_vertexBuffer;
		winrt::com_ptr<ID3D11Buffer>		m_constantBuffer;
		winrt::com_ptr<ID3D11Buffer>		m_indexBuffer;
		winrt::com_ptr<ID3D11InputLayout>	m_inputLayout;
		winrt::com_ptr<ID3D11VertexShader>	m_vertexShader;
		winrt::com_ptr<ID3D11PixelShader>	m_pixelShader;

		winrt::com_ptr<ID3D11Buffer>		m_vertexBufferBox;


		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32_t	m_indexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		bool	m_tracking;
		float	m_degreesPerSecond;
		double	m_lastTime;
		double	m_xPos;

		// Eye/at/up vectors
		XMVECTORF32 m_eye;
		XMVECTORF32 m_at;
		XMVECTORF32 m_up;
	};
}
