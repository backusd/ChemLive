#pragma once

#include "Control.h"
#include "TextBox.h"

using winrt::Windows::Foundation::EventHandler;

namespace ChemLive
{
	enum class SliderTextLocation
	{
		COLLAPSED,
		TOP,
		RIGHT,
		BOTTOM,
		LEFT
	};

	class Slider : public Control
	{
	private:
		float m_value;
		float m_minValue;
		float m_maxValue;


		// Slider Rect
		D2D1_RECT_F  m_sliderRect;       // The actual location in pixels of the slider rect
		float		 m_sliderRectHeight; // In pixels
		float		 m_sliderRectWidth;  // In pixels
		D2D1_POINT_2F m_sliderBarLeft;
		D2D1_POINT_2F m_sliderBarRight;
		D2D1::ColorF m_sliderRectColor[3];
		D2D1::ColorF m_sliderRectBorderColor[3];
		float        m_sliderRectBorderThickness[3];

		// Slider Bar
		D2D1::ColorF m_sliderBarColor;
		float		 m_sliderBarThickness;

		// Resources for rendering
		winrt::com_ptr<ID2D1SolidColorBrush>    m_sliderRectBrush;
		winrt::com_ptr<ID2D1SolidColorBrush>    m_sliderBorderBrush;
		winrt::com_ptr<ID2D1SolidColorBrush>    m_sliderBarBrush;
		winrt::com_ptr<ID2D1DrawingStateBlock1>	m_stateBlock;

		// TextBox
		winrt::com_ptr<TextBox> m_textBox;
		SliderTextLocation   	m_textBoxLocation;
		float					m_textBoxMaxWidth; // The width (in pixels) of the space given to the TextBox

	public:
		Slider(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2D1_RECT_F parentRect);

		// Virtual functions from Control
		void Render();
		void CreateDeviceDependentResources(bool includingControls);
		void ReleaseDeviceDependentResources();
		void Finalize();
		void UpdateOnPointerHover();
		void UpdateOnPointerDown();
		void PointerNotOver();
		bool OnPointerPressed(CoreWindow w, PointerEventArgs const& e);
		void OnPointerMoved(CoreWindow w, PointerEventArgs const& e);
		void OnPointerReleased(CoreWindow w, PointerEventArgs const& e);
		bool PointerOverControl(Point p); // Must override because we only care about pointer over the slider rect

		// Set Height/Width of the entire space for the slider to reside in
		void Height(float height) { m_finalized = false; m_controlRect.bottom = m_controlRect.top + height; }
		void Width(float width) { m_finalized = false; m_controlRect.right = m_controlRect.left + width; }

		// Set Methods
		void MinValue(float value) { m_finalized = false; m_minValue = value; }
		void MaxValue(float value) { m_finalized = false; m_maxValue = value; }
		void Value(float value) { m_finalized = false; m_value = value; }		

		void SliderRectWidth(float width) { m_finalized = false; m_sliderRectWidth = width; }
		void SliderRectHeight(float height) { m_finalized = false; m_sliderRectHeight = height; }

		void SliderRectColor(D2D1::ColorF color) { m_finalized = false; m_sliderRectColor[0] = color; }
		void SliderRectColorOnPointerHover(D2D1::ColorF color) { m_finalized = false; m_sliderRectColor[1] = color; }
		void SliderRectColorOnPointerDown(D2D1::ColorF color) { m_finalized = false; m_sliderRectColor[2] = color; }

		void SliderRectBorderColor(D2D1::ColorF color) { m_finalized = false; m_sliderRectBorderColor[0] = color; }
		void SliderRectBorderColorOnPointerHover(D2D1::ColorF color) { m_finalized = false; m_sliderRectBorderColor[1] = color; }
		void SliderRectBorderColorOnPointerDown(D2D1::ColorF color) { m_finalized = false; m_sliderRectBorderColor[2] = color; }

		void SliderRectBorderThickness(float thickness) { m_finalized = false; m_sliderRectBorderThickness[0] = thickness; }
		void SliderRectBorderThicknessOnPointerHover(float thickness) { m_finalized = false; m_sliderRectBorderThickness[1] = thickness; }
		void SliderRectBorderThicknessOnPointerDown(float thickness) { m_finalized = false; m_sliderRectBorderThickness[2] = thickness; }

		void SliderBarColor(D2D1::ColorF color) { m_finalized = false; m_sliderBarColor = color; }
		void SliderBarThickness(float thickness) { m_finalized = false; m_sliderBarThickness = thickness; }

		void TextLocation(SliderTextLocation location) { m_finalized = false; m_textBoxLocation = location; }
		void TextFontWeight(DWRITE_FONT_WEIGHT fontWeight);
		void TextFontStyle(DWRITE_FONT_STYLE fontStyle);
		void TextFontSize(float fontSize);
		void TextColor(D2D1::ColorF color);
		void TextBoxMaxWidth(float width) { m_finalized = false; m_textBoxMaxWidth = width; }


		// Get Methods
		float MinValue() { return m_minValue; }
		float MaxValue() { return m_maxValue; }
		float Value() { return m_value; }

		D2D1_RECT_F SliderRect() { return m_sliderRect; }
		float SliderRectWidth() { return m_sliderRectWidth; }
		float SliderRectHeight() { return m_sliderRectHeight; }

		D2D1::ColorF SliderRectColor() { return m_sliderRectColor[0]; }
		D2D1::ColorF SliderRectColorOnPointerHover() { return m_sliderRectColor[1]; }
		D2D1::ColorF SliderRectColorOnPointerDown() { return m_sliderRectColor[2]; }

		D2D1::ColorF SliderRectBorderColor() { return m_sliderRectBorderColor[0]; }
		D2D1::ColorF SliderRectBorderColorOnPointerHover() { return m_sliderRectBorderColor[1]; }
		D2D1::ColorF SliderRectBorderColorOnPointerDown() { return m_sliderRectBorderColor[2]; }

		float SliderRectBorderThickness() { return m_sliderRectBorderThickness[0]; }
		float SliderRectBorderThicknessOnPointerHover() { return m_sliderRectBorderThickness[1]; }
		float SliderRectBorderThicknessOnPointerDown() { return m_sliderRectBorderThickness[2]; }

		D2D1::ColorF SliderBarColor() { return m_sliderBarColor; }
		float SliderBarThickness() { return m_sliderBarThickness; }

		SliderTextLocation TextLocation() { return m_textBoxLocation; }
		DWRITE_FONT_WEIGHT TextFontWeight() { return m_textBox->FontWeight(); }
		DWRITE_FONT_STYLE TextFontStyle() { return m_textBox->FontStyle(); }
		float TextFontSize() { return m_textBox->FontSize(); }
		D2D1::ColorF TextColor() { return m_textBox->TextColor(); }
		float TextBoxMaxWidth() { return m_textBoxMaxWidth; }


		// Slider Events
		winrt::event_token SliderMoved(EventHandler<float> const& handler);
		void SliderMoved(winrt::event_token const& token) noexcept;
		winrt::event<EventHandler<float>> m_sliderMovedEvent;

		void FireSliderMoved();
	};
}