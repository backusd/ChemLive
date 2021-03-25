#include "pch.h"
#include "Slider.h"



namespace ChemLive
{
	Slider::Slider(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2D1_RECT_F parentRect) :
		Control(deviceResources, parentRect),
		m_sliderRectHeight(30.0f),
		m_sliderRectWidth(15.0f),
		m_sliderRectColor{ D2D1::ColorF::Red, D2D1::ColorF::Pink, D2D1::ColorF::Purple },
		m_sliderRectBorderColor{ D2D1::ColorF::Black, D2D1::ColorF::Black, D2D1::ColorF::Black },
		m_sliderRectBorderThickness{1.0f, 1.0f, 1.0f},
		m_sliderBarColor(D2D1::ColorF::Black),
		m_sliderBarThickness(3.0f),
		m_minValue(0.0f),
		m_maxValue(1.0f),
		m_value(0.0f),
		m_textBoxLocation(SliderTextLocation::RIGHT),
		m_textBoxMaxWidth(100.f)
	{
		// Set a default height and width for the slider to reside in
		Height(25.0f);
		HorizontalAlignment(HorizontalAlignment::STRETCH);

		DX::ThrowIfFailed(
			m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(m_stateBlock.put())
		);

		// Don't trigger for TextBox because it hasn't been created yet
		CreateDeviceDependentResources(false);

		// By default, a control's initial rect is equal to that of the parent rect
		// Therefore, when Slider is initialized, it will simply be set to the size of its
		// parent rect, which is what gets passed along to the TextBox through the control rect

		// Create the TextBox & set initial values
		m_textBox = winrt::make_self<TextBox>(m_deviceResources, m_controlRect);
	}

	void Slider::CreateDeviceDependentResources(bool includingControls)
	{
		if (ControlIsActive())
		{
			// Before you can run .put(), the value must be nullptr
			m_sliderRectBrush = nullptr;
			m_sliderBorderBrush = nullptr;
			m_sliderBarBrush = nullptr;

			DX::ThrowIfFailed(
				m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(m_sliderRectColor[m_state], m_sliderRectBrush.put())
			);

			DX::ThrowIfFailed(
				m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(m_sliderRectBorderColor[m_state], m_sliderBorderBrush.put())
			);

			DX::ThrowIfFailed(
				m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(m_sliderBarColor, m_sliderBarBrush.put())
			);

			if (includingControls)
			{
				// Text Box is the only content control
				m_textBox->CreateDeviceDependentResources(true);
			}
		}
	}

	void Slider::ReleaseDeviceDependentResources()
	{
		m_sliderRectBrush = nullptr;
		m_sliderBorderBrush = nullptr;
		m_sliderBarBrush = nullptr;

		// Call release for all content controls (right now that is just a single TextBox)
		m_textBox->ReleaseDeviceDependentResources();
	}

	void Slider::Finalize()
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

		// Set the parent rect for the textbox, update its data, then finalize it
		m_textBox->ParentRect(m_controlRect);

		if (m_textBoxLocation != SliderTextLocation::COLLAPSED)
		{
			m_textBox->Text(std::to_wstring(m_value));

			// The easiest way to place the text box is going to simply be to let it use the entire
			// slider control rect as its control rect but then update its alignment
			switch (m_textBoxLocation)
			{
			case SliderTextLocation::COLLAPSED:	break;
			case SliderTextLocation::TOP:
				m_textBox->VerticalAlignment(VerticalAlignment::TOP);
				m_textBox->HorizontalAlignment(HorizontalAlignment::CENTER);
				break;
			case SliderTextLocation::RIGHT:
				m_textBox->VerticalAlignment(VerticalAlignment::CENTER);
				m_textBox->HorizontalAlignment(HorizontalAlignment::RIGHT);
				break;
			case SliderTextLocation::BOTTOM:
				m_textBox->VerticalAlignment(VerticalAlignment::BOTTOM);
				m_textBox->HorizontalAlignment(HorizontalAlignment::CENTER);
				break;
			case SliderTextLocation::LEFT:
				m_textBox->VerticalAlignment(VerticalAlignment::CENTER);
				m_textBox->HorizontalAlignment(HorizontalAlignment::LEFT);
				break;
			default:
				throw std::invalid_argument("Unknown value for SliderTextLocation enum");
			}

			m_textBox->Finalize();
		}

