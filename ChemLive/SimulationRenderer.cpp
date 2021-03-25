#include "pch.h"
#include "SimulationRenderer.h"



namespace Simulation
{
	SimulationRenderer::SimulationRenderer(
		const std::shared_ptr<DX::DeviceResources>& deviceResources,
		XMFLOAT3 boxDimensions, 
		const std::shared_ptr<MoveLookController>& moveLookController) :
			m_deviceResources(deviceResources),
			m_moveLookController(moveLookController),
			m_loadingComplete(false),
			m_boxDimensions(boxDimensions),
			m_atomHoveredOver(nullptr)
	{
		CreateDeviceDependentResourcesAsync();
		CreateWindowSizeDependentResources();

		// Load data that will not change on window resizing and is not dependent on the device
		CreateStaticResources();
	}

	concurrency::task<void> SimulationRenderer::CreateDeviceDependentResourcesAsync()
	{
		auto myVSFileData = co_await DX::ReadDataAsync(L"MyVertexShader.cso");
		auto myPSFileData = co_await DX::ReadDataAsync(L"MyPixelShader.cso");

		LoadVertexShader(myVSFileData);
		LoadPixelShader(myPSFileData);

		CreateBox();

		m_loadingComplete = true;
	}

	// Initializes view parameters when the window size changes.
	void SimulationRenderer::CreateWindowSizeDependentResources()
	{
		auto outputSize = m_deviceResources->GetRenderPaneOutputSize();
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

		// Projection Matrix (No Transpose)
		m_projectionMatrix = perspectiveMatrix * orientationMatrix;

		// Set the view matrix
		m_viewMatrix = m_moveLookController->ViewMatrix();
	}

	void SimulationRenderer::CreateStaticResources()
	{
		// ========================================================================================
		// Sphere Material

		// Load vector of material properties
		MaterialProperties* dummy = new MaterialProperties(); // Set a dummy property so that each element is at the index of its element type
		m_materialProperties.push_back(dummy);

		MaterialProperties* hydrogen = new MaterialProperties();
		hydrogen->Material.Emissive = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);
		hydrogen->Material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		hydrogen->Material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		hydrogen->Material.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		hydrogen->Material.SpecularPower = 6.0f;

		m_materialProperties.push_back(hydrogen);

		MaterialProperties* helium = new MaterialProperties();
		helium->Material.Emissive = XMFLOAT4(0.4f, 0.14f, 0.14f, 1.0f);
		helium->Material.Ambient = XMFLOAT4(1.0f, 0.75f, 0.75f, 1.0f);
		helium->Material.Diffuse = XMFLOAT4(1.0f, 0.6f, 0.6f, 1.0f);
		helium->Material.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		helium->Material.SpecularPower = 6.0f;

		m_materialProperties.push_back(helium);

		MaterialProperties* lithium = new MaterialProperties();
		lithium->Material.Emissive = XMFLOAT4(0.15f, 0.0f, 0.15f, 1.0f);
		lithium->Material.Ambient = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
		lithium->Material.Diffuse = XMFLOAT4(1.0f, 0.6f, 0.6f, 1.0f);
		lithium->Material.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		lithium->Material.SpecularPower = 6.0f;

		m_materialProperties.push_back(lithium);

		MaterialProperties* beryllium = new MaterialProperties();
		beryllium->Material.Emissive = XMFLOAT4(0.15f, 0.15f, 0.0f, 1.0f);
		beryllium->Material.Ambient = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
		beryllium->Material.Diffuse = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
		beryllium->Material.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		beryllium->Material.SpecularPower = 6.0f;

		m_materialProperties.push_back(beryllium);

		MaterialProperties* aluminum = new MaterialProperties();
		aluminum->Material.Emissive = XMFLOAT4(0.45f, 0.22f, 0.22f, 1.0f);
		aluminum->Material.Ambient = XMFLOAT4(1.0f, 0.45f, 0.45f, 1.0f);
		aluminum->Material.Diffuse = XMFLOAT4(1.0f, 0.8f, 0.8f, 1.0f);
		aluminum->Material.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		aluminum->Material.SpecularPower = 6.0f;

		m_materialProperties.push_back(aluminum);

		MaterialProperties* carbon = new MaterialProperties();
		carbon->Material.Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		carbon->Material.Ambient = XMFLOAT4(0.12f, 0.12f, 0.12f, 1.0f);
		carbon->Material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		carbon->Material.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		carbon->Material.SpecularPower = 6.0f;

