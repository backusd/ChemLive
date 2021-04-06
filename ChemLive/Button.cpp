#include "pch.h"
#include "Button.h"


namespace ChemLive
{
	Button::Button(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2D1_RECT_F parentRect) :
		Control(deviceResources, parentRect),
		m_backgroundColor{ D2D1::ColorF::Gray, D2D1::ColorF::Gray, D2D1::ColorF::Gray },
		m_borderColor{ D2D1::ColorF::Black, D2D1::ColorF::Black, D2D1::ColorF::Black },
		m_borderThickness{ 1.0f, 1.0f, 1.0f }
	{
		DX::ThrowIfFailed(
			m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(m_stateBlock.put())
		);
	}
	
	void Button::CreateDeviceDependentResources(bool includingControls)
	{
		if (ControlIsActive())
		{
			// Before you can run .put(), the value must be nullptr
			m_backgroundBrush = nullptr;
			m_borderBrush = nullptr;

			DX::ThrowIfFailed(
				m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(m_backgroundColor[m_state], m_backgroundBrush.put())
			);

			DX::ThrowIfFailed(
				m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(m_borderColor[m_state], m_borderBrush.put())
			);

			if (includingControls)
			{
				for (unsigned int iii = 0; iii < m_content.size(); ++iii)
					m_content[iii]->CreateDeviceDependentResources(true);
			}
		}
	}

	void Button::ReleaseDeviceDependentResources()
	{
		m_backgroundBrush = nullptr;
		m_borderBrush = nullptr;

		// Call release for each control as well
		for (unsigned int iii = 0; iii < m_content.size(); ++iii)
			m_content[iii]->ReleaseDeviceDependentResources();
	}

	void Button::Finalize()
	{
		float height = m_controlRect.bottom - m_controlRect.top;
		float width = m_controlRect.right - m_controlRect.left;

		switch (m_verticalAlignment)
		{
		case ChemLive::VerticalAlignment::STRETCH:
			// In the STRETCH case, make the button as tall as possible while still adjusting for margins
			// This takes precedence over any Height/Width settings 
			m_controlRect.top = m_parentRect.top + m_marginTop;
			m_controlRect.bottom = m_parentRect.bottom - m_marginBottom;
			break;
		case ChemLive::VerticalAlignment::TOP:
			// Then set the new top and bottom
			m_controlRect.top = m_parentRect.top + m_marginTop;
			m_controlRect.bottom = m_controlRect.top + height;
			break;
		case ChemLive::VerticalAlignment::BOTTOM:
			// From the bottom, go up the bottom margin plus the text height
			m_controlRect.bottom = m_parentRect.bottom - m_marginBottom;
			m_controlRect.top = m_controlRect.bottom - height;
			break;
		case ChemLive::VerticalAlignment::CENTER:
			// Compute the center of the parent rect 
			// then go up half the height of the button rect
			// Ignore all margins
			m_controlRect.top = m_parentRect.top + ((m_parentRect.bottom - m_parentRect.top) / 2) - (height / 2);
			m_controlRect.bottom = m_controlRect.top + height;
			break;
		default:
			throw std::invalid_argument("Unknown value for m_verticalAlignment enum");
		}

		switch (m_horizontalAlignment)
		{
		case ChemLive::HorizontalAlignment::STRETCH:
			// In the STRETCH case, make the button as wide as possible while still adjusting for margins
			// This takes precedence over any Height/Width settings 
			m_controlRect.left = m_parentRect.left + m_marginLeft;
			m_controlRect.right = m_parentRect.right - m_marginRight;
			break;
		case ChemLive::HorizontalAlignment::LEFT:
			// Then set the new top and bottom
			m_controlRect.left = m_parentRect.left + m_marginLeft;
			m_controlRect.right = m_controlRect.left + width;
			break;
		case ChemLive::HorizontalAlignment::RIGHT:
			// From the bottom, go up the bottom margin plus the text height
			m_controlRect.right = m_parentRect.right - m_marginRight;
			m_controlRect.left = m_controlRect.right - width;
			break;
		case ChemLive::HorizontalAlignment::CENTER:
			// Compute the center of the parent rect 
			// then go right half the width of the button rect
			// Ignore all margins
			m_controlRect.left = m_parentRect.left + ((m_parentRect.right - m_parentRect.left) / 2) - (width / 2);
			m_controlRect.right = m_controlRect.left + width;
			break;
		default:
			throw std::invalid_argument("Unknown value for m_verticalAlignment enum");
		}

		// Don't need to recursively create device dependent resources here because the call to Finalize in a few lines
		// should create the resources for each control anyways
		CreateDeviceDependentResources(false);

		// Pass the button rect to the content controls and finalize them as well
		for (unsigned int iii = 0; iii < m_content.size(); ++iii)
		{
			//m_content[iii]->ParentRect(m_buttonRect);
			m_content[iii]->ParentRect(m_controlRect);
			m_content[iii]->Finalize();
		}

		m_finalized = true;
	}

