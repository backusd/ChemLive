#pragma once
#include "Pane.h"
#include "DirectXHelper.h"

using winrt::Windows::UI::Core::CoreWindow;
using winrt::Windows::Foundation::Point;
using winrt::Windows::Graphics::Display::DisplayInformation;

namespace ChemLive
{
	/* This class controls the two important things:
	*	1. The background color for the app
	*		a. Each "pane" will have its own background color
	*	2. The size and location of the "panes" that hold content
	*		a. The app will consist of 2 panes:
	*			i.  A rendering pane where the simulation will be drawn
	*			ii. A control pane where button/input control will be present
	*/
	class Layout
	{
	private:
		std::unique_ptr<Pane> m_renderPane;
		std::unique_ptr<Pane> m_menuPane;
		std::unique_ptr<Pane> m_menuBarPane;

		float m_dpi;

		winrt::agile_ref<CoreWindow> m_window;

		void UpdatePanes(float windowWidthDIPS, float windowHeightDIPS);
		void AdjustDPI();

	public:
		Layout(CoreWindow const& window);
		void UpdateLayout(CoreWindow const& window);
		void UpdateLayout(DXGI_MODE_ROTATION displayRotation);


		float DPI() { return m_dpi; }
		bool DPI(float dpi);
		bool WindowSizeChanged();

		CoreWindow GetCoreWindow() { return m_window.get(); }

		// Get full window height/width
		float FullWindowWidthDIPS() { return m_window.get().Bounds().Width; }
		float FullWindowHeightDIPS() { return m_window.get().Bounds().Height; }
		float FullWindowWidthPixels() { return DX::ConvertDipsToPixels(m_window.get().Bounds().Width, m_dpi); }
		float FullWindowHeightPixels() { return DX::ConvertDipsToPixels(m_window.get().Bounds().Height, m_dpi); }

		// Get Background colors
		DirectX::XMVECTORF32 RenderBackground() { return m_renderPane->BackgroundColor(); }
		DirectX::XMVECTORF32 MenuBackground() { return m_menuPane->BackgroundColor(); }
		DirectX::XMVECTORF32 MenuBarBackground() { return m_menuBarPane->BackgroundColor(); }

		// Set Background colors
		void RenderBackground(DirectX::XMVECTORF32 color) { m_renderPane->BackgroundColor(color); }
		void MenuBackground(DirectX::XMVECTORF32 color) { m_menuPane->BackgroundColor(color); }
		void MenuBarBackground(DirectX::XMVECTORF32 color) { m_menuBarPane->BackgroundColor(color); }

		// Get Render Pane values in DIPS
		float RenderPaneTopDIPS() { return m_renderPane->Top(); }
		float RenderPaneLeftDIPS() { return m_renderPane->Left(); }
		float RenderPaneWidthDIPS() { return m_renderPane->Width(); }
		float RenderPaneHeightDIPS() { return m_renderPane->Height(); }
		float RenderPaneRightDIPS() { return m_renderPane->Right(); }
		float RenderPaneBottomDIPS() { return m_renderPane->Bottom(); }
		D2D1_RECT_F RenderPaneRectFDIPS() { return D2D1::RectF(m_renderPane->Left(), m_renderPane->Top(), m_renderPane->Right(), m_renderPane->Bottom()); }

		// Get Render Pane values in physical pixels
		float RenderPaneTopPixels() { return DX::ConvertDipsToPixels(m_renderPane->Top(), m_dpi); }
		float RenderPaneLeftPixels() { return DX::ConvertDipsToPixels(m_renderPane->Left(), m_dpi); }
		float RenderPaneWidthPixels() {	return DX::ConvertDipsToPixels(m_renderPane->Width(), m_dpi); }
		float RenderPaneHeightPixels() { return DX::ConvertDipsToPixels(m_renderPane->Height(), m_dpi); }
		float RenderPaneRightPixels() { return DX::ConvertDipsToPixels(m_renderPane->Right(), m_dpi); }
		float RenderPaneBottomPixels() { return DX::ConvertDipsToPixels(m_renderPane->Bottom(), m_dpi); }
		D2D1_RECT_F RenderPaneRectFPixels()
		{
			return D2D1::RectF(
				DX::ConvertDipsToPixels(m_renderPane->Left(), m_dpi),
				DX::ConvertDipsToPixels(m_renderPane->Top(), m_dpi),
				DX::ConvertDipsToPixels(m_renderPane->Right(), m_dpi),
				DX::ConvertDipsToPixels(m_renderPane->Bottom(), m_dpi)
			);
		}