		m_materialProperties.push_back(carbon);

		MaterialProperties* nitrogen = new MaterialProperties();
		nitrogen->Material.Emissive = XMFLOAT4(0.0f, 0.0f, 0.3f, 1.0f);
		nitrogen->Material.Ambient = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		nitrogen->Material.Diffuse = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		nitrogen->Material.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		nitrogen->Material.SpecularPower = 6.0f;

		m_materialProperties.push_back(nitrogen);

		MaterialProperties* oxygen = new MaterialProperties();
		oxygen->Material.Emissive = XMFLOAT4(0.3f, 0.0f, 0.0f, 1.0f);
		oxygen->Material.Ambient = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		oxygen->Material.Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		oxygen->Material.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		oxygen->Material.SpecularPower = 6.0f;

		m_materialProperties.push_back(oxygen);

		MaterialProperties* flourine = new MaterialProperties();
		flourine->Material.Emissive = XMFLOAT4(0.0f, 0.12f, 0.12f, 1.0f);
		flourine->Material.Ambient = XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f);
		flourine->Material.Diffuse = XMFLOAT4(0.0f, 0.2f, 1.0f, 1.0f);
		flourine->Material.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		flourine->Material.SpecularPower = 6.0f;

		m_materialProperties.push_back(flourine);

		MaterialProperties* neon = new MaterialProperties();
		neon->Material.Emissive = XMFLOAT4(0.1f, 0.3f, 0.3f, 1.0f);
		neon->Material.Ambient = XMFLOAT4(0.3f, 1.0f, 0.0f, 1.0f);
		neon->Material.Diffuse = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
		neon->Material.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		neon->Material.SpecularPower = 6.0f;

		m_materialProperties.push_back(neon);

		// ========================================================================================
		// Box Material
		m_boxMaterialProperties = MaterialProperties();

		m_boxMaterialProperties.Material.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		m_boxMaterialProperties.Material.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		m_boxMaterialProperties.Material.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		m_boxMaterialProperties.Material.SpecularPower = 10.0f;


		// ========================================================================================
		// Lighting
		m_lightProperties = LightProperties();
		m_lightProperties.GlobalAmbient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

		// The initial eye position - you will want to modify MoveLookController so the Eye
		// position can be retrieved to also update the light position
		m_lightProperties.EyePosition = XMFLOAT4(0.0f, 0.0f, 2 * m_boxDimensions.z, 0.0f);

		// Add the lights
		static const XMVECTORF32 LightColors[MAX_LIGHTS] = {
			DirectX::Colors::White,
			DirectX::Colors::Orange,
			DirectX::Colors::Yellow,
			DirectX::Colors::Green,
			DirectX::Colors::Blue,
			DirectX::Colors::Indigo,
			DirectX::Colors::Violet,
			DirectX::Colors::White
		};

		static const LightType LightTypes[MAX_LIGHTS] = {
			PointLight, SpotLight, SpotLight, PointLight, SpotLight, SpotLight, SpotLight, PointLight
		};

		static const bool LightEnabled[MAX_LIGHTS] = {
			true, false, false, false, false, false, false, false
		};

		const int numLights = MAX_LIGHTS;
		for (int i = 0; i < numLights; ++i)
		{
			Light light;
			light.Enabled = static_cast<int>(LightEnabled[i]);
			light.LightType = LightTypes[i];
			light.Color = XMFLOAT4(LightColors[i]);
			light.SpotAngle = XMConvertToRadians(45.0f);
			light.ConstantAttenuation = 1.0f;
			light.LinearAttenuation = 0.08f;
			light.QuadraticAttenuation = 0.0f;

			// Make the light slightly offset from the initial eye position
			//XMFLOAT4 LightPosition = XMFLOAT4(std::sin(totalTime + offset * i) * radius, 9.0f, std::cos(totalTime + offset * i) * radius, 1.0f);
			XMFLOAT4 LightPosition = XMFLOAT4(m_boxDimensions.x / 4, m_boxDimensions.y / 4, 2 * m_boxDimensions.z, 1.0f);
			light.Position = LightPosition;
			XMVECTOR LightDirection = XMVectorSet(-LightPosition.x, -LightPosition.y, -LightPosition.z, 0.0f);
			XMStoreFloat4(&light.Direction, XMVector3Normalize(LightDirection));

			m_lightProperties.Lights[i] = light;
		}
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

		/* D3D11_INPUT_ELEMENT_DESC
		Parameter 1: Semantic Name           - must match the semantic used in VertexShaderInput
		Parameter 2: Semantic Index          - Only used when there are more than one element with the same semantic name
		Parameter 3: Format                  - DXGI_FORMAT_R32G32B32_FLOAT = a 3-component float for storing color info
		Parameter 4: Input Slot              - Value that identifies the input-assembler ???
		Parameter 5: Aligned Byte Offset     - Offset in bytes from the start of the vertex (Use D3D11_APPEND_ALIGNED_ELEMENT to have the value inferred)
		Parameter 6: Input Slot Class        - D3D11_INPUT_PER_VERTEX_DATA = Input data is per vertex
		Parameter 7: Instance Data Step Rate - Number of instance to draw using same instance data (must be 0 for per-vertex data)
		*/

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
				m_modelViewProjectionBuffer.put()
			)
		);

		// Sphere Material constant buffer (Pixel Shader)
		CD3D11_BUFFER_DESC materialConstantBufferDesc(sizeof(MaterialProperties), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&materialConstantBufferDesc,
				nullptr,
				m_materialPropertiesConstantBuffer.put()
			)
		);

		// Box Material constant buffer (Pixel Shader)
		CD3D11_BUFFER_DESC boxMaterialConstantBufferDesc(sizeof(MaterialProperties), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&boxMaterialConstantBufferDesc,
				nullptr,
				m_boxMaterialPropertiesConstantBuffer.put()
			)
		);

		// Lights constant buffer (Pixel Shader)
		CD3D11_BUFFER_DESC lightsConstantBufferDesc(sizeof(LightProperties), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&lightsConstantBufferDesc,
				nullptr,
				m_lightPropertiesConstantBuffer.put()
			)
		);
	}

	void SimulationRenderer::Render(const std::vector<Atom*>& atoms)
	{
		// Loading is asynchronous. Only draw geometry after it's loaded.
		if (!m_loadingComplete)
		{
			return;
		}

		/* PREPARE THE PIPELINE
		
			The goal here is to prepare the rendering pipeline by implementing the settings
			that will remain constant for rendering each atom. The includes the following:

			1. Compute ViewProjection matrix - this will get passed to each atom to compute the final ModelViewProjection matrix
			2. IASetPrimitiveTopology - Set topology to be triangle lists, which is true for all atoms
			3. IASetInputLayout - Input layout will be the say for all vertices
			4. VSSetShader - Use the same vertex shader for all vertices
			5. PSSetShader - Use the same pixel shader for all vertices
			6. PSSetConstantBuffers1 - Pixel shader requires two buffers:
					* LightPropertiesBuffer - Will only change between render passes
					* MaterialPropertiesBuffer - Must be set each time a new atom type is encountered
							- To do this efficiently, the vector of atoms should be sorted in increasing atomic number
							- The render loop can then see if the next atom to render is off a different type
									-> If so, load the material for that atom type
		
		*/
		auto context = m_deviceResources->GetD3DDeviceContext();

		// Compute the view/projection matrix
		XMMATRIX viewProjectionMatrix = m_viewMatrix * m_projectionMatrix;

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(m_inputLayout.get());

		// Attach our vertex shader.
		context->VSSetShader(m_vertexShader.get(), nullptr, 0);

		// Attach our pixel shader.
		context->PSSetShader(m_pixelShader.get(), nullptr, 0);

		// Update the Material constant buffer and Light constant buffer then bind it to the pixel shader
		context->UpdateSubresource(m_lightPropertiesConstantBuffer.get(), 0, nullptr, &m_lightProperties, 0, 0);

		// Set the current element to invalid so that the first atom will set the material properties
		Element currentElement = Element::INVALID;
		MaterialProperties* hoverAtomMaterialPropertiesOLD;
		MaterialProperties* hoverAtomMaterialPropertiesNEW;

		for (Atom* atom : atoms)
		{
			// If the atom is the atom that is hovered over, then we need to adjust its color directly
			if (atom == m_atomHoveredOver)
			{
				hoverAtomMaterialPropertiesOLD = m_materialProperties[atom->Element()];

				// Copy the material settings and adjust the Emission of the material
				hoverAtomMaterialPropertiesNEW = new MaterialProperties();
				hoverAtomMaterialPropertiesNEW->Material = hoverAtomMaterialPropertiesOLD->Material;
				hoverAtomMaterialPropertiesNEW->Material.Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

				context->UpdateSubresource(m_materialPropertiesConstantBuffer.get(), 0, nullptr, hoverAtomMaterialPropertiesNEW, 0, 0);
				ID3D11Buffer* const psConstantBuffers[] = { m_materialPropertiesConstantBuffer.get(), m_lightPropertiesConstantBuffer.get() };
				context->PSSetConstantBuffers1(0, 2, psConstantBuffers, nullptr, nullptr);

				// Set the atom element to invalid so that the materials properties will get updated for the next atom
				currentElement = Element::INVALID;
			}
			else
			{
				if (atom->Element() != currentElement)
				{
					currentElement = atom->Element();

					context->UpdateSubresource(m_materialPropertiesConstantBuffer.get(), 0, nullptr, m_materialProperties[currentElement], 0, 0);

					ID3D11Buffer* const psConstantBuffers[] = { m_materialPropertiesConstantBuffer.get(), m_lightPropertiesConstantBuffer.get() };
					context->PSSetConstantBuffers1(0, 2, psConstantBuffers, nullptr, nullptr);
				}				
			}

			atom->Render(viewProjectionMatrix);
		}

		// Draw Box =============================================================================
		UINT stride = sizeof(VertexPositionNormal);
		UINT offset = 0;
		ID3D11Buffer* const boxVertexBuffers[] = { m_boxVertexBuffer.get() };
		context->IASetVertexBuffers(0, 1, boxVertexBuffers, &stride, &offset);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		// Set the translation to 0 and update the modelviewprojection matrices
		XMMATRIX model = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		XMStoreFloat4x4(&m_modelViewProjectionBufferData.model, model);
		XMStoreFloat4x4(&m_modelViewProjectionBufferData.modelViewProjection, model * viewProjectionMatrix);
		XMStoreFloat4x4(&m_modelViewProjectionBufferData.inverseTransposeModel, XMMatrixTranspose(XMMatrixInverse(nullptr, model)));

		// Prepare the constant buffer to send it to the graphics device.
		context->UpdateSubresource1(m_modelViewProjectionBuffer.get(), 0, NULL, &m_modelViewProjectionBufferData, 0, 0, 0);

		// Send the constant buffer to the graphics device.
		ID3D11Buffer* const boxConstantBuffers[] = { m_modelViewProjectionBuffer.get() };
		context->VSSetConstantBuffers1(0, 1, boxConstantBuffers, nullptr, nullptr);

		// Update the Material constant buffer for the box and then bind it to the pixel shader
		context->UpdateSubresource(m_boxMaterialPropertiesConstantBuffer.get(), 0, nullptr, &m_boxMaterialProperties, 0, 0);
		ID3D11Buffer* const psBoxConstantBuffers[] = { m_boxMaterialPropertiesConstantBuffer.get(), m_lightPropertiesConstantBuffer.get() };
		context->PSSetConstantBuffers1(0, 2, psBoxConstantBuffers, nullptr, nullptr);

		// Draw the objects.
		context->Draw(24, 0);
	}
	
	

	void SimulationRenderer::PointerMoved(Point point, D2D1_RECT_F renderPaneRect, const std::vector<Atom*>& atoms)
	{
		/* Will update which atom the pointer is over
		*  To not affect performance, this method should only be called
		*  when the simulation is paused
		*/

		XMFLOAT3 clickpointNear = XMFLOAT3(point.X, point.Y, 0.0f);
		XMFLOAT3 clickpointFar  = XMFLOAT3(point.X, point.Y, 1.0f);

		XMVECTOR clickpointNearVector = XMLoadFloat3(&clickpointNear);
		XMVECTOR clickpointFarVector  = XMLoadFloat3(&clickpointFar);

		XMVECTOR origin, destination, direction;

		float shortestDistance = FLT_MAX; // Set initial to the maximum possible float value
		float distance = FLT_MAX; 
		m_atomHoveredOver = nullptr;

		for (Atom* atom : atoms)
		{
			origin = XMVector3Unproject(
				clickpointNearVector,
				renderPaneRect.left,
				renderPaneRect.top,
				renderPaneRect.right - renderPaneRect.left,
				renderPaneRect.bottom - renderPaneRect.top,
				0,
				1,
				m_projectionMatrix,
				m_viewMatrix,
				atom->TranslationMatrix());

			destination = XMVector3Unproject(
				clickpointFarVector,
				renderPaneRect.left,
				renderPaneRect.top,
				renderPaneRect.right - renderPaneRect.left,
				renderPaneRect.bottom - renderPaneRect.top,
				0,
				1,
				m_projectionMatrix,
				m_viewMatrix,
				atom->TranslationMatrix());

			direction = XMVector3Normalize(destination - origin);

			// if an intersection is found, the distance will be returned in the 'distance' variable
			if (SphereIntersection(origin, direction, atom, distance))
			{
				if (distance < shortestDistance)
				{
					m_atomHoveredOver = atom;
					shortestDistance = distance;
				}
			}
		}
	}
	
	bool SimulationRenderer::SphereIntersection(XMVECTOR rayOrigin, XMVECTOR rayDirection, Atom* atom, float& distance)
	{
		XMFLOAT3 origin, direction;
		XMStoreFloat3(&origin, rayOrigin);
		XMStoreFloat3(&direction, rayDirection);

		float a, b, c, discriminant;
		float radius = atom->Radius();

		// Calculate the a, b, and c coefficients.
		a = (direction.x * direction.x) + (direction.y * direction.y) + (direction.z * direction.z);
		b = ((direction.x * origin.x) + (direction.y * origin.y) + (direction.z * origin.z)) * 2.0f;
		c = ((origin.x * origin.x) + (origin.y * origin.y) + (origin.z * origin.z)) - (radius * radius);

		// Find the discriminant.
		discriminant = (b * b) - (4 * a * c);

		// if discriminant is negative the picking ray missed the sphere, otherwise it intersected the sphere.
		if (discriminant < 0.0f)
			return false;

		// determine the distance to the closest point
		float minRoot = (-b + std::sqrtf(discriminant)) / (2 * a);
		float maxRoot = (-b - std::sqrtf(discriminant)) / (2 * a);

		if (minRoot > maxRoot)
			std::swap(minRoot, maxRoot);

		// Get the smallest positive root
		if (minRoot < 0)
		{
			minRoot = maxRoot;
			if (minRoot < 0)
				return false;		// Both roots are negative so return false
		}

		// Return the distance to the sphere
		distance = minRoot;

		return true;
	}
	



	void SimulationRenderer::CreateBox()
	{
		// Draw the simulation box
		float x = m_boxDimensions.x / 2.0f;
		float y = m_boxDimensions.y / 2.0f;
		float z = m_boxDimensions.z / 2.0f;

		std::vector<VertexPositionNormal> v(8); // box vertices
		v[0].position = XMFLOAT3( x,  y,  z);
		v[1].position = XMFLOAT3(-x,  y,  z);
		v[2].position = XMFLOAT3( x, -y,  z);
		v[3].position = XMFLOAT3( x,  y, -z);
		v[4].position = XMFLOAT3(-x, -y,  z);
		v[5].position = XMFLOAT3(-x,  y, -z);
		v[6].position = XMFLOAT3( x, -y, -z);
		v[7].position = XMFLOAT3(-x, -y, -z);

		for (VertexPositionNormal vertex : v)
			vertex.normal = XMFLOAT3(1.0f, 1.0f, 1.0f);

		std::vector<VertexPositionNormal> vertexList;
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
			sizeof(VertexPositionNormal) * 24,
			D3D11_BIND_VERTEX_BUFFER
		);

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				m_boxVertexBuffer.put()
			)
		);
	}



	void SimulationRenderer::ReleaseDeviceDependentResources()
	{
		m_loadingComplete = false;

		m_vertexShader = nullptr;
		m_inputLayout = nullptr;
		m_pixelShader = nullptr;

		m_modelViewProjectionBuffer = nullptr;
		
		m_materialPropertiesConstantBuffer = nullptr;

		m_lightPropertiesConstantBuffer = nullptr;

		m_boxVertexBuffer = nullptr;
		m_boxMaterialPropertiesConstantBuffer = nullptr;
	}

	void SimulationRenderer::UpdateBoxDimensions(XMFLOAT3 newBoxDimensions)
	{
		// When the box dimensions change, it may be necessary to update the eye/at/up vectors

	}
}