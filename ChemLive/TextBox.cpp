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
		m_fontWeight{ DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_WEIGHT_LIGHT },
		m_fontStyle{ DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STYLE_NORMAL },
		m_fontStretch{ DWRITE_FONT_STRETCH_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DWRITE_FONT_STRETCH_NORMAL },
		m_fontSize{ 20.0f, 20.0f, 20.0f, 20.0f },
		m_locale(L"en-US"),
		m_paragraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR),
		m_textAlignment(DWRITE_TEXT_ALIGNMENT_LEADING),
		m_textColor{ D2D1::ColorF::White, D2D1::ColorF::White, D2D1::ColorF::White, D2D1::ColorF::Gray }
		/*
		m_fillParentHeight(false),
		m_fillParentWidth(false)
		*/
	{
		ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));
		ZeroMemory(&m_trimming, sizeof(DWRITE_TRIMMING));
		m_trimming.granularity = DWRITE_TRIMMING_GRANULARITY::DWRITE_TRIMMING_GRANULARITY_NONE; // default is no trimming

		DX::ThrowIfFailed(
			m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(m_stateBlock.put())
		);

		CreateDeviceDependentResources();
		UpdateTextFormat();
		UpdateTextLayout();
	}

	void TextBox::CreateDeviceDependentResources()
	{
		// Before you can run .put(), the value must be nullptr
		m_textColorBrush = nullptr;

		DX::ThrowIfFailed(
			m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(m_textColor[m_state], m_textColorBrush.put())
		);
	}

	void TextBox::ReleaseDeviceDependentResources()
	{
		m_textColorBrush = nullptr;
	}

	void TextBox::ControlRectUpdated()
	{
		// This method is called whenever a change was made to the control rect
		// For a text box, the only change we need to make here is to update the 
		// screen translation of the text
		UpdateScreenTranslation();
	}

	void TextBox::UpdateState(ControlState state)
	{
		m_state = state;

		TextFormatChanged();
		CreateDeviceDependentResources();
	}

	void TextBox::TextLayoutChanged()
	{
		// Only the layout needs changing, so no need to call UpdateTextFormat
		/*
		if (m_fillParentHeight || m_fillParentWidth)
			UpdateFillParent();
		else
			UpdateTextLayout();
		*/

		UpdateTextLayout();

		// Update the control rect so it is aware of the new height/width of the text
		// We want the control rect to exactly encompass the text, so we need to update it with the 
		// new height and width values
		SetControlRectToTextSize();

		// No need to update screen translation here
		// Call sequence:
		// SetControlRectToTextSize() -> UpdateControlRect() -> ControlRectUpdated() -> UpdateScreenTranslation()
		//UpdateScreenTranslation();
	}

	void TextBox::TextFormatChanged()
	{
		// text format needs changing, so everything needs upadting
		/*
		if (m_fillParentHeight || m_fillParentWidth)
			UpdateFillParent();
		else
		{
			UpdateTextFormat();
			UpdateTextLayout();
		}		
		*/

		UpdateTextFormat();
		UpdateTextLayout();

		// Update the control rect so it is aware of the new height/width of the text
		// We want the control rect to exactly encompass the text, so we need to update it with the 
		// new height and width values
		SetControlRectToTextSize();

		// No need to update screen translation here
		// Call sequence:
		// SetControlRectToTextSize() -> UpdateControlRect() -> ControlRectUpdated() -> UpdateScreenTranslation()
		//UpdateScreenTranslation();
	}

	void TextBox::SetControlRectToTextSize()
	{
		float parentWidth = max(0, m_parentRect.right - m_parentRect.left);
		float parentHeight = max(0, m_parentRect.bottom - m_parentRect.top);

		float height = max(0, min(m_textMetrics.height, parentHeight));
		float width = max(0, min(m_textMetrics.width, parentWidth));
		UpdateControlRect(height, width);
	}

	// UpdateFillParent
	/*
	void TextBox::UpdateFillParent()
	{
		if (m_fillParentHeight && m_fillParentWidth)
		{
			// If text height and width are both smaller than parent -> increase font size until larger then reduce by 1
			// THEN if text height OR width are larger than parent -> reduce font size until they both fit
			// Update format and layout so we can get the text height
			UpdateTextFormat();
			UpdateTextLayout();

			float totalHeight  = m_textMetrics.height + m_margin.top + m_margin.bottom;
			float totalWidth   = m_textMetrics.width + m_margin.left + m_margin.right;
			float parentHeight = m_parentRect.bottom - m_parentRect.top;
			float parentWidth  = m_parentRect.right - m_parentRect.left;

			while (totalHeight < parentHeight && totalWidth < parentWidth)
			{
				m_fontSize[m_state] += 1.0f;

				UpdateTextFormat();
				UpdateTextLayout();

				totalHeight = m_textMetrics.height + m_margin.top + m_margin.bottom;
				totalWidth  = m_textMetrics.width + m_margin.left + m_margin.right;
			}

			while ((totalHeight > parentHeight || totalWidth > parentWidth) && m_fontSize[m_state] > 2.0f)
			{
				m_fontSize[m_state] -= 1.0f;

				UpdateTextFormat();
				UpdateTextLayout();

				totalHeight = m_textMetrics.height + m_margin.top + m_margin.bottom;
				totalWidth = m_textMetrics.width + m_margin.left + m_margin.right;
			}
		}
		else if (m_fillParentHeight)
		{
			// If we are filling height, adjust the font size until the text fills the parent perfectly
			
			// Update format and layout so we can get the text height
			UpdateTextFormat();
			UpdateTextLayout();

			float totalHeight = m_textMetrics.height + m_margin.top + m_margin.bottom;
			float parentHeight = m_parentRect.bottom - m_parentRect.top;

			while (totalHeight < parentHeight)
			{
				m_fontSize[m_state] += 1.0f;

				UpdateTextFormat();
				UpdateTextLayout();

				totalHeight = m_textMetrics.height + m_margin.top + m_margin.bottom;
			}

			while (totalHeight > parentHeight && m_fontSize[m_state] > 2.0f)
			{
				m_fontSize[m_state] -= 1.0f;

				UpdateTextFormat();
				UpdateTextLayout();

				totalHeight = m_textMetrics.height + m_margin.top + m_margin.bottom;
			}
		}
		else if (m_fillParentWidth)
		{
			// If we are filling width, adjust the font size until the text fills the parent width perfectly

			// Update format and layout so we can get the text height
			UpdateTextFormat();
			UpdateTextLayout();

			float totalWidth = m_textMetrics.width + m_margin.left + m_margin.right;
			float parentWidth = m_parentRect.right - m_parentRect.left;

			while (totalWidth < parentWidth)
			{
				m_fontSize[m_state] += 1.0f;

				UpdateTextFormat();
				UpdateTextLayout();

				totalWidth = m_textMetrics.width + m_margin.left + m_margin.right;
			}

			while (totalWidth > parentWidth && m_fontSize[m_state] > 2.0f)
			{
				m_fontSize[m_state] -= 1.0f;

				UpdateTextFormat();
				UpdateTextLayout();

				totalWidth = m_textMetrics.width + m_margin.left + m_margin.right;
			}
		}

		SetControlRectToTextSize();
	}
	*/

	void TextBox::UpdateTextFormat()
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

		DX::ThrowIfFailed(
			m_textFormat->SetParagraphAlignment(m_paragraphAlignment)
		);

		/*
		DX::ThrowIfFailed(
			m_textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP)
		);
		*/

		DX::ThrowIfFailed(
			m_textFormat->SetTrimming(&m_trimming, nullptr)
		);
	}

	void TextBox::UpdateTextLayout()
	{
		// Even though there is no trimming by default, we will naturally set the max width/height
		// to be that of the parent rect
		float maxWidth  = max(0, m_parentRect.right - m_parentRect.left);
		float maxHeight = max(0, m_parentRect.bottom - m_parentRect.top);

		// Create the text layout
		winrt::com_ptr<IDWriteTextLayout> textLayout;
		DX::ThrowIfFailed(
			m_deviceResources->GetDWriteFactory()->CreateTextLayout(
				m_text.c_str(),
				(uint32_t)m_text.length(),
				m_textFormat.get(),
				maxWidth,
				maxHeight,
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
		// The control rect is updated every time the parent rect changes, whenever any of the margins change,
		// or any time the text size changes. Therefore, we can simply use the top left of the control rect 
		// to locate the text.
		m_screenTranslation = D2D1::Matrix3x2F::Translation(m_controlRect.left, m_controlRect.top);
	}

	void TextBox::Render()
	{
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

	bool TextBox::OnPointerPressed(CoreWindow w, PointerEventArgs const& e)
	{
		// Pointer interaction should only affect the control if it is active
		if (ControlIsActive())
		{
			UpdateState(ControlState::POINTER_DOWN);

			// Could trigger a custom event
			//
			// ...
			//
		}

		// return false to indicate we do not want to capture pointer
		return false;
	}

	void TextBox::OnPointerMoved(CoreWindow w, PointerEventArgs const& e)
	{
		// The state should already have been updated in the Control::PointerOverControl method

		// Could trigger a custom event
		//
		// ...
		//
	}

	void TextBox::OnPointerReleased(CoreWindow w, PointerEventArgs const& e)
	{
		// Pointer interaction should only affect the control if it is active
		if (ControlIsActive())
		{
			UpdateState(ControlState::POINTER_HOVER);

			// Could trigger a click-like event
			//
			// ...
			//
		}
	}

	void TextBox::PointerNotOver()
	{
		if (ControlIsActive())
		{
			if (m_state == ControlState::POINTER_HOVER || m_state == ControlState::POINTER_DOWN)
			{
				UpdateState(ControlState::POINTER_NOT_OVER);

				// Could trigger a custom event
				//
				// ...
				//
			}
		}
	}

	// Set Methods ===============================================================
	void TextBox::Text(std::wstring text)
	{
		m_text = text;
		TextLayoutChanged();
	}
	void TextBox::FontFamily(FONT_FAMILY fontFamily)
	{
		m_fontFamily = fontFamily;
		TextFormatChanged();
	}
	void TextBox::Locale(std::wstring locale)
	{
		m_locale = locale;
		TextFormatChanged();
	}
	void TextBox::ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT alignment)
	{
		m_paragraphAlignment = alignment;
		TextFormatChanged();
	}
	void TextBox::TextAlignment(DWRITE_TEXT_ALIGNMENT alignment)
	{
		m_textAlignment = alignment;
		TextFormatChanged();
	}
	void TextBox::TrimmingGranularity(DWRITE_TRIMMING_GRANULARITY granularity)
	{
		m_trimming.granularity = granularity;
		TextFormatChanged();
	}

	void TextBox::FontWeight(DWRITE_FONT_WEIGHT fontWeight)
	{
		m_fontWeight[0] = fontWeight;
		TextFormatChanged();
	}
	void TextBox::FontWeightOnPointerHover(DWRITE_FONT_WEIGHT fontWeight)
	{
		m_fontWeight[1] = fontWeight;
		TextFormatChanged();
	}
	void TextBox::FontWeightOnPointerDown(DWRITE_FONT_WEIGHT fontWeight)
	{
		m_fontWeight[2] = fontWeight;
		TextFormatChanged();
	}

	void TextBox::FontStyle(DWRITE_FONT_STYLE fontStyle)
	{
		m_fontStyle[0] = fontStyle;
		TextFormatChanged();
	}
	void TextBox::FontStyleOnPointerHover(DWRITE_FONT_STYLE fontStyle)
	{
		m_fontStyle[1] = fontStyle;
		TextFormatChanged();
	}
	void TextBox::FontStyleOnPointerDown(DWRITE_FONT_STYLE fontStyle)
	{
		m_fontStyle[2] = fontStyle;
		TextFormatChanged();
	}

	void TextBox::FontStretch(DWRITE_FONT_STRETCH fontStretch)
	{
		m_fontStretch[0] = fontStretch;
		TextFormatChanged();
	}
	void TextBox::FontStretchOnPointerHover(DWRITE_FONT_STRETCH fontStretch)
	{
		m_fontStretch[1] = fontStretch;
		TextFormatChanged();
	}
	void TextBox::FontStretchOnPointerDown(DWRITE_FONT_STRETCH fontStretch)
	{
		m_fontStretch[2] = fontStretch;
		TextFormatChanged();
	}

	void TextBox::FontSize(float fontSize)
	{
		m_fontSize[0] = fontSize;
		TextFormatChanged();
	}
	void TextBox::FontSizeOnPointerHover(float fontSize)
	{
		m_fontSize[1] = fontSize;
		TextFormatChanged();
	}
	void TextBox::FontSizeOnPointerDown(float fontSize)
	{
		m_fontSize[2] = fontSize;
		TextFormatChanged();
	}

	void TextBox::TextColor(D2D1::ColorF color)
	{
		m_textColor[0] = color;
		CreateDeviceDependentResources();
	}
	void TextBox::TextColorOnPointerHover(D2D1::ColorF color)
	{
		m_textColor[1] = color;
		CreateDeviceDependentResources();
	}
	void TextBox::TextColorOnPointerDown(D2D1::ColorF color)
	{
		m_textColor[2] = color;
		CreateDeviceDependentResources();
	}

	/*
	void TextBox::FillParentHeight(bool fill) 
	{ 
		m_fillParentHeight = fill;
		TextFormatChanged();
	}
	void TextBox::FillParentWidth(bool fill) 
	{ 
		m_fillParentWidth = fill; 
		TextFormatChanged();
	}
	void TextBox::FillParent(bool fill) 
	{ 
		m_fillParentWidth = m_fillParentHeight = fill; 
		TextFormatChanged();
	}
	*/
}