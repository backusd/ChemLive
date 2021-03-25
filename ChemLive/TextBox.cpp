#include "pch.h"
#include "TextBox.h"
#include "DirectXHelper.h"
#include <stdexcept>

namespace ChemLive
{
	TextBox::TextBox(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2D1_RECT_F parentRect) :	
		Control(deviceResources, parentRect),
		m_text(L""),
		m_fontFamily(FONT_FAMILY::SEGOE_UI),
		m_fontCollection(nullptr),
		m_fontWeight{ DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_WEIGHT_LIGHT },
		m_fontStyle{ DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STYLE_NORMAL },
		m_fontStretch{ DWRITE_FONT_STRETCH_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DWRITE_FONT_STRETCH_NORMAL },
		m_fontSize{ 20.0f, 20.0f, 20.0f },
		m_locale(L"en-US"),
		m_paragraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR),
		m_textAlignment(DWRITE_TEXT_ALIGNMENT_LEADING),
		m_maxWidth(240.0f),
		m_maxHeight(50.0f),
		m_textColor{ D2D1::ColorF::White, D2D1::ColorF::White, D2D1::ColorF::White }
	{
		ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));

		DX::ThrowIfFailed(
			m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(m_stateBlock.put())
		);
	}

	void TextBox::CreateDeviceDependentResources(bool includingControls)
	{
		if (ControlIsActive())
		{
			// Before you can run .put(), the value must be nullptr
			m_textColorBrush = nullptr;

			DX::ThrowIfFailed(
				m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(m_textColor[m_state], m_textColorBrush.put())
			);
		}
	}

	void TextBox::ReleaseDeviceDependentResources()
	{
		m_textColorBrush = nullptr;
	}

	void TextBox::Finalize()
	{
		// if either fill parent height or width, then just UpdateFillParent which will call the other update methods
		if (m_fillParentHeight || m_fillParentWidth)
			UpdateFillParent();
		else
		{
			UpdateTextFormat();
			UpdateTextLayout();
		}

		UpdateScreenTranslation();

		// Paragraph alignment
		DX::ThrowIfFailed(
			m_textFormat->SetParagraphAlignment(m_paragraphAlignment)
		);

		CreateDeviceDependentResources(false);

		m_finalized = true;
	}

	void TextBox::UpdateFillParent()
	{
		// if we are supposed to fill parent height
		if (m_fillParentHeight && m_fillParentWidth)
		{
			// If text height and width are both smaller than parent -> increase font size until larger than reduce by 1
			// THEN if text height OR width are larger than parent -> reduce font size until they both fit
			// Update format and layout so we can get the text height
			UpdateTextFormat();
			UpdateTextLayout();

			float totalHeight  = m_textMetrics.height + m_marginTop + m_marginBottom;
			float totalWidth   = m_textMetrics.width + m_marginLeft + m_marginRight;
			float parentHeight = m_parentRect.bottom - m_parentRect.top;
			float parentWidth  = m_parentRect.right - m_parentRect.left;

			while (totalHeight < parentHeight && totalWidth < parentWidth)
			{
				m_fontSize[m_state] += 1.0f;

				UpdateTextFormat();
				UpdateTextLayout();

				totalHeight = m_textMetrics.height + m_marginTop + m_marginBottom;
				totalWidth  = m_textMetrics.width + m_marginLeft + m_marginRight;
			}

			while ((totalHeight > parentHeight || totalWidth > parentWidth) && m_fontSize[m_state] > 2.0f)
			{
				m_fontSize[m_state] -= 1.0f;

				UpdateTextFormat();
				UpdateTextLayout();

				totalHeight = m_textMetrics.height + m_marginTop + m_marginBottom;
				totalWidth = m_textMetrics.width + m_marginLeft + m_marginRight;
			}
		}
		else if (m_fillParentHeight)
		{
			// If we are filling height, adjust the font size until the text fills the parent perfectly
			
			// Update format and layout so we can get the text height
			UpdateTextFormat();
			UpdateTextLayout();

			float totalHeight = m_textMetrics.height + m_marginTop + m_marginBottom;
			float parentHeight = m_parentRect.bottom - m_parentRect.top;

			while (totalHeight < parentHeight)
			{
				m_fontSize[m_state] += 1.0f;

				UpdateTextFormat();
				UpdateTextLayout();

				totalHeight = m_textMetrics.height + m_marginTop + m_marginBottom;
			}

			while (totalHeight > parentHeight && m_fontSize[m_state] > 2.0f)
			{
				m_fontSize[m_state] -= 1.0f;

				UpdateTextFormat();
				UpdateTextLayout();

				totalHeight = m_textMetrics.height + m_marginTop + m_marginBottom;
			}
		}
		else if (m_fillParentWidth)
		{
			// If we are filling width, adjust the font size until the text fills the parent width perfectly

			// Update format and layout so we can get the text height
			UpdateTextFormat();
			UpdateTextLayout();

			float totalWidth = m_textMetrics.width + m_marginLeft + m_marginRight;
			float parentWidth = m_parentRect.right - m_parentRect.left;

			while (totalWidth < parentWidth)
			{
				m_fontSize[m_state] += 1.0f;

				UpdateTextFormat();
				UpdateTextLayout();

				totalWidth = m_textMetrics.width + m_marginLeft + m_marginRight;
			}

			while (totalWidth > parentWidth && m_fontSize[m_state] > 2.0f)
			{
				m_fontSize[m_state] -= 1.0f;

				UpdateTextFormat();
				UpdateTextLayout();

				totalWidth = m_textMetrics.width + m_marginLeft + m_marginRight;
			}
		}
	}

	void TextBox::UpdateTextFormat()
	{
		if (ControlIsActive())
		{
			// Create the text format
			winrt::com_ptr<IDWriteTextFormat> textFormat;
			DX::ThrowIfFailed(
				m_deviceResources->GetDWriteFactory()->CreateTextFormat(
					FontFamilyHelper::ToWString(m_fontFamily).c_str(),
					m_fontCollection.get(),
					m_fontWeight[m_state],
					m_fontStyle[m_state],
					m_fontStretch[m_state],
					m_fontSize[m_state],
					m_locale.c_str(),
					textFormat.put()
				)
			);

			DX::As(textFormat, m_textFormat);

			DX::ThrowIfFailed(
				m_textFormat->SetTextAlignment(m_textAlignment)
			);
		}
	}

	void TextBox::UpdateTextLayout()
	{
		// Create the text layout
		winrt::com_ptr<IDWriteTextLayout> textLayout;
		DX::ThrowIfFailed(
			m_deviceResources->GetDWriteFactory()->CreateTextLayout(
				m_text.c_str(),
				(uint32_t)m_text.length(),
				m_textFormat.get(),
				m_maxWidth,
				m_maxHeight,
				textLayout.put()
			)
		);

		DX::As(textLayout, m_textLayout);

		DX::ThrowIfFailed(
			m_textLayout->GetMetrics(&m_textMetrics)
		);
	}

	void TextBox::UpdateScreenTranslation()
	{
		// The goal here is to compute the top left point for the text
		// Here is also a good spot to update m_controlRect

		switch (m_verticalAlignment)
		{
			// Stretch doesn't really make sense for TextBox so just default to Top behavior
			case ChemLive::VerticalAlignment::STRETCH:
			case ChemLive::VerticalAlignment::TOP:
				// from the top, just go up the amount of the top margin
				m_controlRect.top = m_parentRect.top + m_marginTop; 
				break;
			case ChemLive::VerticalAlignment::BOTTOM:
				// From the bottom, go up the bottom margin plus the text height
				m_controlRect.top = m_parentRect.bottom - (m_marginBottom + m_textMetrics.height);
				break;
			case ChemLive::VerticalAlignment::CENTER:
				// Compute the center of the parent rect 
				// then go up half the height of the text
				// then go down the amount of the top margin
				m_controlRect.top = m_parentRect.top + ((m_parentRect.bottom - m_parentRect.top) / 2) - (m_textMetrics.height / 2) + m_marginTop;
				break;
			default:
				throw std::invalid_argument("Unknown value for m_verticalAlignment enum");
		}

		switch (m_horizontalAlignment)
		{
			// Stretch doesn't really make sense for TextBox so just default to Top behavior
		    case ChemLive::HorizontalAlignment::STRETCH:
			case ChemLive::HorizontalAlignment::LEFT:
				// from the left, just go right the amount of the left margin
				m_controlRect.left = m_parentRect.left + m_marginLeft;
				break;
			case ChemLive::HorizontalAlignment::RIGHT:
				// From the right, go left the right margin plus the text width
				m_controlRect.left = m_parentRect.right - (m_marginRight + m_textMetrics.width);
				break;
			case ChemLive::HorizontalAlignment::CENTER:
				// Compute the center of the parent rect 
				// then go left half the width of the text
				// then go right the amount of the left margin
				m_controlRect.left = m_parentRect.left + ((m_parentRect.right - m_parentRect.left) / 2) - (m_textMetrics.width / 2) + m_marginLeft;
				break;
			default:
				throw std::invalid_argument("Unknown value for m_verticalAlignment enum");
		}

		m_screenTranslation = D2D1::Matrix3x2F::Translation(m_controlRect.left, m_controlRect.top);

		// Update the control rect so that it just encompasses the text and not the margin
		m_controlRect.top += m_marginTop;
		m_controlRect.bottom = m_textMetrics.height;

		m_controlRect.left += m_marginLeft;
		m_controlRect.right = m_controlRect.left + m_textMetrics.width;
	}

	void TextBox::Render()
	{
		if (ControlIsActive())
		{
			if (!m_finalized)
				throw std::invalid_argument("TextBox has not been finalized");

			ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext();
			auto logicalSize = m_deviceResources->GetFullLogicalSize();

			context->SaveDrawingState(m_stateBlock.get());
			context->BeginDraw();

			context->SetTransform(m_screenTranslation * m_deviceResources->GetOrientationTransform2D());

			context->DrawTextLayout(
				D2D1::Point2F(0.f, 0.f),
				m_textLayout.get(),
				m_textColorBrush.get()
			);

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

	void TextBox::UpdateOnPointerHover()
	{
		if (ControlIsActive())
		{
			m_state = ControlState::POINTER_HOVER;
			Finalize();
		}
	}

	void TextBox::UpdateOnPointerDown()
	{
		if (ControlIsActive())
		{
			m_state = ControlState::POINTER_DOWN;
			Finalize();
		}
	}

	bool TextBox::OnPointerPressed(CoreWindow w, PointerEventArgs const& e)
	{
		// The Control::PointerOverControl method does not set the POINTER_DOWN state, so it must be set here
		m_state = ControlState::POINTER_DOWN;
		Finalize();

		// Could trigger a custom event so that we can assign a custom handler to each control
		//
		// ...
		//

		// return false to indicate we do not want to capture pointer
		return false;
	}

	void TextBox::OnPointerMoved(CoreWindow w, PointerEventArgs const& e)
	{
		// The state should already have been updated in the Control::PointerOverControl method

		// Could trigger a custom event so that we can assign a custom handler to each control
		//
		// ...
		//
	}

	void TextBox::OnPointerReleased(CoreWindow w, PointerEventArgs const& e)
	{
		// The only reason this method should get called is if the pointer is actively over the control
		// therefore we can update the state to POINT_HOVER and trigger the Click event
		m_state = ControlState::POINTER_HOVER;
		Finalize();

		// Tigger TextBoxClick event

	}

	void TextBox::PointerNotOver()
	{
		if (m_state == ControlState::POINTER_HOVER || m_state == ControlState::POINTER_DOWN)
		{
			m_state = ControlState::POINTER_NOT_OVER;
			Finalize();
		}
	}

	float TextBox::TextHeight()
	{
		if (!m_finalized)
			Finalize();

		return m_textMetrics.height;
	}

	float TextBox::TextWidth()
	{
		if (!m_finalized)
			Finalize();

		return m_textMetrics.width;
	}
}