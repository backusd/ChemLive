#include "pch.h"
#include "Control.h"

namespace ChemLive
{
	Control::Control(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2D1_RECT_F parentRect) :
		m_deviceResources(deviceResources),
		m_state(ControlState::POINTER_NOT_OVER),
		m_finalized(false),
		m_marginLeft(0.0f),
		m_marginRight(0.0f),
		m_marginTop(0.0f),
		m_marginBottom(0.0f),
		m_parentRect(parentRect),
		m_controlRect(parentRect),
		m_horizontalAlignment(ChemLive::HorizontalAlignment::LEFT),
		m_verticalAlignment(ChemLive::VerticalAlignment::TOP),
		m_fillParentHeight(false),
		m_fillParentWidth(false)
	{
	}

	bool Control::PointerOverControl(Point p)
	{ 
		if (p.X > m_controlRect.left && p.X < m_controlRect.right && p.Y > m_controlRect.top && p.Y < m_controlRect.bottom)
		{
			// Only change the state if we are in POINTER_NOT_OVER
			if (m_state == ControlState::POINTER_NOT_OVER)
			{
				m_state = ControlState::POINTER_HOVER;
				Finalize();
			}

			return true;
		}

		// If the pointer was previously POINTER_HOVER, then update to POINTER_NOT_OVER
		if (m_state == ControlState::POINTER_HOVER || m_state == ControlState::POINTER_DOWN)
		{
			m_state = ControlState::POINTER_NOT_OVER;
			Finalize();
		}

		return false;
	}

}