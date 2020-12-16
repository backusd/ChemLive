#include "pch.h"
#include "SimulationRenderer.h"
#include "DirectXHelper.h"


namespace Simulation
{
	SimulationRenderer::SimulationRenderer(
		const std::shared_ptr<DX::DeviceResources>& deviceResources,
		XMFLOAT3 boxDimensions) :
		m_deviceResources(deviceResources),
		m_loadingComplete(false),
		m_indexCount(0)
	{
		//m_eye = { 0.0f, 0.0f, (boxDimensions.z / 2.0f) + 1.0f, 0.0f };	// eye should be just outside the box in the z-direction
		m_eye = { 0.0f, 0.0f, 2 * boxDimensions.z, 0.0f };
		m_at = { 0.0f, 0.0f, 0.0f, 0.0f };								// look at the origin
		m_up = { 0.0f, 1.0f, 0.0f, 0.0f };								// Up in the positive y-direction

		CreateDeviceDependentResourcesAsync(boxDimensions);
		CreateWindowSizeDependentResources();
	}

	// Initializes view parameters when the window size changes.
	void SimulationRenderer::CreateWindowSizeDependentResources()
	{
		auto outputSize = m_deviceResources->GetOutputSize();
		float aspectRatio = outputSize.Width / outputSize.Height;
		float fovAngleY = XM_PI / 4;

		// This is a simple example of change that can be made when the app is in
		// portrait or snapped view.
		if (aspectRatio < 1.0f)
		{
			fovAngleY *= 2.0f;
		}

		// Note that the OrientationTransform3D matrix is post-multiplied here
		// in order to correctly orient the scene to match the display orientation.
		// This post-multiplication step is required for any draw calls that are
		// made to the swap chain render target. For draw calls to other targets,
		// this transform should not be applied.

		// This sample makes use of a right-handed coordinate system using row-major matrices.
		XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
			fovAngleY,
			aspectRatio,
			0.01f,
			100.0f
		);

		XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();
		XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

		XMStoreFloat4x4(
			&m_constantBufferData.projection,
			XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

		UpdateViewMatrix();
	}

