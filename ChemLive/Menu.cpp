#include "pch.h"
#include "Menu.h"
#include "TextBox.h"

using winrt::Windows::Foundation::Point;

namespace ChemLive 
{
	Menu::Menu(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2D1_RECT_F rect) :
		m_deviceResources(deviceResources),
		m_menuRect(rect),
		m_controlThatHasCapturedPointerIndex(-1)
	{
		// Create the drawing state block
		/*
		 *    WHY IS THIS NEEDED ?????????????????????
		 */
		DX::ThrowIfFailed(
			m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(m_stateBlock.put())
		);

		// Let includingControls = false because each control's constructor should call it for them
		CreateDeviceDependentResources();
	}

	void Menu::CreateDeviceDependentResources()
	{
		// Before you can run .put(), the value must be nullptr
		m_backgroundBrush = nullptr;

		DX::ThrowIfFailed(
			m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue), m_backgroundBrush.put())
		);

		// Call the same method for each control
		for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
			m_controls[iii]->CreateDeviceDependentResources();
	}
	void Menu::ReleaseDeviceDependentResources()
	{
		m_backgroundBrush = nullptr;

		// Call release for each control as well
		for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
			m_controls[iii]->ReleaseDeviceDependentResources();
	}

	// Performs any updates to the Menu that are needed between frames
	void Menu::Update(DX::StepTimer const& timer)
	{
		/*
		* Perform some kind of update here that is dependent on the timer 
		*/
	}

	// Renders a frame to the screen.
	void Menu::Render()
	{
		DrawBackground();

		// Renders controls (Currently, this is just in the order they were created)
		for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
			m_controls[iii]->Render();
	}

	void Menu::DrawBackground()
	{
		ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext();
		context->SaveDrawingState(m_stateBlock.get());
		context->BeginDraw();
		context->SetTransform(m_deviceResources->GetOrientationTransform2D());
		context->FillRectangle(&m_menuRect, m_backgroundBrush.get());

		// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
		// is lost. It will be handled during the next call to Present.
		HRESULT hr = context->EndDraw();
		if (hr != D2DERR_RECREATE_TARGET)
		{
			DX::ThrowIfFailed(hr);
		}

		context->RestoreDrawingState(m_stateBlock.get());
	}

	// Set Background color
	void Menu::BackgroundColor(D2D1::ColorF color)
	{
		m_backgroundBrush = nullptr;
		DX::ThrowIfFailed(
			m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(color), m_backgroundBrush.put())
		);
	}

	// Update menu size
	void Menu::MenuRect(D2D1_RECT_F rect)
	{
		m_menuRect = rect;

		for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
		{
			m_controls[iii]->ParentRect(m_menuRect);
			/*
			m_controls[iii]->Finalize();
			*/
		}
	}

	// Event Handlers
	void Menu::OnPointerPressed(CoreWindow w, PointerEventArgs const& e)
	{
		// Determine which control the pointer is over and call OnPointerPressed
		// Call OnPointerPressed for EVERY control that it is over

		Point p = e.CurrentPoint().Position();

		for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
		{
			if (m_controls[iii]->PointerOverControl(p))
			{
				// The OnPointerPressed returns bool which indicates whether or not it wants to capture the pointer
				// for the duration that the pointer is down
				if (m_controls[iii]->OnPointerPressed(w, e))
				{
					m_controlThatHasCapturedPointerIndex = iii;
					break;
				}
			}
		}
	}

	void Menu::OnPointerMoved(CoreWindow w, PointerEventArgs const& e)
	{
		// Determine which control the pointer is over and call OnPointerMoved

		Point p = e.CurrentPoint().Position();

		// first check if the pointer has been captured
		if (m_controlThatHasCapturedPointerIndex != -1)
		{
			m_controls[m_controlThatHasCapturedPointerIndex]->OnPointerMoved(w, e);
		}
		else
		{
			for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
			{
				if (m_controls[iii]->PointerOverControl(p))
					m_controls[iii]->OnPointerMoved(w, e);
			}
		}
	}

	void Menu::OnPointerReleased(CoreWindow w, PointerEventArgs const& e)
	{
		// Determine which control the pointer is over and call OnPointerReleased

		Point p = e.CurrentPoint().Position();

		// first check if the pointer has been captured
		if (m_controlThatHasCapturedPointerIndex != -1)
		{
			m_controls[m_controlThatHasCapturedPointerIndex]->OnPointerReleased(w, e);
			m_controlThatHasCapturedPointerIndex = -1;
		}
		else
		{
			for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
			{
				if (m_controls[iii]->PointerOverControl(p))
					m_controls[iii]->OnPointerReleased(w, e);
			}
		}
	}

	void Menu::PointerNotOver()
	{
		for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
			m_controls[iii]->PointerNotOver();
	}
}