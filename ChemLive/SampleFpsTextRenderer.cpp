#include "pch.h"
#include "SampleFpsTextRenderer.h"
#include "DirectXHelper.h"



namespace DirectX
{
	// Initializes D2D resources used for text rendering.
	SampleFpsTextRenderer::SampleFpsTextRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
		m_text(L""),
		m_deviceResources(deviceResources)
	{
		ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));

		// Create device independent resources
		winrt::com_ptr<IDWriteTextFormat> textFormat;
		DX::ThrowIfFailed(
			m_deviceResources->GetDWriteFactory()->CreateTextFormat(
				L"Segoe UI",
				nullptr,
				DWRITE_FONT_WEIGHT_LIGHT,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				32.0f,
				L"en-US",
				textFormat.put()
			)
		);

		DX::As(textFormat, m_textFormat);

		DX::ThrowIfFailed(
			m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
		);

		DX::ThrowIfFailed(
			m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(m_stateBlock.put())
		);

		CreateDeviceDependentResources();
	}

	// Updates the text to be displayed.
	void SampleFpsTextRenderer::Update(DX::StepTimer const& timer)
	{
		// Update display text.
		uint32_t fps = timer.GetFramesPerSecond();

		m_text = (fps > 0) ? std::to_wstring(fps) + L" FPS" : L" - FPS";

		winrt::com_ptr<IDWriteTextLayout> textLayout;
		DX::ThrowIfFailed(
			m_deviceResources->GetDWriteFactory()->CreateTextLayout(
				m_text.c_str(),
				(uint32_t)m_text.length(),
				m_textFormat.get(),
				240.0f, // Max width of the input text.
				50.0f, // Max height of the input text.
				textLayout.put()
			)
		);

		DX::As(textLayout, m_textLayout);

		DX::ThrowIfFailed(
			m_textLayout->GetMetrics(&m_textMetrics)
		);
	}

	// Renders a frame to the screen.
	void SampleFpsTextRenderer::Render()
	{
		ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext();
		auto logicalSize = m_deviceResources->GetFullLogicalSize();

		context->SaveDrawingState(m_stateBlock.get());
		context->BeginDraw();

		// Position on the bottom right corner
		D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(
			logicalSize.Width - m_textMetrics.layoutWidth,
			logicalSize.Height - m_textMetrics.height
		);

		context->SetTransform(screenTranslation * m_deviceResources->GetOrientationTransform2D());

		DX::ThrowIfFailed(
			m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING)
		);

		context->DrawTextLayout(
			D2D1::Point2F(0.f, 0.f),
			m_textLayout.get(),
			m_whiteBrush.get()
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

	void SampleFpsTextRenderer::CreateDeviceDependentResources()
	{
		DX::ThrowIfFailed(
			m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), m_whiteBrush.put())
		);
	}
	void SampleFpsTextRenderer::ReleaseDeviceDependentResources()
	{
		m_whiteBrush = nullptr;
	}
}