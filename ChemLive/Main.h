#pragma once

#include "pch.h"
#include "StepTimer.h"
#include "DeviceResources.h"
#include "Sample3DSceneRenderer.h"
#include "SampleFpsTextRenderer.h"
#include "MoveLookController.h"
#include "SphereRenderer.h"
#include "Simulation.h"

using DirectX::Sample3DSceneRenderer;
using DirectX::SampleFpsTextRenderer;
using DirectX::SphereRenderer;
using winrt::Windows::UI::Core::CoreProcessEventsOption;
using winrt::Windows::UI::Core::CoreWindow;
using winrt::Windows::UI::Core::CoreWindowActivationState;

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


		// App Lifecycle Functions - Should only be called from App.cpp
		void Run();
		void Suspend();
		void Resume();
		void Visibility(bool visibility);
		void Close() { m_windowClosed = true; }
		void WindowActivationChanged(CoreWindowActivationState activationState);


		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		void ProcessInput();

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		std::unique_ptr<MoveLookController> m_moveLookController;

		// TODO: Replace with your own content renderers.
		// Simple Cube Renderers
		std::unique_ptr<Sample3DSceneRenderer> m_sceneRenderer;
		std::unique_ptr<SampleFpsTextRenderer> m_fpsTextRenderer;

		// Sphere Renderer
		std::unique_ptr<SphereRenderer> m_sphereRenderer;

		// Simulation
		std::unique_ptr<Simulation::Simulation> m_simulation;

		// Rendering loop timer.
		DX::StepTimer m_timer;

		bool m_windowClosed;
		bool m_windowVisible;
	};
}