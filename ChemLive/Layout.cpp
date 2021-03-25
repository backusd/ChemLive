#include "pch.h"
#include "Layout.h"


namespace DisplayMetrics
{
	// High resolution displays can require a lot of GPU and battery power to render.
	// High resolution phones, for example, may suffer from poor battery life if
	// games attempt to render at 60 frames per second at full fidelity.
	// The decision to render at full fidelity across all platforms and form factors
	// should be deliberate.
	static const bool SupportHighResolutions = false;

	// The default thresholds that define a "high resolution" display. If the thresholds
	// are exceeded and SupportHighResolutions is false, the dimensions will be scaled
	// by 50%.
	static const float DpiThreshold = 192.0f;		// 200% of standard desktop display.
	static const float WidthThreshold = 1920.0f;	// 1080p width.
	static const float HeightThreshold = 1080.0f;	// 1080p height.
};

namespace ChemLive
{
	Layout::Layout(CoreWindow const& window) :
		m_renderPane(nullptr),
		m_menuPane(nullptr),
		m_menuBarPane(nullptr),
		m_window(window)
	{
		this->UpdateLayout(window);
	}

	void Layout::UpdateLayout(CoreWindow const& window)
	{
		// Get the Dots-per-inch (DPI) of the screen in use
		m_dpi = DisplayInformation::GetForCurrentView().LogicalDpi();

		// Adjust DPI based on resolution of device
		AdjustDPI();

		// Populate the Pane pointers with the DIPS values from the window
		UpdatePanes(window.Bounds().Width, window.Bounds().Height);
	}

	void Layout::UpdateLayout(DXGI_MODE_ROTATION displayRotation)
	{
		if (displayRotation == DXGI_MODE_ROTATION_ROTATE90 || displayRotation == DXGI_MODE_ROTATION_ROTATE270)
		{
			// The device is rotated, so we need to completely re-compute the pane sizes
			// Call UpdatePanes, but swap the height and width parameters
			UpdatePanes(m_window.get().Bounds().Height, m_window.get().Bounds().Width);
		}
	}

	void Layout::UpdatePanes(float windowWidthDIPS, float windowHeightDIPS)
	{
		if (m_renderPane == nullptr)  m_renderPane  = std::unique_ptr<Pane>(new Pane());
		if (m_menuPane == nullptr)    m_menuPane    = std::unique_ptr<Pane>(new Pane());
		if (m_menuBarPane == nullptr) m_menuBarPane = std::unique_ptr<Pane>(new Pane());

		// prevent 0 width/height sizing
		windowWidthDIPS = max(windowWidthDIPS, 1);
		windowHeightDIPS = max(windowHeightDIPS, 1);

		// Store the DIPS values in the Panes
		// MenuBarPane
		//		Top    = 0
		//      Left   = 0
		//      Height = 20
		//      Width  = Window Width
		// MenuPane
		//		Top    = 20
		//		Left   = Window Width - Menu Pane Width
		//		Height = Window Height - 20
		//		Width  = IF Window Width < 200 -> 0 (Collapsed) ELSE Greater of: 25% Window Width / 100 pixels
		// RenderPane
		//		Top    = 20
		//		Left   = 0
		//		Height = Window Height -20
		//		Width  = Window Width - Menu Pane Width

		float menuBarPaneHeight = 20.0f;
		float menuPaneWidth = (windowWidthDIPS < 200.0f) ? 0.0f : max(0.25f * windowWidthDIPS, 100.0f);

		m_menuBarPane->Top(0.0f);
		m_menuBarPane->Left(0.0f);
		m_menuBarPane->Height(menuBarPaneHeight);
		m_menuBarPane->Width(windowWidthDIPS);

		m_menuPane->Top(menuBarPaneHeight);
		m_menuPane->Left(windowWidthDIPS - menuPaneWidth);
		m_menuPane->Height(windowHeightDIPS - menuBarPaneHeight);
		m_menuPane->Width(menuPaneWidth);

		m_renderPane->Top(menuBarPaneHeight);
		m_renderPane->Left(0.0f);
		m_renderPane->Height(windowHeightDIPS - menuBarPaneHeight);
		m_renderPane->Width(windowWidthDIPS - menuPaneWidth);
	}

	void Layout::AdjustDPI()
	{
		// To improve battery life on high resolution devices, render to a smaller render target
		// and allow the GPU to scale the output when it is presented.
		if (!DisplayMetrics::SupportHighResolutions && m_dpi > DisplayMetrics::DpiThreshold)
		{
			float width = FullWindowWidthPixels();
			float height = FullWindowHeightPixels();

			// When the device is in portrait orientation, height > width. Compare the
			// larger dimension against the width threshold and the smaller dimension
			// against the height threshold.
			if (max(width, height) > DisplayMetrics::WidthThreshold && min(width, height) > DisplayMetrics::HeightThreshold)
			{
				// To scale the app we change the effective DPI. Logical size does not change.
				m_dpi /= 2.0f;
			}
		}
	}

	bool Layout::DPI(float dpi)
	{
		if (dpi != m_dpi)
		{
			// When the display DPI changes, the logical size of the window (measured in Dips) also changes and needs to be updated.
			UpdatePanes(m_window.get().Bounds().Width, m_window.get().Bounds().Height);
			return true;
		}
		return false;
	}
	bool Layout::WindowSizeChanged()
	{
		if (m_window.get().Bounds().Height != m_renderPane->Height() + m_menuPane->Height() + m_menuBarPane->Height() ||
			m_window.get().Bounds().Width != m_renderPane->Width() + m_menuPane->Width() + m_menuBarPane->Width())
		{
			UpdatePanes(m_window.get().Bounds().Width, m_window.get().Bounds().Height);
			return true;
		}

		return false;
	}

	bool Layout::SupportHighResolutions()
	{
		return DisplayMetrics::SupportHighResolutions;
	}
}