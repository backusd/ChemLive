#pragma once

#include "Control.h"
#include "DeviceResources.h"
#include "Layout.h"
#include "StepTimer.h"

#include <vector>

using winrt::Windows::UI::Core::CoreWindow;
using winrt::Windows::UI::Core::PointerEventArgs;

namespace ChemLive
{
	class Menu
	{
	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources>    m_deviceResources;

		// List of child controls
		/*
		std::vector<std::shared_ptr<Control>>   m_controls;
		*/
		std::vector<winrt::com_ptr<Control>>    m_controls;
		int										m_controlThatHasCapturedPointerIndex;

		// Resources for rendering
		winrt::com_ptr<ID2D1SolidColorBrush>    m_backgroundBrush;
		winrt::com_ptr<ID2D1DrawingStateBlock1>	m_stateBlock;
		D2D1_RECT_F								m_menuRect;

		// Render helper functions
		void DrawBackground();

	public:
		Menu(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2D1_RECT_F rect);
		void CreateDeviceDependentResources(bool includingControls);
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();

		// Event handlers
		void OnPointerPressed(CoreWindow w, PointerEventArgs const& e);
		void OnPointerMoved(CoreWindow w, PointerEventArgs const& e);
		void OnPointerReleased(CoreWindow w, PointerEventArgs const& e);
		void PointerNotOver();

		// Add Control
		/*
		void AddControl(std::shared_ptr<Control> control) { m_controls.push_back(control); }
		*/
		void AddControl(winrt::com_ptr<Control> control) { m_controls.push_back(control); }

		// Set Methods
		void MenuRect(D2D1_RECT_F rect);
		void BackgroundColor(D2D1::ColorF color); 

		// Get Methods
		D2D1_RECT_F MenuRect() { return m_menuRect; }
	};

}