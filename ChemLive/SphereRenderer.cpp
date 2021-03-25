#include "pch.h"
#include "SphereRenderer.h"
#include "DirectXHelper.h"


namespace DirectX
{
	// Loads vertex and pixel shaders from files and instantiates the cube geometry.
	SphereRenderer::SphereRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
		m_loadingComplete(false),
		m_degreesPerSecond(45),
		m_lastTime(0),
		m_xPos(0),
		m_indexCount(0),
		m_tracking(false),
		m_deviceResources(deviceResources)
	{
		CreateDeviceDependentResourcesAsync();
		CreateWindowSizeDependentResources();
	}

	// Initializes view parameters when the window size changes.
	void SphereRenderer::CreateWindowSizeDependentResources()
	{
		auto outputSize = m_deviceResources->GetRenderPaneOutputSize();
		float aspectRatio = outputSize.Width / outputSize.Height;
		float fovAngleY = 70.0f * XM_PI / 180.0f;

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

		// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
		static const XMVECTORF32 eye = { 0.0f, 0.0f, 5.0f, 0.0f };
		static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
		static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

		XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
	}

	// Called once per frame, rotates the cube and calculates the model and view matrices.
	void SphereRenderer::Update(DX::StepTimer const& timer)
	{
		if (!m_tracking)
		{
			double currentTime = timer.GetTotalSeconds();

			// Convert degrees to radians, then convert seconds to rotation angle
			float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
			double totalRotation = currentTime * radiansPerSecond;
			float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

			//Rotate(radians);

			// Translate in the x-direction

			float radius = 1.0f;
			float xAdjustment = currentTime - m_lastTime;
			m_lastTime = currentTime;

			if (static_cast<int>(currentTime) % 2 == 0)
				m_xPos -= xAdjustment;
			else
				m_xPos += xAdjustment;

			XMStoreFloat4x4(
				&m_constantBufferData.model,
				XMMatrixTranspose(
					XMMatrixScaling(radius, radius, radius) *
					XMMatrixRotationY(radians) *
					XMMatrixTranslation(m_xPos, 0.0f, 0.0f)
				)
			);
		}
	}

	void SphereRenderer::Rotate(float radians)
	{
		// Prepare to pass the updated model matrix to the shader
		XMStoreFloat4x4(
			&m_constantBufferData.model, 
			XMMatrixTranspose(XMMatrixRotationY(radians))
		);
	}

	// Renders one frame using the vertex and pixel shaders.
	void SphereRenderer::Render()
	{
		// Loading is asynchronous. Only draw geometry after it's loaded.
		if (!m_loadingComplete)
		{
			return;
		}

		auto context = m_deviceResources->GetD3DDeviceContext();

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

	void SphereRenderer::LoadVertexShader(const std::vector<byte>& fileData) 
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

	void SphereRenderer::LoadPixelShader(const std::vector<byte>& fileData) 
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

	void SphereRenderer::CreateSphere() 
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

	concurrency::task<void> SphereRenderer::CreateDeviceDependentResourcesAsync()
	{
		// Load shaders asynchronously.
		auto vsFileData = co_await DX::ReadDataAsync(L"SphereVertexShader.cso");
		auto psFileData = co_await DX::ReadDataAsync(L"SpherePixelShader.cso");

		LoadVertexShader(vsFileData);
		LoadPixelShader(psFileData);
		CreateSphere();

		m_loadingComplete = true;
	}

	void SphereRenderer::ReleaseDeviceDependentResources()
	{
		m_loadingComplete = false;
		m_vertexShader = nullptr;
		m_inputLayout = nullptr;
		m_pixelShader = nullptr;
		m_constantBuffer = nullptr;
		m_vertexBuffer = nullptr;
		m_indexBuffer = nullptr;
	}
}