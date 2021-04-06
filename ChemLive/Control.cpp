#include "pch.h"
#include "Control.h"

namespace ChemLive
{
	Control::Control(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2D1_RECT_F parentRect) :
		m_deviceResources(deviceResources),
		m_state(ControlState::POINTER_NOT_OVER),
		m_parentRect(parentRect),
		m_controlRect(parentRect),
		m_horizontalAlignment(ChemLive::HorizontalAlignment::LEFT),
		m_verticalAlignment(ChemLive::VerticalAlignment::TOP),
		m_margin{0.0f, 0.0f, 0.0f, 0.0f}
	{
	}

	bool Control::PointerOverControl(Point p)
	{ 
		if (p.X > m_controlRect.left && p.X < m_controlRect.right && p.Y > m_controlRect.top && p.Y < m_controlRect.bottom)
		{
			// Only change the state if we are in POINTER_NOT_OVER
			if (m_state == ControlState::POINTER_NOT_OVER)
				UpdateState(ControlState::POINTER_HOVER);
			
			return true;
		}

		// If the pointer was previously POINTER_HOVER, then update to POINTER_NOT_OVER
		if (m_state == ControlState::POINTER_HOVER || m_state == ControlState::POINTER_DOWN)
			UpdateState(ControlState::POINTER_NOT_OVER);

		return false;
	}

	void Control::UpdateControlRect()
	{
		// Keep the same height and width values
		UpdateControlRect(m_controlRect.bottom - m_controlRect.top, m_controlRect.right - m_controlRect.left);
	}
	void Control::UpdateControlRect(float height, float width)
	{
		switch (m_horizontalAlignment)
		{
		case HorizontalAlignment::STRETCH:
			// In the stretch case, ignore the original width and stretch the control rect as much as
			// possible while still obeying margins
			m_controlRect.left  = m_parentRect.left  + m_margin.left;
			m_controlRect.right = m_parentRect.right - m_margin.right;
			break;
		case HorizontalAlignment::LEFT:
			// In the left case, keep the original width and align to the left of the parent rect while
			// stilling obeying the left margin
			m_controlRect.left  = m_parentRect.left  + m_margin.left;
			m_controlRect.right = m_controlRect.left + width;
			break;
		case HorizontalAlignment::RIGHT:
			// In the right case, keep the original width and align to the right of the parent rect while
			// stilling obeying the right margin
			m_controlRect.right = m_parentRect.right  - m_margin.right;
			m_controlRect.left  = m_controlRect.right - width;
			break;
		case HorizontalAlignment::CENTER:
			// In the center case, we want to keep the original width and center it within the parent rect
			// This will completely ignore margins
			// left = middle - (width / 2), middle = parent.left + (parent.width / 2), parent.width = parent.right - parent.left
			//float halfParentWidth = (m_parentRect.right - m_parentRect.left) / 2.0f;
			//float middle = m_parentRect.left + halfParentWidth;
			m_controlRect.left  = m_parentRect.left + ((m_parentRect.right - m_parentRect.left) / 2.0f) - (width / 2.0f);
			m_controlRect.right = m_controlRect.left + width;
			break;
		default:
			throw std::invalid_argument("Control::UpdateControlRect - Unknown HorizontalAlignment value");
		}

		switch (m_verticalAlignment)
		{
		case VerticalAlignment::STRETCH:
			// In the stretch case, ignore the original height and stretch the control rect as much as
			// possible while still obeying margins
			m_controlRect.top = m_parentRect.top + m_margin.top;
			m_controlRect.bottom = m_parentRect.bottom - m_margin.bottom;
			break;
		case VerticalAlignment::TOP:
			// In the top case, keep the original height and align to the top of the parent rect while
			// stilling obeying the top margin
			m_controlRect.top = m_parentRect.top + m_margin.top;
			m_controlRect.bottom = m_controlRect.top + height;
			break;
		case VerticalAlignment::BOTTOM:
			// In the bottom case, keep the original height and align to the bottom of the parent rect while
			// stilling obeying the bottom margin
			m_controlRect.bottom = m_parentRect.bottom - m_margin.bottom;
			m_controlRect.top = m_controlRect.bottom - height;
			break;
		case VerticalAlignment::CENTER:
			// In the center case, we want to keep the original height and center it within the parent rect
			// This will completely ignore margins
			// top = middle - (height / 2), middle = parent.top + (parent.height / 2), parent.height = parent.bottom - parent.top
			//float halfParentHeight = (m_parentRect.bottom - m_parentRect.top) / 2.0f;
			//float middle = m_parentRect.top + halfParentHeight;
			m_controlRect.top = m_parentRect.top + ((m_parentRect.bottom - m_parentRect.top) / 2.0f) - (height / 2.0f);
			m_controlRect.bottom = m_controlRect.top + height;
			break;
		default:
			throw std::invalid_argument("Control::UpdateControlRect - Unknown VerticalAlignment value");
		}

		// Call the pure virtual function to signal that the control rect may have been changed
		ControlRectUpdated();
	}

	// Margin updates should also update the control rect
	void Control::MarginLeft(float margin) 
	{ 
		m_margin.left = margin;
		UpdateControlRect();
	}
	void Control::MarginRight(float margin) 
	{ 
		m_margin.right = margin;
		UpdateControlRect();
	}
	void Control::MarginTop(float margin) 
	{ 
		m_margin.top = margin;
		UpdateControlRect();
	}
	void Control::MarginBottom(float margin) 
	{ 
		m_margin.bottom = margin;
		UpdateControlRect();
	}
	void Control::Margin(float l, float t, float r, float b) 
	{ 
		m_margin.left = l; 
		m_margin.top = t; 
		m_margin.right = r; 
		m_margin.bottom = b;
		UpdateControlRect();
	}
	void Control::ParentRect(D2D1_RECT_F rect) 
	{ 
		m_parentRect = rect;
		UpdateControlRect();
	}
	void Control::HorizontalAlignment(ChemLive::HorizontalAlignment alignment) 
	{ 
		m_horizontalAlignment = alignment;
		UpdateControlRect();
	}
	void Control::VerticalAlignment(ChemLive::VerticalAlignment alignment) 
	{ 
		m_verticalAlignment = alignment;
		UpdateControlRect();
	}


}