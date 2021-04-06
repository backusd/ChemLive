#pragma once

#include "pch.h"
#include "DeviceResources.h"

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
		virtual void CreateDeviceDependentResources(bool includindControls) = 0;
		virtual void ReleaseDeviceDependentResources() = 0;
		
		// Each control could require several updates to it's member variables before its content can be considered "final"
		// Therefore, each control will have a "Finalize" function that set m_finalized = true and creates all of the resources
		// needed for rendering in one pass. This way we are not contiually creating resources every time some control data changes.
		// Lastly, the Render method should first check if the control has been finalized and if not, throw an error
		virtual void Finalize() = 0;

		// Reactive Methods - Most controls will need to react some way when mouse is hovering over the control or is clicked on the control
		virtual void UpdateOnPointerHover() = 0;
		virtual void UpdateOnPointerDown() = 0;
		virtual void PointerNotOver() = 0;

		// Every Control Needs to be able to auto resize itself by either filling height or width of the parent
		void FillParentHeight(bool fill) { m_finalized = false; m_fillParentHeight = fill; }
		void FillParentWidth(bool fill) { m_finalized = false; m_fillParentWidth = fill; }
		void FillParent(bool fill) { m_finalized = false; m_fillParentWidth = fill; m_fillParentHeight = fill; }

		// Set Methods
		void MarginLeft(float margin) { m_finalized = false; m_marginLeft = margin; }
		void MarginRight(float margin) { m_finalized = false; m_marginRight = margin; }
		void MarginTop(float margin) { m_finalized = false; m_marginTop = margin; }
		void MarginBottom(float margin) { m_finalized = false; m_marginBottom = margin; }
		void ParentRect(D2D1_RECT_F rect) { m_finalized = false; m_parentRect = rect; }
		void HorizontalAlignment(ChemLive::HorizontalAlignment alignment) { m_finalized = false; m_horizontalAlignment = alignment; }
		void VerticalAlignment(ChemLive::VerticalAlignment alignment) { m_finalized = false; m_verticalAlignment = alignment; }
		
		// Get Methods
		float MarginLeft() { return m_marginLeft; }
		float MarginRight() { return m_marginRight; }
		float MarginTop() { return m_marginTop; }
		float MarginBottom() { return m_marginBottom; }
		D2D1_RECT_F ParentRect() { return m_parentRect; }
		D2D1_RECT_F ControlRect() { return m_controlRect; }
		ChemLive::HorizontalAlignment HorizontalAlignment() { return m_horizontalAlignment; }
		ChemLive::VerticalAlignment VerticalAlignment() { return m_verticalAlignment; }

		// Get Height / Width
		float Height() { return m_controlRect.bottom - m_controlRect.top; }
		float Width() { return m_controlRect.right - m_controlRect.left; }

		// Pointer Methods
		virtual bool PointerOverControl(Point p);
		virtual bool OnPointerPressed(CoreWindow w, PointerEventArgs const& e) = 0; // returns bool indicating whether or not to capture pointer while pointer is down
		virtual void OnPointerMoved(CoreWindow w, PointerEventArgs const& e) = 0;
		virtual void OnPointerReleased(CoreWindow w, PointerEventArgs const& e) = 0;

		// Other
		bool ControlIsActive() { return m_state != ControlState::DEACTIVATED; }

	protected:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Hold the state of the control
		ControlState m_state;

		// Must be set to true for the control to be rendered
		bool m_finalized;

		// Whether or not to fill parent height/width
		bool m_fillParentHeight;
		bool m_fillParentWidth;

		// Control margins
		float m_marginLeft;
		float m_marginRight;
		float m_marginTop;
		float m_marginBottom;

		// Parent Rect / Location
		D2D1_RECT_F     m_parentRect;
		D2D1_RECT_F		m_controlRect;

		// Alignment
		ChemLive::HorizontalAlignment m_horizontalAlignment;
		ChemLive::VerticalAlignment   m_verticalAlignment;
	};

}