#include "pch.h"
#include "DropDown.h"

namespace ChemLive
{
	DropDown::DropDown(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2D1_RECT_F parentRect) :
		Control(deviceResources, parentRect),
		m_dropDownBackgroundColor{ D2D1::ColorF::Gray, D2D1::ColorF::Gray, D2D1::ColorF::Gray },
		m_dropDownBorderColor{ D2D1::ColorF::Black, D2D1::ColorF::Black, D2D1::ColorF::Black },
		m_dropDownBorderThickness{ 1.0f, 1.0f, 1.0f },
		m_dropDownOpen(false),
		m_dropDownRect{0.0f, 0.0f, 0.0f, 0.0f}
	{
		DX::ThrowIfFailed(
			m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(m_stateBlock.put())
		);
	}

	void DropDown::CreateDeviceDependentResources(bool includingControls)
	{
		if (ControlIsActive())
		{
			// Before you can run .put(), the value must be nullptr
			m_dropDownBackgroundBrush = nullptr;
			m_dropDownBorderBrush = nullptr;

			DX::ThrowIfFailed(
				m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(m_dropDownBackgroundColor[m_state], m_dropDownBackgroundBrush.put())
			);

			DX::ThrowIfFailed(
				m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(m_dropDownBorderColor[m_state], m_dropDownBorderBrush.put())
			);

			if (includingControls)
			{
				m_headerButton->CreateDeviceDependentResources(true);

				for (unsigned int iii = 0; iii < m_dropDownControls.size(); ++iii)
					m_dropDownControls[iii]->CreateDeviceDependentResources(true);
			}
		}
	}

	void DropDown::ReleaseDeviceDependentResources()
	{
		m_dropDownBackgroundBrush = nullptr;
		m_dropDownBorderBrush = nullptr;

		m_headerButton->ReleaseDeviceDependentResources();

		// Call release for each control as well
		for (unsigned int iii = 0; iii < m_dropDownControls.size(); ++iii)
			m_dropDownControls[iii]->ReleaseDeviceDependentResources();
	}

	void DropDown::Finalize()
	{
		// Responsibilities
		// 1. Finalize the header button
		// 2. Align the top left of the drop down with bottom left of header
		// 3. Assign height of the drop down
		//		- height is the sum of all drop down item heights
		//		- width must be set manually
		// 4. Set parent rect for each drop down item and finalize each drop down item
		//      - Update each item to fill parent width

		// 1. 
		m_headerButton->Finalize();

		// 2. 
		D2D1_RECT_F headerRect = m_headerButton->ControlRect();
		float width = m_dropDownRect.right - m_dropDownRect.left;
		m_dropDownRect.top = headerRect.bottom;
		m_dropDownRect.left = headerRect.left;
		DropDownWidth(width); // reassign the right side because the left was updated

		// 3.
		m_dropDownRect.bottom = m_dropDownRect.top;
		for (unsigned int iii = 0; iii < m_dropDownControls.size(); ++iii)
			m_dropDownRect.bottom += m_dropDownControls[iii]->Height();

		// Don't need to recursively create device dependent resources here because the call to Finalize in a few lines
		// should create the resources for each control anyways
		CreateDeviceDependentResources(false);

		// 4. 
		float marginTop = 0;
		for (unsigned int iii = 0; iii < m_dropDownControls.size(); ++iii)
		{
			m_dropDownControls[iii]->MarginTop(marginTop);			// Set the top of the control to the bottom of the previous control
			m_dropDownControls[iii]->ParentRect(m_dropDownRect);
			m_dropDownControls[iii]->HorizontalAlignment(ChemLive::HorizontalAlignment::STRETCH);
			m_dropDownControls[iii]->Finalize();

			marginTop += m_dropDownControls[iii]->Height();
		}		

		m_finalized = true;
	}

	void DropDown::Render()
	{
		if (ControlIsActive())
		{
			if (!m_finalized)
				throw std::invalid_argument("DropDown has not been finalized");


			// 1. Render the header button
			// 2. Render the dropdown background (if open)
			// 3. Render the dropdown items

			m_headerButton->Render();

			if (m_dropDownOpen)
			{
				DrawDropDownBackground();
				DrawDropDownBorder();

				for (unsigned int iii = 0; iii < m_dropDownControls.size(); ++iii)
					m_dropDownControls[iii]->Render();
			}
		}
	}

	void DropDown::DrawDropDownBackground()
	{
		ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext();
		context->SaveDrawingState(m_stateBlock.get());
		context->BeginDraw();
		context->SetTransform(m_deviceResources->GetOrientationTransform2D());

		context->FillRectangle(&m_dropDownRect, m_dropDownBackgroundBrush.get());

		// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
		// is lost. It will be handled during the next call to Present.
		HRESULT hr = context->EndDraw();
		if (hr != D2DERR_RECREATE_TARGET)
		{
			DX::ThrowIfFailed(hr);
		}

		context->RestoreDrawingState(m_stateBlock.get());
	}

