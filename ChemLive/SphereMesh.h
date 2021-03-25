#pragma once

#include "DeviceResources.h"
#include "HLSLStructures.h"


using namespace DirectX;

namespace Simulation
{
	/* 
	The purpose of this class is to house the vertex and index
	buffers that are required to render a sphere
	
	It also is responsible for computing the model (world) matrix for
	the sphere and makes the Draw call to render it
	*/
	class SphereMesh
	{
	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		winrt::com_ptr<ID3D11Buffer>		m_vertexBuffer;
		winrt::com_ptr<ID3D11Buffer>		m_indexBuffer;
		uint32_t							m_indexCount;

		winrt::com_ptr<ID3D11Buffer>		m_modelViewProjectionBuffer;
		ModelViewProjectionConstantBuffer	m_modelViewProjectionBufferData;

		XMMATRIX m_modelMatrix;

		void CreateModelViewProjectionBuffer();
		void CreateAndLoadVertexAndIndexBuffers();

	public:
		SphereMesh(const std::shared_ptr<DX::DeviceResources>& deviceResources);

		/* Render must be passed the following:
			1. XMFLOAT3 position       - position of the atom   -> used for translating to compute the model matrix
			2. float    radius         - radius of the atom     -> used for scaling to compute the model matrix
			3. XMMATRIX viewProjection - view projection matrix -> used for computing the final modelviewprojection matrix

			Upstream:
			1. Simulation Render should have already
					IASetInputLayout
					IASetPrimitiveTopology <- Only true as long as we are always rendered triangle lists
					VSSetShader
					PSSetShader
					PSSetConstantBuffers1  <- This will require a lookup for each atom to make sure the correct material is used
					
			Responsilities:
			1. Render will perform the following
					IASetVertexBuffers
					ISSetIndexBuffer
					VSSetConstantBuffers1
					DrawIndexed
		*/
		void Render(XMFLOAT3 position, float radius, XMMATRIX viewProjection);

		XMMATRIX ModelMatrix() { return m_modelMatrix; }

	};
}