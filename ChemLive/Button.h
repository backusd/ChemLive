#pragma once

#include "Control.h"
#include <vector>

using winrt::Windows::Foundation::EventHandler;

namespace ChemLive
{
	class Button : public Control
	{
	private:
		std::vector<winrt::com_ptr<Control>> m_content;

		D2D1::ColorF m_backgroundColor[3];
		D2D1::ColorF m_borderColor[3];
		float        m_borderThickness[3];

		// Resources for rendering
		winrt::com_ptr<ID2D1SolidColorBrush>    m_backgroundBrush;
		winrt::com_ptr<ID2D1SolidColorBrush>    m_borderBrush;
		winrt::com_ptr<ID2D1DrawingStateBlock1>	m_stateBlock;

		void DrawBackground();
		void DrawBorder();
		void DrawContent();

	public:
		Button(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2D1_RECT_F parentRect);

		// Virtual functions from Control class
		void Render();
		void CreateDeviceDependentResources(bool includingControls);
		void ReleaseDeviceDependentResources();
		void UpdateOnPointerHover();
		void UpdateOnPointerDown();
		void Finalize();
		void PointerNotOver();

		bool OnPointerPressed(CoreWindow w, PointerEventArgs const& e);
		void OnPointerMoved(CoreWindow w, PointerEventArgs const& e);
		void OnPointerReleased(CoreWindow w, PointerEventArgs const& e);
		

		// Add Content control
		void AddControl(winrt::com_ptr<Control> control) { m_content.push_back(control); }

		// Set Height/Width of the button
		void Height(float height) { m_finalized = false; m_controlRect.bottom = m_controlRect.top + height; }
		void Width(float width) { m_finalized = false; m_controlRect.right = m_controlRect.left + width; }

		// Set Methods
		void BackgroundColor(D2D1::ColorF color) { m_finalized = false; m_backgroundColor[0] = color; }
		void BackgroundColorOnPointerHover(D2D1::ColorF color) { m_finalized = false; m_backgroundColor[1] = color; }
		void BackgroundColorOnPointerDown(D2D1::ColorF color) { m_finalized = false; m_backgroundColor[2] = color; }

		void BorderColor(D2D1::ColorF color) { m_finalized = false; m_borderColor[0] = color; }
		void BorderColorOnPointerHover(D2D1::ColorF color) { m_finalized = false; m_borderColor[1] = color; }
		void BorderColorOnPointerDown(D2D1::ColorF color) { m_finalized = false; m_borderColor[2] = color; }

		void BorderThickness(float thickness) { m_finalized = false; m_borderThickness[0] = thickness; }
		void BorderThicknessOnPointerHover(float thickness) { m_finalized = false; m_borderThickness[1] = thickness; }
		void BorderThicknessOnPointerDown(float thickness) { m_finalized = false; m_borderThickness[2] = thickness; }

		// Get Methods
		D2D1::ColorF BackgroundColor() { return m_backgroundColor[0]; }
		D2D1::ColorF BackgroundColorOnPointerHover() { return m_backgroundColor[1]; }
		D2D1::ColorF BackgroundColorOnPointerDown() { return m_backgroundColor[2]; }

		D2D1::ColorF BorderColor() { return m_borderColor[0]; }
		D2D1::ColorF BorderColorOnPointerHover() { return m_borderColor[1]; }
		D2D1::ColorF BorderColorOnPointerDown() { return m_borderColor[2]; }

		float BorderThickness() { return m_borderThickness[0]; }
		float BorderThicknessOnPointerHover() { return m_borderThickness[1]; }
		float BorderThicknessOnPointerDown() { return m_borderThickness[2]; }

		// Button events		
		winrt::event_token ButtonClick(EventHandler<int> const& handler);
		void ButtonClick(winrt::event_token const& token) noexcept;
		winrt::event<EventHandler<int>> m_buttonClickEvent;

		void FireButtonClick();
		
	};

}