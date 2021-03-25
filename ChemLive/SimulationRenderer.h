#pragma once

#include "pch.h"
#include "Atom.h"
#include "DeviceResources.h"
#include "Enums.h"
#include "HLSLStructures.h"
#include "MoveLookController.h"
#include "StepTimer.h"
#include "DirectXHelper.h"
#include "Pane.h"
#include <algorithm>
#include <cmath>
#include <pplawait.h>
#include <vector>

using namespace DirectX;
using ChemLive::MoveLookController;
using ChemLive::Pane;
using winrt::Windows::Foundation::Point;

namespace Simulation
{
	class SimulationRenderer
	{
	public:
		SimulationRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources, 
							XMFLOAT3 boxDimensions, 
							const std::shared_ptr<MoveLookController>& moveLookController);
		concurrency::task<void> CreateDeviceDependentResourcesAsync();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();

		// Render
		void Render(const std::vector<Atom*> &atoms);

		void UpdateBoxDimensions(XMFLOAT3 newBoxDimensions);		// Update the eye location if box dimensions change
		void BoxDimensions(XMFLOAT3 dims) { m_boxDimensions = dims; }

		void SetViewMatrix(XMMATRIX viewMatrix) { m_viewMatrix = viewMatrix; }

		// Pointer methods (used for picking / highlighting atoms)
		void PointerMoved(Point point, D2D1_RECT_F renderPaneRect, const std::vector<Atom*>& atoms);



		// Material Set Methods
		/*
		void EmmissiveX(float value) { m_materialProperties.Material.Emissive.x = value; }
		void EmmissiveY(float value) { m_materialProperties.Material.Emissive.y = value; }
		void EmmissiveZ(float value) { m_materialProperties.Material.Emissive.z = value; }

		void AmbientX(float value) { m_materialProperties.Material.Ambient.x = value; }
		void AmbientY(float value) { m_materialProperties.Material.Ambient.y = value; }
		void AmbientZ(float value) { m_materialProperties.Material.Ambient.z = value; }

		void DiffuseX(float value) { m_materialProperties.Material.Diffuse.x = value; }
		void DiffuseY(float value) { m_materialProperties.Material.Diffuse.y = value; }
		void DiffuseZ(float value) { m_materialProperties.Material.Diffuse.z = value; }

		void SpecularX(float value) { m_materialProperties.Material.Specular.x = value; }
		void SpecularY(float value) { m_materialProperties.Material.Specular.y = value; }
		void SpecularZ(float value) { m_materialProperties.Material.Specular.z = value; }

		void SpecularPower(float value) { m_materialProperties.Material.SpecularPower = value; }

		// Material Get Methods
		float EmmissiveX() { return m_materialProperties.Material.Emissive.x; }
		float EmmissiveY() { return m_materialProperties.Material.Emissive.y; }
		float EmmissiveZ() { return m_materialProperties.Material.Emissive.z; }

		float AmbientX() { return m_materialProperties.Material.Ambient.x; }
		float AmbientY() { return m_materialProperties.Material.Ambient.y; }
		float AmbientZ() { return m_materialProperties.Material.Ambient.z; }

		float DiffuseX() { return m_materialProperties.Material.Diffuse.x; }
		float DiffuseY() { return m_materialProperties.Material.Diffuse.y; }
		float DiffuseZ() { return m_materialProperties.Material.Diffuse.z; }

		float SpecularX() { return m_materialProperties.Material.Specular.x; }
		float SpecularY() { return m_materialProperties.Material.Specular.y; }
		float SpecularZ() { return m_materialProperties.Material.Specular.z; }

		float SpecularPower() { return m_materialProperties.Material.SpecularPower; }
		*/

	private:
		void LoadVertexShader(const std::vector<byte>& fileData);
		void LoadPixelShader(const std::vector<byte>& fileData);

		void CreateBox();
		void CreateStaticResources();


		bool SphereIntersection(XMVECTOR rayOrigin, XMVECTOR rayDirection, Atom* atom, float& distance);
		

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources; 
		std::shared_ptr<MoveLookController> m_moveLookController;		

		bool	m_loadingComplete;

		// Picking parameters
		Atom* m_atomHoveredOver;

		XMFLOAT4X4 m_projection;
		XMFLOAT4X4 m_view;
		XMFLOAT4X4 m_inverseView;
		XMFLOAT3   m_direction;
		XMVECTOR   m_eyeVector;
		XMFLOAT4X4 m_model;
		XMFLOAT4X4 m_translation;

		// Object Independent Resources
		winrt::com_ptr<ID3D11VertexShader>	m_vertexShader;
		winrt::com_ptr<ID3D11PixelShader>	m_pixelShader;
		winrt::com_ptr<ID3D11InputLayout>	m_inputLayout;

		winrt::com_ptr<ID3D11Buffer>		m_modelViewProjectionBuffer;
		ModelViewProjectionConstantBuffer	m_modelViewProjectionBufferData;
		XMMATRIX							m_viewMatrix;
		XMMATRIX							m_projectionMatrix;

		// Light Properties
		LightProperties						m_lightProperties;
		winrt::com_ptr<ID3D11Buffer>		m_lightPropertiesConstantBuffer;

		// MaterialProperties
		winrt::com_ptr<ID3D11Buffer>		m_materialPropertiesConstantBuffer;
		std::vector<MaterialProperties*>	m_materialProperties;

		// Box Resources
		winrt::com_ptr<ID3D11Buffer>		m_boxVertexBuffer;
		MaterialProperties					m_boxMaterialProperties;
		winrt::com_ptr<ID3D11Buffer>		m_boxMaterialPropertiesConstantBuffer;
		DirectX::XMFLOAT3					m_boxDimensions;
	};
}