	void DropDown::DrawDropDownBorder()
	{
		if (m_dropDownBorderThickness[m_state] > 0.01f)
		{
			ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext();
			context->SaveDrawingState(m_stateBlock.get());
			context->BeginDraw();
			context->SetTransform(m_deviceResources->GetOrientationTransform2D());

			context->DrawRectangle(&m_dropDownRect, m_dropDownBorderBrush.get(), m_dropDownBorderThickness[m_state]);


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

	void DropDown::UpdateOnPointerHover()
	{

	}

	void DropDown::UpdateOnPointerDown()
	{

	}

	void DropDown::PointerNotOver()
	{
		if (m_state == ControlState::POINTER_HOVER || m_state == ControlState::POINTER_DOWN)
		{
			m_state = ControlState::POINTER_NOT_OVER;
			Finalize();

			for (unsigned int iii = 0; iii < m_dropDownControls.size(); ++iii)
				m_dropDownControls[iii]->PointerNotOver();
		}
	}

	bool DropDown::OnPointerPressed(CoreWindow w, PointerEventArgs const& e)
	{
		// The Control::PointerOverControl method does not set the POINTER_DOWN state, so it must be set here
		m_state = ControlState::POINTER_DOWN;
		Finalize();

		Point p = e.CurrentPoint().Position();

		// If the header button is pressed, pass along the OnPointerPressed call
		if (m_headerButton->PointerOverControl(p))
		{
			m_headerButton->OnPointerPressed(w, e);
		}
		else
		{
			// else, pass along the call to the drop down item the pointer is over
			for (unsigned int iii = 0; iii < m_dropDownControls.size(); ++iii)
			{
				if (m_dropDownControls[iii]->PointerOverControl(p))
				{
					m_dropDownControls[iii]->OnPointerPressed(w, e);
					break;
				}
			}
		}

		// return false to indicate we do not want to capture the pointer while it is down
		return false;
	}

	void DropDown::OnPointerMoved(CoreWindow w, PointerEventArgs const& e)
	{
		// The state should already have been updated in the Control::PointerOverControl method

		Point p = e.CurrentPoint().Position();

		// If the header button is pressed, pass along the OnPointerMoved call
		if (m_headerButton->PointerOverControl(p))
		{
			m_headerButton->OnPointerMoved(w, e);

			// update the drop down controls
			for (unsigned int iii = 0; iii < m_dropDownControls.size(); ++iii)
				m_dropDownControls[iii]->PointerNotOver();
		}
		else
		{
			// else, pass along the call to the drop down item the pointer is over
			for (unsigned int iii = 0; iii < m_dropDownControls.size(); ++iii)
			{
				if (m_dropDownControls[iii]->PointerOverControl(p))
					m_dropDownControls[iii]->OnPointerMoved(w, e);
				else
					m_dropDownControls[iii]->PointerNotOver();
			}
		}
	}

	void DropDown::OnPointerReleased(CoreWindow w, PointerEventArgs const& e)
	{
		// The only reason this method should get called is if the pointer is actively over the control
		// therefore we can update the state to POINT_HOVER and trigger the Click event
		m_state = ControlState::POINTER_HOVER;
		Finalize();

		Point p = e.CurrentPoint().Position();

		// If the header button is pressed, pass along the OnPointerReleased call
		if (m_headerButton->PointerOverControl(p))
		{
			m_headerButton->OnPointerReleased(w, e);
		}
		else
		{
			// else, pass along the call to the drop down item the pointer is over
			for (unsigned int iii = 0; iii < m_dropDownControls.size(); ++iii)
			{
				if (m_dropDownControls[iii]->PointerOverControl(p))
				{
					m_dropDownControls[iii]->OnPointerReleased(w, e);
					break;
				}
			}
		}
	}

	bool DropDown::PointerOverControl(Point p)
	{
		if (m_headerButton->PointerOverControl(p) || (m_dropDownOpen && p.X > m_dropDownRect.left && p.X < m_dropDownRect.right && p.Y > m_dropDownRect.top && p.Y < m_dropDownRect.bottom))
		{
			// Only change the state if we are in POINTER_NOT_OVER
			if (m_state == ControlState::POINTER_NOT_OVER)
			{
				m_state = ControlState::POINTER_HOVER;
				Finalize();
			}

			return true;
		}

		// If the pointer was previously POINTER_HOVER, then update to POINTER_NOT_OVER
		if (m_state == ControlState::POINTER_HOVER || m_state == ControlState::POINTER_DOWN)
		{
			m_state = ControlState::POINTER_NOT_OVER;
			Finalize();
		}

		return false;
	}

	void DropDown::HeaderButtonClick(const winrt::Windows::Foundation::IInspectable i, int args)
	{
		m_dropDownOpen = !m_dropDownOpen;

	}
}