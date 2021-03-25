#include "pch.h"
#include "SphereMesh.h"


namespace Simulation
{
	SphereMesh::SphereMesh(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
		m_deviceResources(deviceResources),
		m_indexCount(0)
	{
		CreateModelViewProjectionBuffer();
		CreateAndLoadVertexAndIndexBuffers();
	}

	void SphereMesh::CreateModelViewProjectionBuffer()
	{
		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				m_modelViewProjectionBuffer.put()
			)
		);
	}

	void SphereMesh::CreateAndLoadVertexAndIndexBuffers()
	{
		unsigned int segments = 26;
		unsigned int slices = segments / 2;
		unsigned int numVertices = (slices + 1) * (segments + 1) + 1;
		unsigned int numIndices = slices * segments * 3 * 2;

		std::vector<VertexPositionNormal> sphereVertices(numVertices);

		// To make the texture look right on the top and bottom of the sphere
		// each slice will have 'segments + 1' vertices.  The top and bottom
		// vertices will all be coincident, but have different U texture cooordinates.
		unsigned int p = 0;
		XMFLOAT3 positionNormal; // We are creating a unit sphere so the position is the same as the normal
		for (unsigned int a = 0; a <= slices; a++)
		{
			float angle1 = static_cast<float>(a) / static_cast<float>(slices) * XM_PI;
			float z = cos(angle1);
			float r = sin(angle1);
			for (unsigned int b = 0; b <= segments; b++)
			{
				float angle2 = static_cast<float>(b) / static_cast<float>(segments) * XM_2PI;

				// We are working with the unit sphere so the position and normal
				// vectors are the same
				positionNormal = XMFLOAT3(r * cos(angle2), r * sin(angle2), z);
				sphereVertices[p].position = positionNormal;
				sphereVertices[p].normal = positionNormal;

				p++;
			}
		}
		unsigned int vertexCount = p;

		D3D11_SUBRESOURCE_DATA vertexBufferDataSphere = { 0 };
		vertexBufferDataSphere.pSysMem = sphereVertices.data();
		vertexBufferDataSphere.SysMemPitch = 0;
		vertexBufferDataSphere.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC vertexBufferDescSphere(
			sizeof(VertexPositionNormal) * vertexCount,
			D3D11_BIND_VERTEX_BUFFER
		);

		m_vertexBuffer = nullptr;
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDescSphere,
				&vertexBufferDataSphere,
				m_vertexBuffer.put()
			)
		);

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.

		std::vector<unsigned short> sphereIndices(numIndices);

		p = 0;
		for (unsigned short a = 0; a < slices; a++)
		{
			unsigned short p1 = a * (segments + 1);
			unsigned short p2 = (a + 1) * (segments + 1);

			// Generate two triangles for each segment around the slice.
			for (unsigned short b = 0; b < segments; b++)
			{
				if (a < (slices - 1))
				{
					// For all but the bottom slice add the triangle with one
					// vertex in the a slice and two vertices in the a + 1 slice.
					// Skip it for the bottom slice since the triangle would be
					// degenerate as all the vertices in the bottom slice are coincident.
					sphereIndices[p] = b + p1;
					sphereIndices[p + 1] = b + p2;
					sphereIndices[p + 2] = b + p2 + 1;
					p = p + 3;
				}
				if (a > 0)
				{
					// For all but the top slice add the triangle with two
					// vertices in the a slice and one vertex in the a + 1 slice.
					// Skip it for the top slice since the triangle would be
					// degenerate as all the vertices in the top slice are coincident.
					sphereIndices[p] = b + p1;
					sphereIndices[p + 1] = b + p2 + 1;
					sphereIndices[p + 2] = b + p1 + 1;
					p = p + 3;
				}
			}
		}
		m_indexCount = p;

		D3D11_SUBRESOURCE_DATA indexBufferDataSphere = { 0 };
		indexBufferDataSphere.pSysMem = sphereIndices.data();
		indexBufferDataSphere.SysMemPitch = 0;
		indexBufferDataSphere.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC indexBufferDescSphere(
			sizeof(unsigned short) * m_indexCount,
			D3D11_BIND_INDEX_BUFFER
		);

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDescSphere,
				&indexBufferDataSphere,
				m_indexBuffer.put()
			)
		);
	}

	void SphereMesh::Render(XMFLOAT3 position, float radius, XMMATRIX viewProjection)
	{
		auto context = m_deviceResources->GetD3DDeviceContext();

		// Each vertex is one instance of the MyVertexPositionNormal struct.
		UINT stride = sizeof(VertexPositionNormal);
		UINT offset = 0;
		ID3D11Buffer* const vertexBuffers[] = { m_vertexBuffer.get() };
		context->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

		context->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);

		// Compute the model matrix
		//XMMATRIX model = XMMatrixTranslation(position.x, position.y, position.z) * XMMatrixScaling(radius, radius, radius);
		m_modelMatrix = XMMatrixScaling(radius, radius, radius) * XMMatrixTranslation(position.x, position.y, position.z);
		//m_modelMatrix = XMMatrixTranslation(position.x, position.y, position.z);


		// Store model, modelviewprojection, and inversetransposemodel matrices
		XMStoreFloat4x4(&m_modelViewProjectionBufferData.model, m_modelMatrix);
		XMStoreFloat4x4(&m_modelViewProjectionBufferData.modelViewProjection, m_modelMatrix * viewProjection);
		XMStoreFloat4x4(&m_modelViewProjectionBufferData.inverseTransposeModel, XMMatrixTranspose(XMMatrixInverse(nullptr, m_modelMatrix)));

		// Prepare the constant buffer to send it to the graphics device.
		context->UpdateSubresource1(m_modelViewProjectionBuffer.get(), 0, NULL, &m_modelViewProjectionBufferData, 0, 0, 0);

		// Send the constant buffer to the graphics device.
		ID3D11Buffer* const constantBuffers[] = { m_modelViewProjectionBuffer.get() };
		context->VSSetConstantBuffers1(0, 1, constantBuffers, nullptr, nullptr);

		// Draw the objects.
		context->DrawIndexed(m_indexCount, 0, 0);
	}

}