		// Get Menu Pane values in DIPS
		float MenuPaneTopDIPS() { return m_menuPane->Top(); }
		float MenuPaneLeftDIPS() { return m_menuPane->Left(); }
		float MenuPaneWidthDIPS() { return m_menuPane->Width(); }
		float MenuPaneHeightDIPS() { return m_menuPane->Height(); }
		float MenuPaneRightDIPS() { return m_menuPane->Right(); }
		float MenuPaneBottomDIPS() { return m_menuPane->Bottom(); }
		D2D1_RECT_F MenuPaneRectFDIPS() { return D2D1::RectF(m_menuPane->Left(), m_menuPane->Top(), m_menuPane->Right(), m_menuPane->Bottom()); }

		// Get Menu Pane values in physical pixels
		float MenuPaneTopPixels() { return DX::ConvertDipsToPixels(m_menuPane->Top(), m_dpi); }
		float MenuPaneLeftPixels() { return DX::ConvertDipsToPixels(m_menuPane->Left(), m_dpi); }
		float MenuPaneWidthPixels() { return DX::ConvertDipsToPixels(m_menuPane->Width(), m_dpi); }
		float MenuPaneHeightPixels() { return DX::ConvertDipsToPixels(m_menuPane->Height(), m_dpi); }
		float MenuPaneRightPixels() { return DX::ConvertDipsToPixels(m_menuPane->Right(), m_dpi); }
		float MenuPaneBottomPixels() { return DX::ConvertDipsToPixels(m_menuPane->Bottom(), m_dpi); }
		D2D1_RECT_F MenuPaneRectFPixels()
		{	return D2D1::RectF(
				DX::ConvertDipsToPixels(m_menuPane->Left(), m_dpi),
				DX::ConvertDipsToPixels(m_menuPane->Top(), m_dpi),
				DX::ConvertDipsToPixels(m_menuPane->Right(), m_dpi),
				DX::ConvertDipsToPixels(m_menuPane->Bottom(), m_dpi)
			); 
		}

		// Get Menu Bar Pane values in DIPS
		float MenuBarPaneTopDIPS() { return m_menuBarPane->Top(); }
		float MenuBarPaneLeftDIPS() { return m_menuBarPane->Left(); }
		float MenuBarPaneWidthDIPS() { return m_menuBarPane->Width(); }
		float MenuBarPaneHeightDIPS() { return m_menuBarPane->Height(); }
		float MenuBarPaneRightDIPS() { return m_menuBarPane->Right(); }
		float MenuBarPaneBottomDIPS() { return m_menuBarPane->Bottom(); }
		D2D1_RECT_F MenuBarPaneRectFDIPS() { return D2D1::RectF(m_menuBarPane->Left(), m_menuBarPane->Top(), m_menuBarPane->Right(), m_menuBarPane->Bottom()); }

		// Get Menu Bar Pane values in physical pixels
		float MenuBarPaneTopPixels() { return DX::ConvertDipsToPixels(m_menuBarPane->Top(), m_dpi); }
		float MenuBarPaneLeftPixels() { return DX::ConvertDipsToPixels(m_menuBarPane->Left(), m_dpi); }
		float MenuBarPaneWidthPixels() { return DX::ConvertDipsToPixels(m_menuBarPane->Width(), m_dpi); }
		float MenuBarPaneHeightPixels() { return DX::ConvertDipsToPixels(m_menuBarPane->Height(), m_dpi); }
		float MenuBarPaneRightPixels() { return DX::ConvertDipsToPixels(m_menuBarPane->Right(), m_dpi); }
		float MenuBarPaneBottomPixels() { return DX::ConvertDipsToPixels(m_menuBarPane->Bottom(), m_dpi); }
		D2D1_RECT_F MenuBarPaneRectFPixels()
		{
			return D2D1::RectF(
				DX::ConvertDipsToPixels(m_menuBarPane->Left(), m_dpi),
				DX::ConvertDipsToPixels(m_menuBarPane->Top(), m_dpi),
				DX::ConvertDipsToPixels(m_menuBarPane->Right(), m_dpi),
				DX::ConvertDipsToPixels(m_menuBarPane->Bottom(), m_dpi)
			);
		}

		// Pointer Over Pane methods
		bool RenderPanePointerCaptured() { return m_renderPane->PointerCaptured(); }
		bool MenuPanePointerCaptured() { return m_menuPane->PointerCaptured(); }
		bool MenuBarPanePointerCaptured() { return m_menuBarPane->PointerCaptured(); }

		void RenderPanePointerCaptured(bool captured) { m_renderPane->PointerCaptured(captured); }
		void MenuPanePointerCaptured(bool captured) { m_menuPane->PointerCaptured(captured); }
		void MenuBarPanePointerCaptured(bool captured) { m_menuBarPane->PointerCaptured(captured); }

		bool PointerOverRenderPane(Point point) { return m_renderPane->PointerOverPane(point); }
		bool PointerOverMenuPane(Point point) { return m_menuPane->PointerOverPane(point); }
		bool PointerOverMenuBarPane(Point point) { return m_menuBarPane->PointerOverPane(point); }

		// Other
		bool SupportHighResolutions();

	};
}