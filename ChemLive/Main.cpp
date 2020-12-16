#include "pch.h"
#include "Main.h"
#include "DirectXHelper.h"


namespace ChemLive
{
	// Loads and initializes application assets when the application is loaded.
	Main::Main(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
		m_deviceResources(deviceResources),
		m_windowClosed(false),
		m_windowVisible(true)
	{
		// Register to be notified if the Device is lost or recreated
		m_deviceResources->RegisterDeviceNotify(this);

		m_moveLookController = std::unique_ptr<MoveLookController>(new MoveLookController(CoreWindow::GetForCurrentThread()));

		// TODO: Replace this with your app's content initialization.

		// Simple Cube
		m_sceneRenderer = std::unique_ptr<Sample3DSceneRenderer>(new Sample3DSceneRenderer(m_deviceResources));
		m_fpsTextRenderer = std::unique_ptr<SampleFpsTextRenderer>(new SampleFpsTextRenderer(m_deviceResources));

		// Sphere
		m_sphereRenderer = std::unique_ptr<SphereRenderer>(new SphereRenderer(m_deviceResources));

		// Simulation
		m_simulation = std::unique_ptr<Simulation::Simulation>(new Simulation::Simulation(m_deviceResources));

		// TODO: Change the timer settings if you want something other than the default variable timestep mode.
		// e.g. for 60 FPS fixed timestep update logic, call:
		/*
		m_timer.SetFixedTimeStep(true);
		m_timer.SetTargetElapsedSeconds(1.0 / 60);
		*/
	}

	Main::~Main()
	{
		// Deregister device notification
		m_deviceResources->RegisterDeviceNotify(nullptr);
	}

	// Updates application state when the window size changes (e.g. device orientation change)
	void Main::CreateWindowSizeDependentResources()
	{
		// TODO: Replace this with the size-dependent initialization of your app's content.
		
		// Simple Cube
		m_sceneRenderer->CreateWindowSizeDependentResources();

		// Sphere
		m_sphereRenderer->CreateWindowSizeDependentResources();

		// Simulation
		m_simulation->CreateWindowSizeDependentResources();
	}

	// Updates the application state once per frame.
	void Main::Update()
	{
		ProcessInput();

		// Update scene objects.
		m_timer.Tick([&]()
			{
				// TODO: Replace this with your app's content update functions.

				// Simple Cube
				//m_sceneRenderer->Update(m_timer);
				m_fpsTextRenderer->Update(m_timer);

				// Sphere
				//m_sphereRenderer->Update(m_timer);

				// Simulation
				m_simulation->UpdateSimulation(m_timer);
			});
	}

	// Process all input from the user before updating simulation state
	void Main::ProcessInput()
	{
		// TODO: Add per frame input handling here.

		// Query the MoveLookController for any updates that may have occurred since 
		// the last rendering pass and update the state accordingly

		// Simple Cube
		if (!m_moveLookController->MouseDown() && m_sceneRenderer->IsTracking())
			m_sceneRenderer->StopTracking();
		else if (m_moveLookController->MouseDown() && !m_sceneRenderer->IsTracking())
			m_sceneRenderer->StartTracking();

		if (m_sceneRenderer->IsTracking())
			m_sceneRenderer->TrackingUpdate(m_moveLookController->MousePositionX());
		
		// Sphere
		//
		// Nothing to implement yet
		//

		// Simulation
		//
		// Need to implement:
		//		1. Mouse Move Event
		//		2. Mouse Down/Up
		//
	}

	// Renders the current frame according to the current application state.
	// Returns true if the frame was rendered and is ready to be displayed.
	bool Main::Render()
	{
		// Don't try to render anything before the first Update.
		if (m_timer.GetFrameCount() == 0)
		{
			return false;
		}

		ID3D11DeviceContext3* context = m_deviceResources->GetD3DDeviceContext();

		// Reset the viewport to target the whole screen.
		D3D11_VIEWPORT viewport = m_deviceResources->GetScreenViewport();
		context->RSSetViewports(1, &viewport);

		// Reset render targets to the screen.
		ID3D11RenderTargetView* const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
		context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

		// Clear the back buffer and depth stencil view.
		context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
		context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Render the scene objects.
		// TODO: Replace this with your app's content rendering functions.

		// Simple Cube
		//m_sceneRenderer->Render();
		m_fpsTextRenderer->Render();

		// Sphere
		//m_sphereRenderer->Render();

		// Simulation
		m_simulation->Render();


		return true;
	}

	// Notifies renderers that device resources need to be released.
	void Main::OnDeviceLost()
	{
		// Simple Cube
		m_sceneRenderer->ReleaseDeviceDependentResources();
		m_fpsTextRenderer->ReleaseDeviceDependentResources();

		// Sphere
		m_sphereRenderer->ReleaseDeviceDependentResources();

		// Simulation
		m_simulation->ReleaseDeviceDependentResources();
	}

	// Notifies renderers that device resources may now be recreated.
	void Main::OnDeviceRestored()
	{
		// Simple Cube
		m_sceneRenderer->CreateDeviceDependentResourcesAsync();
		m_fpsTextRenderer->CreateDeviceDependentResources();

		// Sphere
		m_sphereRenderer->CreateDeviceDependentResourcesAsync();

		// Simulation
		m_simulation->CreateDeviceDependentResourcesAsync();

		CreateWindowSizeDependentResources();
	}

	// =============================================================================
	// App Lifecycle Functions - Should only be called from App.cpp

	void Main::Run()
	{
		while (!m_windowClosed)
		{
			if (m_windowVisible)
			{
				CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

				Update();
				if (Render())
				{
					m_deviceResources->Present();
				}
			}
			else
			{
				CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
			}
		}

	}
	void Main::Suspend()
	{

	}
	void Main::Resume()
	{

	}
	void Main::Visibility(bool visibility)
	{
		/* Going to need to add m_renderNeeded variable
		if (visible && !m_visible)
		{
			m_renderNeeded = true;
		} */
		m_windowVisible = visibility;
	}
	void Main::WindowActivationChanged(CoreWindowActivationState)
	{

	}
}