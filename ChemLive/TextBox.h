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
		DWRITE_FONT_WEIGHT						m_fontWeight[3];
		DWRITE_FONT_STYLE						m_fontStyle[3];
		DWRITE_FONT_STRETCH						m_fontStretch[3];
		float									m_fontSize[3];
		std::wstring							m_locale;	// not sure what else this can be other than "en-US"
		DWRITE_PARAGRAPH_ALIGNMENT				m_paragraphAlignment;
		DWRITE_TEXT_ALIGNMENT					m_textAlignment;
		float									m_maxWidth;
		float									m_maxHeight;
		D2D1::ColorF							m_textColor[3];
		
		// Resources related to text rendering.
		std::wstring                                m_text;
		DWRITE_TEXT_METRICS	                        m_textMetrics;
		winrt::com_ptr<ID2D1SolidColorBrush>		m_textColorBrush;
		winrt::com_ptr<ID2D1DrawingStateBlock1>		m_stateBlock;
		winrt::com_ptr<IDWriteTextLayout3>			m_textLayout;
		winrt::com_ptr<IDWriteTextFormat2>			m_textFormat;
		D2D1::Matrix3x2F							m_screenTranslation;

		// Functions to update format/layout/etc
		void UpdateFillParent();
		void UpdateTextFormat();
		void UpdateTextLayout();
		void UpdateScreenTranslation();

	public:
		TextBox(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2D1_RECT_F parentRect);

		// Virtual functions from Control class
		void Render();
		void CreateDeviceDependentResources(bool includingControls);
		void ReleaseDeviceDependentResources();
		void UpdateOnPointerHover();
		void UpdateOnPointerDown();

		bool OnPointerPressed(CoreWindow w, PointerEventArgs const& e);
		void OnPointerMoved(CoreWindow w, PointerEventArgs const& e);
		void OnPointerReleased(CoreWindow w, PointerEventArgs const& e);
		void PointerNotOver();

		// All Controls must be finalized before they can can rendered
		void Finalize();

		// Non-trivial Get Methods
		float TextHeight();
		float TextWidth();

		// Set Methods
		void Text(std::wstring text) { m_finalized = false; m_text = text; }
		void FontFamily(FONT_FAMILY fontFamily) { m_finalized = false; m_fontFamily = fontFamily; }
		void Locale(std::wstring locale) { m_finalized = false; m_locale = locale; }
		void ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT alignment) { m_finalized = false; m_paragraphAlignment = alignment; }
		void TextAlignment(DWRITE_TEXT_ALIGNMENT alignment) { m_finalized = false; m_textAlignment = alignment; }
		void MaxWidth(float maxWidth) { m_finalized = false; m_maxWidth = maxWidth; }
		void MaxHeight(float maxHeight) { m_finalized = false; m_maxHeight = maxHeight; }

		void FontWeight(DWRITE_FONT_WEIGHT fontWeight) { m_finalized = false; m_fontWeight[0] = fontWeight; }
		void FontWeightOnPointerHover(DWRITE_FONT_WEIGHT fontWeight) { m_finalized = false; m_fontWeight[1] = fontWeight; }
		void FontWeightOnPointerDown(DWRITE_FONT_WEIGHT fontWeight) { m_finalized = false; m_fontWeight[2] = fontWeight; }

		void FontStyle(DWRITE_FONT_STYLE fontStyle) { m_finalized = false; m_fontStyle[0] = fontStyle; }
		void FontStyleOnPointerHover(DWRITE_FONT_STYLE fontStyle) { m_finalized = false; m_fontStyle[1] = fontStyle; }
		void FontStyleOnPointerDown(DWRITE_FONT_STYLE fontStyle) { m_finalized = false; m_fontStyle[2] = fontStyle; }

		void FontStretch(DWRITE_FONT_STRETCH fontStretch) { m_finalized = false; m_fontStretch[0] = fontStretch; }
		void FontStretchOnPointerHover(DWRITE_FONT_STRETCH fontStretch) { m_finalized = false; m_fontStretch[1] = fontStretch; }
		void FontStretchOnPointerDown(DWRITE_FONT_STRETCH fontStretch) { m_finalized = false; m_fontStretch[2] = fontStretch; }

		void FontSize(float fontSize) { m_finalized = false; m_fontSize[0] = fontSize; }
		void FontSizeOnPointerHover(float fontSize) { m_finalized = false; m_fontSize[1] = fontSize; }
		void FontSizeOnPointerDown(float fontSize) { m_finalized = false; m_fontSize[2] = fontSize; }

		void TextColor(D2D1::ColorF color) { m_finalized = false; m_textColor[0] = color; }
		void TextColorOnPointerHover(D2D1::ColorF color) { m_finalized = false; m_textColor[1] = color; }
		void TextColorOnPointerDown(D2D1::ColorF color) { m_finalized = false; m_textColor[2] = color; }
		
		// Get Methods
		std::wstring Text() { return m_text; }
		FONT_FAMILY FontFamily() { return m_fontFamily; }
		std::wstring Locale() { return m_locale; }
		DWRITE_PARAGRAPH_ALIGNMENT ParagraphAlignment() { return m_paragraphAlignment; }
		DWRITE_TEXT_ALIGNMENT TextAlignment() { return m_textAlignment; }
		float MaxWidth() { return m_maxWidth; }
		float MaxHeight() { return m_maxHeight; }

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
