#pragma once

#include "pch.h"
#include "StepTimer.h"
#include "DeviceResources.h"
#include "Layout.h"
#include "Sample3DSceneRenderer.h"
#include "SampleFpsTextRenderer.h"
#include "Menu.h"
#include "MoveLookController.h"
#include "SphereRenderer.h"
#include "Simulation.h"

using DirectX::Sample3DSceneRenderer;
using DirectX::SampleFpsTextRenderer;
using DirectX::SphereRenderer;
using winrt::Windows::UI::Core::CoreProcessEventsOption;
using winrt::Windows::UI::Core::CoreWindow;
using winrt::Windows::UI::Core::CoreWindowActivationState;
using winrt::Windows::UI::Core::PointerEventArgs;

namespace ChemLive
{
	class Main : public DX::IDeviceNotify
	{
	public:
		Main(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~Main();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();

		// Add controls to the UI
		void AddMenuControls();
		void AddMenuBarControls();

		// App Lifecycle Functions - Should only be called from App.cpp
		void Run();
		void Suspend();
		void Resume();
		void Visibility(bool visibility);
		void Close() { m_windowClosed = true; }
		void WindowActivationChanged(CoreWindowActivationState activationState);

		// Pointer / Key event handlers
		void OnPointerPressed(CoreWindow w, PointerEventArgs const& e);
		void OnPointerMoved(CoreWindow w, PointerEventArgs const& e);
		void OnPointerReleased(CoreWindow w, PointerEventArgs const& e);
		void OnKeyDown(CoreWindow w, KeyEventArgs const& args);
		void OnKeyUp(CoreWindow w, KeyEventArgs const& args);

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();



		// Button handlers
		void SimulationPlayButtonClick(const winrt::Windows::Foundation::IInspectable, int);
		void SimulationPauseButtonClick(const winrt::Windows::Foundation::IInspectable, int);

		// Slider Event Handlers
		void EmmissiveXSliderMoved(const winrt::Windows::Foundation::IInspectable, float);
		void EmmissiveYSliderMoved(const winrt::Windows::Foundation::IInspectable, float);
		void EmmissiveZSliderMoved(const winrt::Windows::Foundation::IInspectable, float);
		
		void AmbientXSliderMoved(const winrt::Windows::Foundation::IInspectable, float);
		void AmbientYSliderMoved(const winrt::Windows::Foundation::IInspectable, float);
		void AmbientZSliderMoved(const winrt::Windows::Foundation::IInspectable, float);

		void DiffuseXSliderMoved(const winrt::Windows::Foundation::IInspectable, float);
		void DiffuseYSliderMoved(const winrt::Windows::Foundation::IInspectable, float);
		void DiffuseZSliderMoved(const winrt::Windows::Foundation::IInspectable, float);

		void SpecularXSliderMoved(const winrt::Windows::Foundation::IInspectable, float);
		void SpecularYSliderMoved(const winrt::Windows::Foundation::IInspectable, float);
		void SpecularZSliderMoved(const winrt::Windows::Foundation::IInspectable, float);

		void SpecularPowerSliderMoved(const winrt::Windows::Foundation::IInspectable, float);
		

	private:
		void ProcessInput();

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Shared pointer to the layout for the app panes
		std::shared_ptr<Layout> m_layout;

		// 2D Menu
		std::unique_ptr<Menu> m_menu;
		std::unique_ptr<Menu> m_menuBar;

		// Move/Look Controller
		std::shared_ptr<MoveLookController> m_moveLookController;

		// TODO: Replace with your own content renderers.
		// Simple Cube Renderers
		std::unique_ptr<Sample3DSceneRenderer> m_sceneRenderer;
		std::unique_ptr<SampleFpsTextRenderer> m_fpsTextRenderer;

		// Sphere Renderer
		//std::unique_ptr<SphereRenderer> m_sphereRenderer;

		// Simulation
		std::unique_ptr<Simulation::Simulation> m_simulation;
		std::unique_ptr<Simulation::SimulationRenderer> m_simulationRenderer;

		// Rendering loop timer.
		DX::StepTimer m_timer;

		bool m_windowClosed;
		bool m_windowVisible;
	};
}