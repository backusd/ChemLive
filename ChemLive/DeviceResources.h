#pragma once

#include "Layout.h"

using winrt::Windows::Foundation::Size;
using winrt::Windows::Graphics::Display::DisplayOrientations;
using winrt::Windows::UI::Core::CoreWindow;

namespace DX
{
	// Provides an interface for an application that owns DeviceResources to be notified of the device being lost or created.
	interface IDeviceNotify
	{
		virtual void OnDeviceLost() = 0;
		virtual void OnDeviceRestored() = 0;
	};

	class DeviceResources
	{
	public:
		DeviceResources();
		void SetLayout(std::shared_ptr<ChemLive::Layout> layout);
		void SetCurrentOrientation(DisplayOrientations const& currentOrientation);
		void SetDpi(float dpi);
		void ValidateDevice();
		void HandleDeviceLost();
		void RegisterDeviceNotify(DX::IDeviceNotify* deviceNotify);
		void Trim();
		void Present();
		void UpdateStereoState();
		void WindowSizeChanged();		

		// The size of the render target, in pixels.
		Size	GetFullOutputSize() const { return Size(m_layout->FullWindowWidthPixels(), m_layout->FullWindowHeightPixels()); }
		Size	GetRenderPaneOutputSize() const { return Size(m_layout->RenderPaneWidthPixels(), m_layout->RenderPaneHeightPixels()); }

		// The size of the render target, in dips.
		Size	GetFullLogicalSize() const { return Size(m_layout->FullWindowWidthDIPS(), m_layout->FullWindowHeightDIPS()); }
		float	GetDpi() const { return m_layout->DPI(); }
		bool    GetStereoState() const { return m_stereoEnabled; }

		// Get a pointer to the layout
		std::shared_ptr<ChemLive::Layout> GetLayout() { return m_layout; }

		// D3D Accessors.
		ID3D11Device3*			 GetD3DDevice() const { return m_d3dDevice.get(); }
		ID3D11DeviceContext3*	 GetD3DDeviceContext() const { return m_d3dContext.get(); }
		IDXGISwapChain3*		 GetSwapChain() const { return m_swapChain.get(); }
		D3D_FEATURE_LEVEL		 GetDeviceFeatureLevel() const { return m_d3dFeatureLevel; }
		ID3D11RenderTargetView1* GetBackBufferRenderTargetView() const { return m_d3dRenderTargetView.get(); }
		ID3D11DepthStencilView*	 GetDepthStencilView() const { return m_d3dDepthStencilView.get(); }
		D3D11_VIEWPORT			 GetScreenViewport() const { return m_screenViewport; }
		DirectX::XMFLOAT4X4		 GetOrientationTransform3D() const { return m_orientationTransform3D; }

		// D2D Accessors.
		ID2D1Factory3*		 GetD2DFactory() const { return m_d2dFactory.get(); }
		ID2D1Device2*		 GetD2DDevice() const { return m_d2dDevice.get(); }
		ID2D1DeviceContext2* GetD2DDeviceContext() const { return m_d2dContext.get(); }
		ID2D1Bitmap1*		 GetD2DTargetBitmap() const { return m_d2dTargetBitmap.get(); }
		IDWriteFactory3*	 GetDWriteFactory() const { return m_dwriteFactory.get(); }
		IWICImagingFactory2* GetWicImagingFactory() const { return m_wicFactory.get(); }
		D2D1::Matrix3x2F	 GetOrientationTransform2D() const { return m_orientationTransform2D; }

	private:
		void CreateDeviceIndependentResources();
		void CreateDeviceResources();
		void CreateWindowSizeDependentResources();
		DXGI_MODE_ROTATION ComputeDisplayRotation();
		void CheckStereoEnabledStatus();

		// Shared pointer to Layout class that defines the pane layout for the app
		std::shared_ptr<ChemLive::Layout> m_layout;

		// Direct3D objects.
		winrt::com_ptr<ID3D11Device3>			m_d3dDevice;
		winrt::com_ptr<ID3D11DeviceContext3>	m_d3dContext;
		winrt::com_ptr<IDXGISwapChain3>			m_swapChain;

		// Direct3D rendering objects. Required for 3D.
		winrt::com_ptr<ID3D11RenderTargetView1>	m_d3dRenderTargetView;
		winrt::com_ptr<ID3D11DepthStencilView>	m_d3dDepthStencilView;
		D3D11_VIEWPORT							m_screenViewport;

		// Direct2D drawing components.
		winrt::com_ptr<ID2D1Factory3>		m_d2dFactory;
		winrt::com_ptr<ID2D1Device2>		m_d2dDevice;
		winrt::com_ptr<ID2D1DeviceContext2>	m_d2dContext;
		winrt::com_ptr<ID2D1Bitmap1>		m_d2dTargetBitmap;

		// DirectWrite drawing components.
		winrt::com_ptr<IDWriteFactory3>		m_dwriteFactory;
		winrt::com_ptr<IWICImagingFactory2>	m_wicFactory;

		// Cached reference to the Window.
		winrt::agile_ref<CoreWindow> m_window;

		// Cached device properties.
		D3D_FEATURE_LEVEL	m_d3dFeatureLevel;		//
		DisplayOrientations	m_nativeOrientation;	//
		DisplayOrientations	m_currentOrientation;	//
		bool				m_stereoEnabled;		//

		// Transforms used for display orientation.
		D2D1::Matrix3x2F	m_orientationTransform2D;
		DirectX::XMFLOAT4X4	m_orientationTransform3D;

		// The IDeviceNotify can be held directly as it owns the DeviceResources.
		DX::IDeviceNotify* m_deviceNotify;
	};
}