		m_finalized = true;
	}

	void Slider::Render()
	{
		if (ControlIsActive())
		{
			if (!m_finalized)
				throw std::invalid_argument("Slider has not been finalized");

			// Compute slider bar end points
			m_sliderBarLeft.y = m_controlRect.top + ((m_controlRect.bottom - m_controlRect.top) / 2.0f);
			m_sliderBarLeft.x = m_controlRect.left;
			m_sliderBarRight.x = m_controlRect.right;

			// Margin Between the slider bar itself and the text
			float margin = 10.0f;

			if (m_textBoxLocation == SliderTextLocation::LEFT)        m_sliderBarLeft.x += m_textBoxMaxWidth;
			else if (m_textBoxLocation == SliderTextLocation::RIGHT)  m_sliderBarRight.x -= m_textBoxMaxWidth;
			else if (m_textBoxLocation == SliderTextLocation::TOP)    m_sliderBarLeft.y += (m_textBox->TextHeight() + margin);
			else if (m_textBoxLocation == SliderTextLocation::BOTTOM) m_sliderBarLeft.y -= (m_textBox->TextHeight() + margin);

			m_sliderBarRight.y = m_sliderBarLeft.y;

			// Create the slider rect
			float halfSliderHeight = m_sliderRectHeight / 2.0f;
			float halfSliderWidth = m_sliderRectWidth / 2.0f;

			m_sliderRect.bottom = m_sliderBarLeft.y + halfSliderHeight;
			m_sliderRect.top = m_sliderBarLeft.y - halfSliderHeight;

			m_sliderRect.left = m_value / (m_maxValue - m_minValue);   // percentage from left
			m_sliderRect.left *= (m_sliderBarRight.x - m_sliderBarLeft.x); // pixels from left
			m_sliderRect.left += m_sliderBarLeft.x;						 // true center of slider rect
			m_sliderRect.left -= halfSliderWidth;						 // true left side of the slider rect
			m_sliderRect.right = m_sliderRect.left + m_sliderRectWidth;  // true right side of the slider rect

			// Draw Slider Bar ==========================================================================

			ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext();
			context->SaveDrawingState(m_stateBlock.get());
			context->BeginDraw();
			context->SetTransform(m_deviceResources->GetOrientationTransform2D());

			context->DrawLine(m_sliderBarLeft, m_sliderBarRight, m_sliderBarBrush.get(), m_sliderBarThickness);

			// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
			// is lost. It will be handled during the next call to Present.
			HRESULT hr = context->EndDraw();
			if (hr != D2DERR_RECREATE_TARGET)
			{
				DX::ThrowIfFailed(hr);
			}

			context->RestoreDrawingState(m_stateBlock.get());

			// Draw Slider Rect ==========================================================================

			context->SaveDrawingState(m_stateBlock.get());
			context->BeginDraw();
			context->SetTransform(m_deviceResources->GetOrientationTransform2D());

			context->FillRectangle(m_sliderRect, m_sliderRectBrush.get());

			// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
			// is lost. It will be handled during the next call to Present.
			hr = context->EndDraw();
			if (hr != D2DERR_RECREATE_TARGET)
			{
				DX::ThrowIfFailed(hr);
			}

			context->RestoreDrawingState(m_stateBlock.get());

			// Draw Slider rect border ====================================================================

			context->SaveDrawingState(m_stateBlock.get());
			context->BeginDraw();
			context->SetTransform(m_deviceResources->GetOrientationTransform2D());

			context->DrawRectangle(m_sliderRect, m_sliderBorderBrush.get(), m_sliderRectBorderThickness[m_state]);

			// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
			// is lost. It will be handled during the next call to Present.
			hr = context->EndDraw();
			if (hr != D2DERR_RECREATE_TARGET)
			{
				DX::ThrowIfFailed(hr);
			}

			context->RestoreDrawingState(m_stateBlock.get());

			// Draw Text ===================================================================================
			m_textBox->Render();
		}
	}

	void Slider::UpdateOnPointerHover()
	{

	}

	void Slider::UpdateOnPointerDown()
	{

	}

	void Slider::PointerNotOver()
	{
		if (m_state == ControlState::POINTER_HOVER || m_state == ControlState::POINTER_DOWN)
		{
			m_state = ControlState::POINTER_NOT_OVER;
			Finalize();
			
			m_textBox->PointerNotOver();
		}
	}

	bool Slider::OnPointerPressed(CoreWindow w, PointerEventArgs const& e)
	{
		// The PointerOverControl method does not set the POINTER_DOWN state, so it must be set here
		m_state = ControlState::POINTER_DOWN;
		Finalize();

		// Could trigger a custom event so that we can assign a custom handler to each control
		//
		// ...
		//

		// return true to capture pointer until it has been released
		return true;
	}

	void Slider::OnPointerMoved(CoreWindow w, PointerEventArgs const& e)
	{
		if (m_state == ControlState::POINTER_DOWN)
		{
			// All we need to do is update the value of the slider
			Point p = e.CurrentPoint().Position();

			if (p.X < m_sliderBarLeft.x)
				m_value = m_minValue;
			else if (p.X > m_sliderBarRight.x)
				m_value = m_maxValue;
			else
			{
				float percentFromLeft = (p.X - m_sliderBarLeft.x) / (m_sliderBarRight.x - m_sliderBarLeft.x);
				m_value = percentFromLeft * (m_maxValue - m_minValue) + m_minValue;
			}
				

			Finalize();
			FireSliderMoved();
		}
	}

	void Slider::OnPointerReleased(CoreWindow w, PointerEventArgs const& e)
	{
		// Because the control can capture the pointer, it is possible that this method is
		// being called even if the pointer is not over the control, so you must check for that

		if (PointerOverControl(e.CurrentPoint().Position()))
			m_state = ControlState::POINTER_HOVER;
		else
			m_state = ControlState::POINTER_NOT_OVER;

		Finalize();
	}

	bool Slider::PointerOverControl(Point p)
	{
		if (p.X > m_sliderRect.left && p.X < m_sliderRect.right && p.Y > m_sliderRect.top && p.Y < m_sliderRect.bottom)
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



	// TextBox Pass Through Methods =============================================

	void Slider::TextFontWeight(DWRITE_FONT_WEIGHT fontWeight)
	{
		m_finalized = false;
		m_textBox->FontWeight(fontWeight);
		m_textBox->FontWeightOnPointerHover(fontWeight);
		m_textBox->FontWeightOnPointerDown(fontWeight);
	}
	void Slider::TextFontStyle(DWRITE_FONT_STYLE fontStyle)
	{
		m_finalized = false;
		m_textBox->FontStyle(fontStyle);
		m_textBox->FontStyleOnPointerHover(fontStyle);
		m_textBox->FontStyleOnPointerDown(fontStyle);
	}
	void Slider::TextFontSize(float fontSize)
	{
		m_finalized = false;
		m_textBox->FontSize(fontSize);
		m_textBox->FontSizeOnPointerHover(fontSize);
		m_textBox->FontSizeOnPointerDown(fontSize);
	}
	void Slider::TextColor(D2D1::ColorF color)
	{
		m_finalized = false;
		m_textBox->TextColor(color);
		m_textBox->TextColorOnPointerHover(color);
		m_textBox->TextColorOnPointerDown(color);
	}


	// Slider Events ================================================================
	winrt::event_token Slider::SliderMoved(EventHandler<float> const& handler)
	{
		return m_sliderMovedEvent.add(handler);
	}

	void Slider::SliderMoved(winrt::event_token const& token) noexcept
	{
		m_sliderMovedEvent.remove(token);
	}

	void Slider::FireSliderMoved()
	{
		// Just pass the current slider value
		float args = m_value;
		m_sliderMovedEvent(*this, args);
	}
}