	void SimulationRenderer::UpdateViewMatrix()
	{
		// If any of the eye/at/up vectors become modified, then this
		// method needs to be called
		XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(m_eye, m_at, m_up)));
	}

	// Renders one frame using the vertex and pixel shaders.
	void SimulationRenderer::Render(const std::vector<Atom*>& atoms, XMFLOAT3 boxDimensions)
	{
		// For each atom:
		//		Compute the vertex and index data (if only rendering spheres, may only need to do this once)
		//		Store the location and size info in the model matrix
		//		

		// Loading is asynchronous. Only draw geometry after it's loaded.
		if (!m_loadingComplete)
		{
			return;
		}

		auto context = m_deviceResources->GetD3DDeviceContext();


		// The sphere is already created, all we need to do is render it
		// multiple times in multiple locations based off of atom location
		// data

		for (Atom* atom : atoms)
		{
			// Update the constant buffer data
			XMFLOAT3 pos = atom->Position();
			XMStoreFloat4x4(
				&m_constantBufferData.model,
				XMMatrixTranspose(
					XMMatrixTranslation(pos.x, pos.y, pos.z)
				)
			);


			// Prepare the constant buffer to send it to the graphics device.
			context->UpdateSubresource1(
				m_constantBuffer.get(),
				0,
				NULL,
				&m_constantBufferData,
				0,
				0,
				0
			);

			// Each vertex is one instance of the VertexPositionColor struct.
			UINT stride = sizeof(VertexPositionColor);
			UINT offset = 0;
			ID3D11Buffer* const vertexBuffers[] = { m_vertexBuffer.get() };
			context->IASetVertexBuffers(
				0,
				1,
				vertexBuffers,
				&stride,
				&offset
			);

			context->IASetIndexBuffer(
				m_indexBuffer.get(),
				DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
				0
			);

			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			context->IASetInputLayout(m_inputLayout.get());

			// Attach our vertex shader.
			context->VSSetShader(
				m_vertexShader.get(),
				nullptr,
				0
			);

			// Send the constant buffer to the graphics device.
			ID3D11Buffer* const constantBuffers[] = { m_constantBuffer.get() };
			context->VSSetConstantBuffers1(
				0,
				1,
				constantBuffers,
				nullptr,
				nullptr
			);

			// Attach our pixel shader.
			context->PSSetShader(
				m_pixelShader.get(),
				nullptr,
				0
			);

			// Draw the objects.
			context->DrawIndexed(
				m_indexCount,
				0,
				0
			);
		}

		DrawBox();		
	}

	void SimulationRenderer::DrawBox()
	{
		auto context = m_deviceResources->GetD3DDeviceContext();

		XMStoreFloat4x4(
			&m_constantBufferData.model,
			XMMatrixTranspose(
				XMMatrixTranslation(0, 0, 0)
			)
		);

		// Prepare the constant buffer to send it to the graphics device.
		context->UpdateSubresource1(
			m_constantBuffer.get(),
			0,
			NULL,
			&m_constantBufferData,
			0,
			0,
			0
		);

		// Each vertex is one instance of the VertexPositionColor struct.
		UINT stride = sizeof(VertexPositionColor);
		UINT offset = 0;
		ID3D11Buffer* const vertexBuffers[] = { m_vertexBufferBox.get() };
		context->IASetVertexBuffers(
			0,
			1,
			vertexBuffers,
			&stride,
			&offset
		);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		context->IASetInputLayout(m_inputLayout.get());

		// Attach our vertex shader.
		context->VSSetShader(
			m_vertexShader.get(),
			nullptr,
			0
		);

		// Send the constant buffer to the graphics device.
		ID3D11Buffer* const constantBuffers[] = { m_constantBuffer.get() };
		context->VSSetConstantBuffers1(
			0,
			1,
			constantBuffers,
			nullptr,
			nullptr
		);

		// Attach our pixel shader.
		context->PSSetShader(
			m_pixelShader.get(),
			nullptr,
			0
		);

		// Draw the objects.
		context->Draw(24, 0);
	}

	void SimulationRenderer::LoadVertexShader(const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				m_vertexShader.put()
			)
		);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				m_inputLayout.put()
			)
		);
	}

	void SimulationRenderer::LoadPixelShader(const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				m_pixelShader.put()
			)
		);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				m_constantBuffer.put()
			)
		);
	}
		
	void SimulationRenderer::CreateSphere()
	{
		unsigned int segments = 26;
		unsigned int slices = segments / 2;
		unsigned int numVertices = (slices + 1) * (segments + 1) + 1;
		unsigned int numIndices = slices * segments * 3 * 2;

		std::vector<VertexPositionColor> sphereVertices(numVertices);

		// To make the texture look right on the top and bottom of the sphere
		// each slice will have 'segments + 1' vertices.  The top and bottom
		// vertices will all be coincident, but have different U texture cooordinates.
		unsigned int p = 0;
		for (unsigned int a = 0; a <= slices; a++)
		{
			float angle1 = static_cast<float>(a) / static_cast<float>(slices) * XM_PI;
			float z = cos(angle1);
			float r = sin(angle1);
			for (unsigned int b = 0; b <= segments; b++)
			{
				float angle2 = static_cast<float>(b) / static_cast<float>(segments) * XM_2PI;

				sphereVertices[p].pos = XMFLOAT3(r * cos(angle2), r * sin(angle2), z);
				sphereVertices[p].color = XMFLOAT3(0.0f, 0.0f, 1.0f);

				p++;
			}
		}
		unsigned int vertexCount = p;

		D3D11_SUBRESOURCE_DATA vertexBufferDataSphere = { 0 };
		vertexBufferDataSphere.pSysMem = sphereVertices.data();
		vertexBufferDataSphere.SysMemPitch = 0;
		vertexBufferDataSphere.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC vertexBufferDescSphere(
			sizeof(VertexPositionColor) * vertexCount,
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

	void SimulationRenderer::CreateBox(XMFLOAT3 boxDimensions)
	{
		// Draw the simulation box
		float x = boxDimensions.x / 2.0f;
		float y = boxDimensions.y / 2.0f;
		float z = boxDimensions.z / 2.0f;

		std::vector<VertexPositionColor> v(8); // box vertices
		v[0].pos = XMFLOAT3( x,  y,  z);
		v[1].pos = XMFLOAT3(-x,  y,  z);
		v[2].pos = XMFLOAT3( x, -y,  z);
		v[3].pos = XMFLOAT3( x,  y, -z);
		v[4].pos = XMFLOAT3(-x, -y,  z);
		v[5].pos = XMFLOAT3(-x,  y, -z);
		v[6].pos = XMFLOAT3( x, -y, -z);
		v[7].pos = XMFLOAT3(-x, -y, -z);

		for (VertexPositionColor vertex : v)
			vertex.color = XMFLOAT3(1.0f, 1.0f, 1.0f);

		std::vector<VertexPositionColor> vertexList;
		// draw the square with all positive x
		vertexList.push_back(v[0]);
		vertexList.push_back(v[3]);
		vertexList.push_back(v[3]);
		vertexList.push_back(v[6]);
		vertexList.push_back(v[6]);
		vertexList.push_back(v[2]);
		vertexList.push_back(v[2]);
		vertexList.push_back(v[0]);

		// draw the square with all negative x
		vertexList.push_back(v[1]);
		vertexList.push_back(v[5]);
		vertexList.push_back(v[5]);
		vertexList.push_back(v[7]);
		vertexList.push_back(v[7]);
		vertexList.push_back(v[4]);
		vertexList.push_back(v[4]);
		vertexList.push_back(v[1]);

		// draw the four lines that connect positive x with negative x
		vertexList.push_back(v[0]);
		vertexList.push_back(v[1]);
		vertexList.push_back(v[3]);
		vertexList.push_back(v[5]);
		vertexList.push_back(v[6]);
		vertexList.push_back(v[7]);
		vertexList.push_back(v[2]);
		vertexList.push_back(v[4]);

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = vertexList.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC vertexBufferDesc(
			sizeof(VertexPositionColor) * 24,
			D3D11_BIND_VERTEX_BUFFER
		);

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				m_vertexBufferBox.put()
			)
		);
	}

	concurrency::task<void> SimulationRenderer::CreateDeviceDependentResourcesAsync(XMFLOAT3 boxDimensions)
	{
		// Load shaders asynchronously.
		auto vsFileData = co_await DX::ReadDataAsync(L"SimulationVertexShader.cso");
		auto psFileData = co_await DX::ReadDataAsync(L"SimulationPixelShader.cso");

		LoadVertexShader(vsFileData);
		LoadPixelShader(psFileData);
		CreateSphere();
		CreateBox(boxDimensions);

		m_loadingComplete = true;
	}

	void SimulationRenderer::ReleaseDeviceDependentResources()
	{
		m_loadingComplete = false;
		m_vertexShader = nullptr;
		m_inputLayout = nullptr;
		m_pixelShader = nullptr;
		m_constantBuffer = nullptr;
		m_vertexBuffer = nullptr;
		m_indexBuffer = nullptr;
	}

	void SimulationRenderer::UpdateBoxDimensions(XMFLOAT3 newBoxDimensions)
	{
		// When the box dimensions change, it may be necessary to update the eye/at/up vectors

	}
}