	void Button::Render()
	{
		if (ControlIsActive())
		{
			if (!m_finalized)
				throw std::invalid_argument("TextBox has not been finalized");

			DrawBackground();
			DrawBorder();
			DrawContent();
		}
	}

	void Button::DrawBackground()
	{
		ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext();
		context->SaveDrawingState(m_stateBlock.get());
		context->BeginDraw();
		context->SetTransform(m_deviceResources->GetOrientationTransform2D());
		
		
		//context->FillRectangle(&m_buttonRect, m_backgroundBrush.get());
		context->FillRectangle(&m_controlRect, m_backgroundBrush.get());

		// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
		// is lost. It will be handled during the next call to Present.
		HRESULT hr = context->EndDraw();
		if (hr != D2DERR_RECREATE_TARGET)
		{
			DX::ThrowIfFailed(hr);
		}

		context->RestoreDrawingState(m_stateBlock.get());
	}

	void Button::DrawBorder()
	{
		if (m_borderThickness[m_state] > 0.01f)
		{
			ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext();
			context->SaveDrawingState(m_stateBlock.get());
			context->BeginDraw();
			context->SetTransform(m_deviceResources->GetOrientationTransform2D());
			
			
			//context->DrawRectangle(&m_buttonRect, m_borderBrush.get(), m_borderThickness[m_state]);
			context->DrawRectangle(&m_controlRect, m_borderBrush.get(), m_borderThickness[m_state]);


			// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
			// is lost. It will be handled during the next call to Present.
			HRESULT hr = context->EndDraw();
			if (hr != D2DERR_RECREATE_TARGET)
			{
				DX::ThrowIfFailed(hr);
			}

			context->RestoreDrawingState(m_stateBlock.get());
		}
	}

	void Button::DrawContent()
	{
		for (unsigned int iii = 0; iii < m_content.size(); ++iii)
			m_content[iii]->Render();
	}

	void Button::UpdateOnPointerHover()
	{

	}

	void Button::UpdateOnPointerDown()
	{

	}

	
	winrt::event_token Button::ButtonClick(EventHandler<int> const& handler)
	{
		return m_buttonClickEvent.add(handler);
	}

	void Button::ButtonClick(winrt::event_token const& token) noexcept
	{
		m_buttonClickEvent.remove(token);
	}

	void Button::FireButtonClick()
	{
		int args = 0;
		m_buttonClickEvent(*this, args);
	}
	

	bool Button::OnPointerPressed(CoreWindow w, PointerEventArgs const& e)
	{
		// The Control::PointerOverControl method does not set the POINTER_DOWN state, so it must be set here
		m_state = ControlState::POINTER_DOWN;
		Finalize();

		// Could trigger a custom event so that we can assign a custom handler to each control
		//
		// ...
		//

		// return false to indicate we do not want to capture the pointer while it is down
		return false;
	}

	void Button::OnPointerMoved(CoreWindow w, PointerEventArgs const& e)
	{
		// The state should already have been updated in the Control::PointerOverControl method

		// Could trigger a custom event so that we can assign a custom handler to each control
		//
		// ...
		//
	}

	void Button::OnPointerReleased(CoreWindow w, PointerEventArgs const& e)
	{
		// The only reason this method should get called is if the pointer is actively over the control
		// therefore we can update the state to POINT_HOVER and trigger the Click event
		m_state = ControlState::POINTER_HOVER;
		Finalize();

		// Tigger ButtonClick event
		FireButtonClick();
	}

	void Button::PointerNotOver()
	{
		if (m_state == ControlState::POINTER_HOVER || m_state == ControlState::POINTER_DOWN)
		{
			m_state = ControlState::POINTER_NOT_OVER;
			Finalize();

			for (unsigned int iii = 0; iii < m_content.size(); ++iii)
				m_content[iii]->PointerNotOver();
		}
	}
}