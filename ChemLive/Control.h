#pragma once

#include "pch.h"
#include "DeviceResources.h"
#include <stdexcept>

using winrt::Windows::Foundation::Point;
using winrt::Windows::UI::Core::CoreWindow;
using winrt::Windows::UI::Core::PointerEventArgs;

namespace ChemLive
{
	enum class HorizontalAlignment
	{
		LEFT,
		CENTER,
		RIGHT,
		STRETCH
	};
	enum class VerticalAlignment
	{
		TOP,
		CENTER,
		BOTTOM,
		STRETCH
	};
	enum ControlState
	{
		POINTER_NOT_OVER = 0,
		POINTER_HOVER = 1,
		POINTER_DOWN = 2,
		DEACTIVATED = 3
	};

	class Control : public winrt::implements<Control, winrt::Windows::Foundation::IInspectable>
	{
	public:
		Control(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2D1_RECT_F parentRect);

		virtual void Render() = 0;
		virtual void CreateDeviceDependentResources() = 0;
		virtual void ReleaseDeviceDependentResources() = 0;

		// This function must implement the following:
		// 1. Set m_state = state
		// 2. Perform all necessary variable updates for the new state
		virtual void UpdateState(ControlState state) = 0;

		// This function is called whenever the control rect has been updated
		// Each control must implement all necessary updates given the the size/location
		// of the control rect may now be different
		virtual void ControlRectUpdated() = 0;

		// Pointer Methods
		virtual bool PointerOverControl(Point p);
		virtual bool OnPointerPressed(CoreWindow w, PointerEventArgs const& e) = 0; // returns bool indicating whether or not to capture pointer while pointer is down
		virtual void OnPointerMoved(CoreWindow w, PointerEventArgs const& e) = 0;
		virtual void OnPointerReleased(CoreWindow w, PointerEventArgs const& e) = 0;
		virtual void PointerNotOver() = 0;

		// Set Methods
		void MarginLeft(float margin);
		void MarginRight(float margin);
		void MarginTop(float margin);
		void MarginBottom(float margin);
		void Margin(float l, float t, float r, float b);

		void ParentRect(D2D1_RECT_F rect);
		void HorizontalAlignment(ChemLive::HorizontalAlignment alignment);
		void VerticalAlignment(ChemLive::VerticalAlignment alignment);

		// Get Methods
		float MarginLeft() { return m_margin.left; }
		float MarginRight() { return m_margin.right; }
		float MarginTop() { return m_margin.top; }
		float MarginBottom() { return m_margin.bottom; }	
		D2D1_RECT_F ParentRect() { return m_parentRect; }
		D2D1_RECT_F ControlRect() { return m_controlRect; }
		ChemLive::HorizontalAlignment HorizontalAlignment() { return m_horizontalAlignment; }
		ChemLive::VerticalAlignment VerticalAlignment() { return m_verticalAlignment; }
		
		// Get Height / Width
		float ControlRectHeight() { return m_controlRect.bottom - m_controlRect.top; }
		float ControlRectWidth() { return m_controlRect.right - m_controlRect.left; }

		// Other
		void Deactivate() { UpdateState(ControlState::DEACTIVATED); }
		void Activate() { UpdateState(ControlState::POINTER_NOT_OVER); }
		bool ControlIsActive() { return m_state != ControlState::DEACTIVATED; }

	protected:
		void UpdateControlRect(float height, float width);
		void UpdateControlRect();


		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Hold the state of the control
		ControlState m_state;

		// Control margins
		D2D1_RECT_F m_margin;

		// Parent Rect / Location
		D2D1_RECT_F m_parentRect;
		D2D1_RECT_F	m_controlRect;

		// Alignment
		ChemLive::HorizontalAlignment m_horizontalAlignment;
		ChemLive::VerticalAlignment   m_verticalAlignment;
	};

}