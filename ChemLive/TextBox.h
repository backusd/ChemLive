#pragma once

#include "Control.h"
#include "FontFamilyHelper.h"

namespace ChemLive
{
	class TextBox : public Control
	{
	private:
		// Text parameters
		FONT_FAMILY								m_fontFamily;
		winrt::com_ptr<IDWriteFontCollection>	m_fontCollection;	// not sure what this is - fine to set to nullptr
		DWRITE_FONT_WEIGHT						m_fontWeight[4];
		DWRITE_FONT_STYLE						m_fontStyle[4];
		DWRITE_FONT_STRETCH						m_fontStretch[4];
		float									m_fontSize[4];
		std::wstring							m_locale;	// not sure what else this can be other than "en-US"
		DWRITE_PARAGRAPH_ALIGNMENT				m_paragraphAlignment;
		DWRITE_TEXT_ALIGNMENT					m_textAlignment;
		DWRITE_TRIMMING							m_trimming;
		D2D1::ColorF							m_textColor[4];

		// Resources related to text rendering.
		std::wstring                                m_text;
		DWRITE_TEXT_METRICS	                        m_textMetrics;
		winrt::com_ptr<ID2D1SolidColorBrush>		m_textColorBrush;
		winrt::com_ptr<ID2D1DrawingStateBlock1>		m_stateBlock;
		winrt::com_ptr<IDWriteTextLayout3>			m_textLayout;
		winrt::com_ptr<IDWriteTextFormat2>			m_textFormat;
		D2D1::Matrix3x2F							m_screenTranslation;

		/*
		// Whether or not to fill parent height/width
		bool m_fillParentHeight;
		bool m_fillParentWidth;
		*/

		// Functions to update format/layout/etc
		void TextLayoutChanged();
		void TextFormatChanged();
		void UpdateState(ControlState state);
		void SetControlRectToTextSize();

		// void UpdateFillParent();
		void UpdateTextFormat();
		void UpdateTextLayout();
		void UpdateScreenTranslation();

	public:
		TextBox(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2D1_RECT_F parentRect);

		// Virtual functions from Control class
		void Render();
		void CreateDeviceDependentResources();
		void ReleaseDeviceDependentResources();

		bool OnPointerPressed(CoreWindow w, PointerEventArgs const& e);
		void OnPointerMoved(CoreWindow w, PointerEventArgs const& e);
		void OnPointerReleased(CoreWindow w, PointerEventArgs const& e);
		void PointerNotOver();

		void ControlRectUpdated();

		// Set Methods
		void Text(std::wstring text);
		void FontFamily(FONT_FAMILY fontFamily);
		void Locale(std::wstring locale);
		void ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT alignment);
		void TextAlignment(DWRITE_TEXT_ALIGNMENT alignment);
		void TrimmingGranularity(DWRITE_TRIMMING_GRANULARITY granularity);

		void FontWeight(DWRITE_FONT_WEIGHT fontWeight);
		void FontWeightOnPointerHover(DWRITE_FONT_WEIGHT fontWeight);
		void FontWeightOnPointerDown(DWRITE_FONT_WEIGHT fontWeight);

		void FontStyle(DWRITE_FONT_STYLE fontStyle);
		void FontStyleOnPointerHover(DWRITE_FONT_STYLE fontStyle);
		void FontStyleOnPointerDown(DWRITE_FONT_STYLE fontStyle);

		void FontStretch(DWRITE_FONT_STRETCH fontStretch);
		void FontStretchOnPointerHover(DWRITE_FONT_STRETCH fontStretch);
		void FontStretchOnPointerDown(DWRITE_FONT_STRETCH fontStretch);

		void FontSize(float fontSize);
		void FontSizeOnPointerHover(float fontSize);
		void FontSizeOnPointerDown(float fontSize);

		void TextColor(D2D1::ColorF color);
		void TextColorOnPointerHover(D2D1::ColorF color);
		void TextColorOnPointerDown(D2D1::ColorF color);

		/*
		void FillParentHeight(bool fill);
		void FillParentWidth(bool fill);
		void FillParent(bool fill);
		*/

		// Non-trivial Get Methods
		float TextHeight() { return m_textMetrics.height; }
		float TextWidth() { return m_textMetrics.width; }

		// Get Methods
		std::wstring Text() { return m_text; }
		FONT_FAMILY FontFamily() { return m_fontFamily; }
		std::wstring Locale() { return m_locale; }
		DWRITE_PARAGRAPH_ALIGNMENT ParagraphAlignment() { return m_paragraphAlignment; }
		DWRITE_TEXT_ALIGNMENT TextAlignment() { return m_textAlignment; }
		DWRITE_TRIMMING_GRANULARITY TrimmingGranularity() { return m_trimming.granularity; }

		DWRITE_FONT_WEIGHT FontWeight() { return m_fontWeight[0]; }
		DWRITE_FONT_WEIGHT FontWeightOnPointerHover() { return m_fontWeight[1]; }
		DWRITE_FONT_WEIGHT FontWeightOnPointerDown() { return m_fontWeight[2]; }

		DWRITE_FONT_STYLE FontStyle() { return m_fontStyle[0]; }
		DWRITE_FONT_STYLE FontStyleOnPointerHover() { return m_fontStyle[1]; }
		DWRITE_FONT_STYLE FontStyleOnPointerDown() { return m_fontStyle[2]; }

		DWRITE_FONT_STRETCH FontStretch() { return m_fontStretch[0]; }
		DWRITE_FONT_STRETCH FontStretchOnPointerHover() { return m_fontStretch[1]; }
		DWRITE_FONT_STRETCH FontStretchOnPointerDown() { return m_fontStretch[2]; }

		float FontSize() { return m_fontSize[0]; }
		float FontSizeOnPointerHover() { return m_fontSize[1]; }
		float FontSizeOnPointerDown() { return m_fontSize[2]; }

		D2D1::ColorF TextColor() { return m_textColor[0]; }
		D2D1::ColorF TextColorOnPointerHover() { return m_textColor[1]; }
		D2D1::ColorF TextColorOnPointerDown() { return m_textColor[2]; }
		
	};
}
