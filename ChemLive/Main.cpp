#include "pch.h"
#include "Main.h"
#include "DirectXHelper.h"
#include "TextBox.h"
#include "Button.h"
#include "Slider.h"
#include "DropDown.h"
#include "Theme.h"

using winrt::Windows::Foundation::Point;
using winrt::Windows::ApplicationModel::Core::CoreApplication;
using winrt::Windows::ApplicationModel::Core::CoreApplicationViewTitleBar;
using winrt::Windows::ApplicationModel::Core::CoreApplicationView;


// This simulation itself should ONLY be concerned with the actual atoms
// and/or whatever it is trying to simulate. Therefore, this Main class acts
// as the interface between the Simulation, Simulation Renderer, MoveLookController
// and Menu features. Each of these classes should exist WITHOUT the knowledge
// of any of the other classes.

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

		// TODO: Replace this with your app's content initialization.

		// App Layout - Get it from device resources because it has already been set up there
		m_layout = m_deviceResources->GetLayout();

		// Create the Menu and MenuBar for the app
		m_menu    = std::unique_ptr<Menu>(new Menu(m_deviceResources, m_layout->MenuPaneRectFDIPS()));

		m_menuBar = std::unique_ptr<Menu>(new Menu(m_deviceResources, m_layout->MenuBarPaneRectFDIPS()));
		m_menuBar->BackgroundColor(Theme::MenuBarColor);

		m_titleBar = std::unique_ptr<Menu>(new Menu(m_deviceResources, m_layout->TitleBarPaneRectFDIPS()));
		m_titleBar->BackgroundColor(Theme::MenuBarColor);

		// Simple Cube
		//m_sceneRenderer = std::unique_ptr<Sample3DSceneRenderer>(new Sample3DSceneRenderer(m_deviceResources));
		m_fpsTextRenderer = std::unique_ptr<SampleFpsTextRenderer>(new SampleFpsTextRenderer(m_deviceResources));

		// Sphere
		//m_sphereRenderer = std::unique_ptr<SphereRenderer>(new SphereRenderer(m_deviceResources));

		// Simulation
		m_simulation = std::unique_ptr<Simulation::Simulation>(
			new Simulation::Simulation(m_deviceResources)
		);

		m_moveLookController = std::shared_ptr<MoveLookController>(
			new MoveLookController(
				CoreWindow::GetForCurrentThread(),
				m_simulation->BoxDimensions()
			)
		);

		m_simulationRenderer = std::unique_ptr<Simulation::SimulationRenderer>(
			new Simulation::SimulationRenderer(
				m_deviceResources, 
				m_simulation->BoxDimensions(),
				m_moveLookController
			)
		);

		// Add Controls at the very end because certain controls may need to query the simulation
		AddMenuControls();
		AddMenuBarControls();

		// Pointer/Key Events
		CoreWindow window = CoreWindow::GetForCurrentThread();
		window.PointerPressed({ this, &Main::OnPointerPressed });
		window.PointerMoved({ this, &Main::OnPointerMoved });
		window.PointerReleased({ this, &Main::OnPointerReleased });
		window.KeyDown({ this, &Main::OnKeyDown });
		window.KeyUp({ this, &Main::OnKeyUp });



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
		//m_sceneRenderer->CreateWindowSizeDependentResources();

		// Sphere
		//m_sphereRenderer->CreateWindowSizeDependentResources();

		// Simulation
		m_simulationRenderer->CreateWindowSizeDependentResources();

		// Menu / MenuBar
		m_menu->MenuRect(m_layout->MenuPaneRectFDIPS());
		m_menuBar->MenuRect(m_layout->MenuBarPaneRectFDIPS());
		m_titleBar->MenuRect(m_layout->TitleBarPaneRectFDIPS());
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
				m_simulation->Update(m_timer);
				m_moveLookController->Update(m_timer, m_layout->RenderPaneRectFDIPS());
			});
	}

	// Process all input from the user before updating simulation state
	void Main::ProcessInput()
	{
		// TODO: Add per frame input handling here.

		// Query the MoveLookController for any updates that may have occurred since 
		// the last rendering pass and update the state accordingly

		// Simple Cube
		/*
		if (!m_moveLookController->MouseDown() && m_sceneRenderer->IsTracking())
			m_sceneRenderer->StopTracking();
		else if (m_moveLookController->MouseDown() && !m_sceneRenderer->IsTracking())
			m_sceneRenderer->StartTracking();

		if (m_sceneRenderer->IsTracking())
			m_sceneRenderer->TrackingUpdate(m_moveLookController->MousePositionX());
		*/

		// Simulation
		
		// Query the MoveLookController and see if it is actively moving
		//  - If so, then have it compute a new view matrix and pass it
		//    to the SimulationRenderer
		if (m_moveLookController->IsMoving())
			m_simulationRenderer->SetViewMatrix(m_moveLookController->ViewMatrix());

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
		//m_fpsTextRenderer->Render();

		// Sphere
		//m_sphereRenderer->Render();

		// Simulation
		m_simulationRenderer->Render(m_simulation->Atoms());

		// Render the menu at the end (although it shouldn't really matter)
		m_titleBar->Render();
		m_menu->Render();
		m_menuBar->Render();

		return true;
	}

	// Notifies renderers that device resources need to be released.
	void Main::OnDeviceLost()
	{
		// Simple Cube
		//m_sceneRenderer->ReleaseDeviceDependentResources();
		m_fpsTextRenderer->ReleaseDeviceDependentResources();

		// Sphere
		//m_sphereRenderer->ReleaseDeviceDependentResources();

		// Simulation
		m_simulationRenderer->ReleaseDeviceDependentResources();

		// Menu / Menu Bar
		m_menu->ReleaseDeviceDependentResources();
		m_menuBar->ReleaseDeviceDependentResources();
	}

	// Notifies renderers that device resources may now be recreated.
	void Main::OnDeviceRestored()
	{
		// Simple Cube
		//m_sceneRenderer->CreateDeviceDependentResourcesAsync();
		m_fpsTextRenderer->CreateDeviceDependentResources();

		// Sphere
		//m_sphereRenderer->CreateDeviceDependentResourcesAsync();

		// Simulation
		m_simulationRenderer->BoxDimensions(m_simulation->BoxDimensions());
		m_simulationRenderer->CreateDeviceDependentResourcesAsync();

		// Menu - Set includingControls = true because we want to restore each control as well
		m_menu->CreateDeviceDependentResources(true);
		m_menuBar->CreateDeviceDependentResources(true);

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
		m_windowVisible = visibility;

		if (m_windowVisible)
			m_layout->UpdateLayout();
	}
	void Main::WindowActivationChanged(CoreWindowActivationState)
	{

	}


	// Pointer / Key event handlers
	void Main::OnPointerPressed(CoreWindow w, PointerEventArgs const& e) 
	{ 
		//m_moveLookController->OnPointerPressed(w, e); 

		/*
		* Instead of just passing control to MoveLookController, create some kind of variable to keep
		* track of which pane has 'captured the mouse'. For example, if mouse is pressed and the mouse is 
		* over the render pane, then set a variable within the render pane that says the render pane has
		* captured the mouse. Therefore, when OnPointerMoved fires, if the mouse is now over the Menu, it
		* won't matter because the render pane has priority and therefore the simulation will still rotate
		* 
		* 1. Query each pane to determine if the mouse is captured
		* 2a. If the mouse is captured, just skip to #3
		* 2b. Else, determine which pane the mouse is over & set the capture flag for the corresponding pane
		* 3. Call OnPointerPressed on the corresponding object
		* 		
		*/

		// Determine if a pane has captured the mouse
		if (m_layout->RenderPanePointerCaptured())
			m_moveLookController->OnPointerPressed(w, e);
		else if (m_layout->MenuPanePointerCaptured())
			m_menu->OnPointerPressed(w, e);
		else if (m_layout->MenuBarPanePointerCaptured())
			m_menuBar->OnPointerPressed(w, e);
		else
		{
			// The pointer has not yet been captured, so determine which pane needs
			// to capture it and then call the appropriate OnPointerPressed method

			Point p = e.CurrentPoint().Position();

			if (m_layout->PointerOverRenderPane(p))
			{
				m_layout->RenderPanePointerCaptured(true);
				m_moveLookController->OnPointerPressed(w, e);
			}
			else if (m_layout->PointerOverMenuPane(p))
			{
				m_layout->MenuPanePointerCaptured(true);
				m_menu->OnPointerPressed(w, e);
			}
			else if (m_layout->PointerOverMenuBarPane(p))
			{
				m_layout->MenuBarPanePointerCaptured(true);
				m_menuBar->OnPointerPressed(w, e);
			}
		}
	}

	void Main::OnPointerMoved(CoreWindow w, PointerEventArgs const& e) 
	{ 
		/* If the simulation or one of the panes has pointer priority, call the appropriate method
		*  Else If pointer is pressed
		*		
		*/

		// Determine if a pane has already captured the mouse
		if (m_layout->RenderPanePointerCaptured())
			m_moveLookController->OnPointerMoved(w, e);
		else if (m_layout->MenuPanePointerCaptured())
			m_menu->OnPointerMoved(w, e);
		else if (m_layout->MenuBarPanePointerCaptured())
			m_menuBar->OnPointerMoved(w, e);
		else
		{
			// The pointer has not been captured, but we still need to pass along the 
			// OnPointerMoved call for triggering hover events

			// Must call PointerNotOver method for each menu so controls can revert back to POINTER_NOT_OVER
			// if the pointer moves too fast

			Point p = e.CurrentPoint().Position();

			if (m_layout->PointerOverRenderPane(p))
			{
				// Set the pointer location in movelookcontroller so that the next time it
				// updates, it will rotate the scene (if the pointer is down)
				m_moveLookController->OnPointerMoved(w, e);

				// If the simulation is paused, then we want to determine which atom
				// the pointer is over and update its color accordingly
				if (m_simulation->IsPaused())
					m_simulationRenderer->PointerMoved(p, m_layout->RenderPaneRectFDIPS(), m_simulation->Atoms());

				m_menu->PointerNotOver();
				m_menuBar->PointerNotOver();
			}
			else if (m_layout->PointerOverMenuPane(p))
			{
				m_menu->OnPointerMoved(w, e);
				m_menuBar->PointerNotOver();
			}
			else if (m_layout->PointerOverMenuBarPane(p))
			{
				m_menuBar->OnPointerMoved(w, e);
				m_menu->PointerNotOver();
			}
		}
	}

	void Main::OnPointerReleased(CoreWindow w, PointerEventArgs const& e)
	{ 
		//m_moveLookController->OnPointerReleased(w, e);

		// If the pointer is being released, then it should have been captured
		// So you only need to check to see if is captured
		if (m_layout->RenderPanePointerCaptured())
		{
			m_layout->RenderPanePointerCaptured(false);
			m_moveLookController->OnPointerReleased(w, e);
		}
		else if (m_layout->MenuPanePointerCaptured())
		{
			m_layout->MenuPanePointerCaptured(false);
			m_menu->OnPointerReleased(w, e);
		}
		else if (m_layout->MenuBarPanePointerCaptured())
		{
			m_layout->MenuBarPanePointerCaptured(false);
			m_menuBar->OnPointerReleased(w, e);
		}
	}

	void Main::OnKeyDown(CoreWindow w, KeyEventArgs const& args) 
	{ 
		m_moveLookController->OnKeyDown(w, args); 
	}

	void Main::OnKeyUp(CoreWindow w, KeyEventArgs const& args) 
	{ 
		m_moveLookController->OnKeyUp(w, args); 
	}


	// Add UI Controls
	void Main::AddMenuControls()
	{		
		winrt::com_ptr<TextBox> playText = winrt::make_self<TextBox>(m_deviceResources, m_menu->MenuRect());
		playText->Text(L"Play");
		playText->HorizontalAlignment(ChemLive::HorizontalAlignment::LEFT);
		playText->MarginLeft(5.0f);
		playText->FillParent(true);

		winrt::com_ptr<Button> playButton = winrt::make_self<Button>(m_deviceResources, m_menu->MenuRect());
		playButton->BackgroundColor(D2D1::ColorF::Green);
		playButton->BackgroundColorOnPointerHover(D2D1::ColorF::Yellow);
		playButton->BackgroundColorOnPointerDown(D2D1::ColorF::Purple);
		playButton->Height(20.0f);
		playButton->Width(30.0f);
		playButton->HorizontalAlignment(ChemLive::HorizontalAlignment::STRETCH);
		playButton->VerticalAlignment(ChemLive::VerticalAlignment::TOP);
		playButton->MarginLeft(10.0f);
		playButton->MarginRight(10.0f);
		playButton->MarginTop(20.0f);
		playButton->AddControl(playText);
		playButton->ButtonClick({ this, &Main::SimulationPlayButtonClick });
		playButton->Finalize();

		m_menu->AddControl(playButton);

		winrt::com_ptr<TextBox> pauseTextBox = winrt::make_self<TextBox>(m_deviceResources, m_menu->MenuRect());
		pauseTextBox->Text(L"Pause");
		pauseTextBox->HorizontalAlignment(ChemLive::HorizontalAlignment::LEFT);
		pauseTextBox->MarginLeft(5.0f);
		pauseTextBox->FillParent(true);

		winrt::com_ptr<Button> pauseButton = winrt::make_self<Button>(m_deviceResources, m_menu->MenuRect());
		pauseButton->BackgroundColor(D2D1::ColorF::Green);
		pauseButton->BackgroundColorOnPointerHover(D2D1::ColorF::Yellow);
		pauseButton->BackgroundColorOnPointerDown(D2D1::ColorF::Purple);
		pauseButton->Height(20.0f);
		pauseButton->Width(30.0f);
		pauseButton->HorizontalAlignment(ChemLive::HorizontalAlignment::STRETCH);
		pauseButton->VerticalAlignment(ChemLive::VerticalAlignment::TOP);
		pauseButton->MarginLeft(10.0f);
		pauseButton->MarginRight(10.0f);
		pauseButton->MarginTop(50.0f);
		pauseButton->AddControl(pauseTextBox);
		pauseButton->ButtonClick({ this, &Main::SimulationPauseButtonClick });
		pauseButton->Finalize();

		m_menu->AddControl(pauseButton);


		// Slider ==========================

		// Emmisive 
		winrt::com_ptr<TextBox> emmisiveTextBox = winrt::make_self<TextBox>(m_deviceResources, m_menu->MenuRect());
		emmisiveTextBox->Text(L"Emmissive");
		emmisiveTextBox->HorizontalAlignment(ChemLive::HorizontalAlignment::LEFT);
		emmisiveTextBox->MarginLeft(10.0f);
		emmisiveTextBox->MarginTop(85.0f);
		emmisiveTextBox->FontSize(15.0f);
		emmisiveTextBox->Finalize();

		m_menu->AddControl(emmisiveTextBox);

		winrt::com_ptr<Slider> emmisiveXSlider = winrt::make_self<Slider>(m_deviceResources, m_menu->MenuRect());
		emmisiveXSlider->MarginTop(110.0f);
		emmisiveXSlider->MarginLeft(10.0f);
		emmisiveXSlider->MarginRight(10.0f);
		emmisiveXSlider->TextBoxMaxWidth(85.0f);
		//emmisiveXSlider->Value(m_simulationRenderer->EmmissiveX());
		emmisiveXSlider->SliderMoved({ this, &Main::EmmissiveXSliderMoved });
		emmisiveXSlider->Finalize();

		m_menu->AddControl(emmisiveXSlider);

		winrt::com_ptr<Slider> emmisiveYSlider = winrt::make_self<Slider>(m_deviceResources, m_menu->MenuRect());
		emmisiveYSlider->MarginTop(145.0f);
		emmisiveYSlider->MarginLeft(10.0f);
		emmisiveYSlider->MarginRight(10.0f);
		emmisiveYSlider->TextBoxMaxWidth(85.0f);
		//emmisiveYSlider->Value(m_simulationRenderer->EmmissiveY());
		emmisiveYSlider->SliderMoved({ this, &Main::EmmissiveYSliderMoved });
		emmisiveYSlider->Finalize();

		m_menu->AddControl(emmisiveYSlider);

		winrt::com_ptr<Slider> emmisiveZSlider = winrt::make_self<Slider>(m_deviceResources, m_menu->MenuRect());
		emmisiveZSlider->MarginTop(180.0f);
		emmisiveZSlider->MarginLeft(10.0f);
		emmisiveZSlider->MarginRight(10.0f);
		emmisiveZSlider->TextBoxMaxWidth(85.0f);
		//emmisiveZSlider->Value(m_simulationRenderer->EmmissiveZ());
		emmisiveZSlider->SliderMoved({ this, &Main::EmmissiveZSliderMoved });
		emmisiveZSlider->Finalize();

		m_menu->AddControl(emmisiveZSlider);

		// Ambient 
		winrt::com_ptr<TextBox> ambientTextBox = winrt::make_self<TextBox>(m_deviceResources, m_menu->MenuRect());
		ambientTextBox->Text(L"Ambient");
		ambientTextBox->HorizontalAlignment(ChemLive::HorizontalAlignment::LEFT);
		ambientTextBox->MarginLeft(10.0f);
		ambientTextBox->MarginTop(215.0f);
		ambientTextBox->FontSize(15.0f);
		ambientTextBox->Finalize();

		m_menu->AddControl(ambientTextBox);

		winrt::com_ptr<Slider> ambientXSlider = winrt::make_self<Slider>(m_deviceResources, m_menu->MenuRect());
		ambientXSlider->MarginTop(240.0f);
		ambientXSlider->MarginLeft(10.0f);
		ambientXSlider->MarginRight(10.0f);
		ambientXSlider->TextBoxMaxWidth(85.0f);
		//ambientXSlider->Value(m_simulationRenderer->AmbientX());
		ambientXSlider->SliderMoved({ this, &Main::AmbientXSliderMoved });
		ambientXSlider->Finalize();

		m_menu->AddControl(ambientXSlider);

		winrt::com_ptr<Slider> ambientYSlider = winrt::make_self<Slider>(m_deviceResources, m_menu->MenuRect());
		ambientYSlider->MarginTop(275.0f);
		ambientYSlider->MarginLeft(10.0f);
		ambientYSlider->MarginRight(10.0f);
		ambientYSlider->TextBoxMaxWidth(85.0f);
		//ambientYSlider->Value(m_simulationRenderer->AmbientY());
		ambientYSlider->SliderMoved({ this, &Main::AmbientYSliderMoved });
		ambientYSlider->Finalize();

		m_menu->AddControl(ambientYSlider);

		winrt::com_ptr<Slider> ambientZSlider = winrt::make_self<Slider>(m_deviceResources, m_menu->MenuRect());
		ambientZSlider->MarginTop(310.0f);
		ambientZSlider->MarginLeft(10.0f);
		ambientZSlider->MarginRight(10.0f);
		ambientZSlider->TextBoxMaxWidth(85.0f);
		//ambientZSlider->Value(m_simulationRenderer->AmbientZ());
		ambientZSlider->SliderMoved({ this, &Main::AmbientZSliderMoved });
		ambientZSlider->Finalize();

		m_menu->AddControl(ambientZSlider);

		// Diffuse 
		winrt::com_ptr<TextBox> diffuseTextBox = winrt::make_self<TextBox>(m_deviceResources, m_menu->MenuRect());
		diffuseTextBox->Text(L"Diffuse");
		diffuseTextBox->HorizontalAlignment(ChemLive::HorizontalAlignment::LEFT);
		diffuseTextBox->MarginLeft(10.0f);
		diffuseTextBox->MarginTop(345.0f);
		diffuseTextBox->FontSize(15.0f);
		diffuseTextBox->Finalize();

		m_menu->AddControl(diffuseTextBox);

		winrt::com_ptr<Slider> diffuseXSlider = winrt::make_self<Slider>(m_deviceResources, m_menu->MenuRect());
		diffuseXSlider->MarginTop(380.0f);
		diffuseXSlider->MarginLeft(10.0f);
		diffuseXSlider->MarginRight(10.0f);
		diffuseXSlider->TextBoxMaxWidth(85.0f);
		//diffuseXSlider->Value(m_simulationRenderer->DiffuseX());
		diffuseXSlider->SliderMoved({ this, &Main::DiffuseXSliderMoved });
		diffuseXSlider->Finalize();

		m_menu->AddControl(diffuseXSlider);

		winrt::com_ptr<Slider> diffuseYSlider = winrt::make_self<Slider>(m_deviceResources, m_menu->MenuRect());
		diffuseYSlider->MarginTop(415.0f);
		diffuseYSlider->MarginLeft(10.0f);
		diffuseYSlider->MarginRight(10.0f);
		diffuseYSlider->TextBoxMaxWidth(85.0f);
		//diffuseYSlider->Value(m_simulationRenderer->DiffuseY());
		diffuseYSlider->SliderMoved({ this, &Main::DiffuseYSliderMoved });
		diffuseYSlider->Finalize();

		m_menu->AddControl(diffuseYSlider);

		winrt::com_ptr<Slider> diffuseZSlider = winrt::make_self<Slider>(m_deviceResources, m_menu->MenuRect());
		diffuseZSlider->MarginTop(450.0f);
		diffuseZSlider->MarginLeft(10.0f);
		diffuseZSlider->MarginRight(10.0f);
		diffuseZSlider->TextBoxMaxWidth(85.0f);
		//diffuseZSlider->Value(m_simulationRenderer->DiffuseZ());
		diffuseZSlider->SliderMoved({ this, &Main::DiffuseZSliderMoved });
		diffuseZSlider->Finalize();

		m_menu->AddControl(diffuseZSlider);

		// Diffuse 
		winrt::com_ptr<TextBox> specularTextBox = winrt::make_self<TextBox>(m_deviceResources, m_menu->MenuRect());
		specularTextBox->Text(L"Specular");
		specularTextBox->HorizontalAlignment(ChemLive::HorizontalAlignment::LEFT);
		specularTextBox->MarginLeft(10.0f);
		specularTextBox->MarginTop(485.0f);
		specularTextBox->FontSize(15.0f);
		specularTextBox->Finalize();

		m_menu->AddControl(specularTextBox);

		winrt::com_ptr<Slider> specularXSlider = winrt::make_self<Slider>(m_deviceResources, m_menu->MenuRect());
		specularXSlider->MarginTop(520.0f);
		specularXSlider->MarginLeft(10.0f);
		specularXSlider->MarginRight(10.0f);
		specularXSlider->TextBoxMaxWidth(85.0f);
		//specularXSlider->Value(m_simulationRenderer->SpecularX());
		specularXSlider->SliderMoved({ this, &Main::SpecularXSliderMoved });
		specularXSlider->Finalize();

		m_menu->AddControl(specularXSlider);

		winrt::com_ptr<Slider> specularYSlider = winrt::make_self<Slider>(m_deviceResources, m_menu->MenuRect());
		specularYSlider->MarginTop(555.0f);
		specularYSlider->MarginLeft(10.0f);
		specularYSlider->MarginRight(10.0f);
		specularYSlider->TextBoxMaxWidth(85.0f);
		//specularYSlider->Value(m_simulationRenderer->SpecularY());
		specularYSlider->SliderMoved({ this, &Main::SpecularYSliderMoved });
		specularYSlider->Finalize();

		m_menu->AddControl(specularYSlider);

		winrt::com_ptr<Slider> specularZSlider = winrt::make_self<Slider>(m_deviceResources, m_menu->MenuRect());
		specularZSlider->MarginTop(590.0f);
		specularZSlider->MarginLeft(10.0f);
		specularZSlider->MarginRight(10.0f);
		specularZSlider->TextBoxMaxWidth(85.0f);
		//specularZSlider->Value(m_simulationRenderer->SpecularZ());
		specularZSlider->SliderMoved({ this, &Main::SpecularZSliderMoved });
		specularZSlider->Finalize();

		m_menu->AddControl(specularZSlider);

		// Specular Power 
		winrt::com_ptr<TextBox> specularPowerTextBox = winrt::make_self<TextBox>(m_deviceResources, m_menu->MenuRect());
		specularPowerTextBox->Text(L"Specular Power");
		specularPowerTextBox->HorizontalAlignment(ChemLive::HorizontalAlignment::LEFT);
		specularPowerTextBox->MarginLeft(10.0f);
		specularPowerTextBox->MarginTop(625.0f);
		specularPowerTextBox->FontSize(15.0f);
		specularPowerTextBox->Finalize();

		m_menu->AddControl(specularPowerTextBox);

		winrt::com_ptr<Slider> specularPowerSlider = winrt::make_self<Slider>(m_deviceResources, m_menu->MenuRect());
		specularPowerSlider->MarginTop(660.0f);
		specularPowerSlider->MarginLeft(10.0f);
		specularPowerSlider->MarginRight(10.0f);
		specularPowerSlider->TextBoxMaxWidth(85.0f);
		specularPowerSlider->MinValue(1.0f);
		specularPowerSlider->MaxValue(128.0f);
		//specularPowerSlider->Value(m_simulationRenderer->SpecularPower());
		specularPowerSlider->SliderMoved({ this, &Main::SpecularPowerSliderMoved });
		specularPowerSlider->Finalize();
		
		m_menu->AddControl(specularPowerSlider);
	}

	void Main::AddMenuBarControls()
	{
		// Add File/Edit/View Drop down controls
		winrt::com_ptr<TextBox> fileText = winrt::make_self<TextBox>(m_deviceResources, m_menuBar->MenuRect());
		fileText->Text(L"File");
		fileText->HorizontalAlignment(ChemLive::HorizontalAlignment::CENTER);
		fileText->FillParent(true);

		winrt::com_ptr<Button> fileButton = winrt::make_self<Button>(m_deviceResources, m_menuBar->MenuRect());
		fileButton->BackgroundColor(Theme::MenuBarButtonColor);
		fileButton->BackgroundColorOnPointerHover(Theme::MenuBarButtonColorOnPointerHover);
		fileButton->BackgroundColorOnPointerDown(Theme::MenuBarButtonColorOnPointerDown);
		fileButton->BorderThickness(0.0f);
		fileButton->BorderThicknessOnPointerHover(0.0f);
		fileButton->BorderThicknessOnPointerDown(0.0f);
		fileButton->Height(20.0f);
		fileButton->Width(60.0f);
		fileButton->VerticalAlignment(ChemLive::VerticalAlignment::TOP);
		fileButton->MarginLeft(10.0f);
		fileButton->MarginRight(0.0f);
		fileButton->MarginTop(0.0f);
		fileButton->AddControl(fileText);
		fileButton->Finalize();

		winrt::com_ptr<TextBox> dropDownItemText1 = winrt::make_self<TextBox>(m_deviceResources, m_menuBar->MenuRect());
		dropDownItemText1->Text(L"Item 1");
		dropDownItemText1->HorizontalAlignment(ChemLive::HorizontalAlignment::LEFT);
		dropDownItemText1->MarginLeft(5.0f);
		dropDownItemText1->FillParent(true);

		winrt::com_ptr<Button> dropDownButton1 = winrt::make_self<Button>(m_deviceResources, m_menuBar->MenuRect());
		dropDownButton1->BackgroundColor(Theme::MenuBarButtonColorOnPointerDown);
		dropDownButton1->BackgroundColorOnPointerHover(Theme::MenuBarButtonColorOnPointerHover);
		dropDownButton1->BackgroundColorOnPointerDown(Theme::MenuBarButtonColorOnPointerHover);
		dropDownButton1->BorderThickness(0.0f);
		dropDownButton1->BorderThicknessOnPointerHover(0.0f);
		dropDownButton1->BorderThicknessOnPointerDown(0.0f);
		dropDownButton1->Height(20.0f);
		dropDownButton1->Width(50.0f);
		dropDownButton1->VerticalAlignment(ChemLive::VerticalAlignment::TOP);
		dropDownButton1->AddControl(dropDownItemText1);
		dropDownButton1->Finalize();

		winrt::com_ptr<TextBox> dropDownItemText2 = winrt::make_self<TextBox>(m_deviceResources, m_menuBar->MenuRect());
		dropDownItemText2->Text(L"Item 2");
		dropDownItemText2->HorizontalAlignment(ChemLive::HorizontalAlignment::LEFT);
		dropDownItemText2->MarginLeft(5.0f);
		dropDownItemText2->FillParent(true);

		winrt::com_ptr<Button> dropDownButton2 = winrt::make_self<Button>(m_deviceResources, m_menuBar->MenuRect());
		dropDownButton2->BackgroundColor(Theme::MenuBarButtonColorOnPointerDown);
		dropDownButton2->BackgroundColorOnPointerHover(Theme::MenuBarButtonColorOnPointerHover);
		dropDownButton2->BackgroundColorOnPointerDown(Theme::MenuBarButtonColorOnPointerHover);
		dropDownButton2->BorderThickness(0.0f);
		dropDownButton2->BorderThicknessOnPointerHover(0.0f);
		dropDownButton2->BorderThicknessOnPointerDown(0.0f);
		dropDownButton2->Height(20.0f);
		dropDownButton2->Width(50.0f);
		dropDownButton2->VerticalAlignment(ChemLive::VerticalAlignment::TOP);
		dropDownButton2->AddControl(dropDownItemText2);
		dropDownButton2->Finalize();

		winrt::com_ptr<DropDown> fileDropDown = winrt::make_self<DropDown>(m_deviceResources, m_menuBar->MenuRect());
		fileDropDown->DropDownWidth(100.0f);

		fileDropDown->DropDownBackgroundColor(Theme::MenuBarButtonColorOnPointerDown);
		fileDropDown->DropDownBackgroundColorOnPointerHover(Theme::MenuBarButtonColorOnPointerDown);
		fileDropDown->DropDownBackgroundColorOnPointerDown(Theme::MenuBarButtonColorOnPointerDown);
		fileDropDown->DropDownBorderColor(Theme::MenuBarButtonColorOnPointerDown);
		fileDropDown->DropDownBorderColorOnPointerHover(Theme::MenuBarButtonColorOnPointerDown);
		fileDropDown->DropDownBorderColorOnPointerDown(Theme::MenuBarButtonColorOnPointerDown);

		fileDropDown->SetHeaderButton(fileButton);
		fileDropDown->AddDropDownItem(dropDownButton1);
		fileDropDown->AddDropDownItem(dropDownButton2);

		fileDropDown->Finalize();

		m_menuBar->AddControl(fileDropDown);
	}


	// Button Event Handlers =============================================================
	void Main::SimulationPlayButtonClick(const winrt::Windows::Foundation::IInspectable i, int args)
	{
		m_simulation->PlaySimulation();
	}

	void Main::SimulationPauseButtonClick(const winrt::Windows::Foundation::IInspectable i, int args)
	{
		m_simulation->PauseSimulation();
	}

	// Slider Event Handlers ===========================================================
	void Main::EmmissiveXSliderMoved(const winrt::Windows::Foundation::IInspectable i, float args)
	{
		// args is just the value of the slider (should be 0-1)
		//m_simulationRenderer->EmmissiveX(args);
	}
	void Main::EmmissiveYSliderMoved(const winrt::Windows::Foundation::IInspectable i, float args)
	{
		// args is just the value of the slider (should be 0-1)
		//m_simulationRenderer->EmmissiveY(args);
	}
	void Main::EmmissiveZSliderMoved(const winrt::Windows::Foundation::IInspectable i, float args)
	{
		// args is just the value of the slider (should be 0-1)
		//m_simulationRenderer->EmmissiveZ(args);
	}

	void Main::AmbientXSliderMoved(const winrt::Windows::Foundation::IInspectable, float args)
	{
		// args is just the value of the slider (should be 0-1)
		//m_simulationRenderer->AmbientX(args);
	}
	void Main::AmbientYSliderMoved(const winrt::Windows::Foundation::IInspectable, float args)
	{
		// args is just the value of the slider (should be 0-1)
		//m_simulationRenderer->AmbientY(args);
	}
	void Main::AmbientZSliderMoved(const winrt::Windows::Foundation::IInspectable, float args)
	{
		// args is just the value of the slider (should be 0-1)
		//m_simulationRenderer->AmbientZ(args);
	}

	void Main::DiffuseXSliderMoved(const winrt::Windows::Foundation::IInspectable, float args)
	{
		// args is just the value of the slider (should be 0-1)
		//m_simulationRenderer->DiffuseX(args);
	}
	void Main::DiffuseYSliderMoved(const winrt::Windows::Foundation::IInspectable, float args)
	{
		// args is just the value of the slider (should be 0-1)
		//m_simulationRenderer->DiffuseY(args);
	}
	void Main::DiffuseZSliderMoved(const winrt::Windows::Foundation::IInspectable, float args)
	{
		// args is just the value of the slider (should be 0-1)
		//m_simulationRenderer->DiffuseZ(args);
	}

	void Main::SpecularXSliderMoved(const winrt::Windows::Foundation::IInspectable, float args)
	{
		// args is just the value of the slider (should be 0-1)
		//m_simulationRenderer->SpecularX(args);
	}
	void Main::SpecularYSliderMoved(const winrt::Windows::Foundation::IInspectable, float args)
	{
		// args is just the value of the slider (should be 0-1)
		//m_simulationRenderer->SpecularY(args);
	}
	void Main::SpecularZSliderMoved(const winrt::Windows::Foundation::IInspectable, float args)
	{
		// args is just the value of the slider (should be 0-1)
		//m_simulationRenderer->SpecularZ(args);
	}

	void Main::SpecularPowerSliderMoved(const winrt::Windows::Foundation::IInspectable, float args)
	{
		// args is just the value of the slider (should be 0-1)
		//m_simulationRenderer->SpecularPower(args);
	}
}
