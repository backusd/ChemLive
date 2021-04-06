#pragma once

#include "Control.h"
#include "Button.h"

#include <vector>

using winrt::Windows::Foundation::EventHandler;

namespace ChemLive
{
	class DropDown : public Control
	{
	private:
		// The main header content should be a single button
		winrt::com_ptr<Button> m_headerButton;

		// Drop down content should generically be a list of controls
		// Could add a Line control if we wanted to be able to add horizontal lines between items
		std::vector<winrt::com_ptr<Control>> m_dropDownControls;

		// Parameters for rendering
		// We don't need special parameters for the header button because its parameters are entirely
		// self-contained. However, we do want to render a background rect for the drop down so there 
		// aren't off-color gaps between the drop down items
		D2D1::ColorF m_dropDownBackgroundColor[3];
		D2D1::ColorF m_dropDownBorderColor[3];
		float        m_dropDownBorderThickness[3];

		// Resources for rendering
		winrt::com_ptr<ID2D1SolidColorBrush>    m_dropDownBackgroundBrush;
		winrt::com_ptr<ID2D1SolidColorBrush>    m_dropDownBorderBrush;
		winrt::com_ptr<ID2D1DrawingStateBlock1>	m_stateBlock;

		D2D1_RECT_F m_dropDownRect;
		bool        m_dropDownOpen;


		void DrawDropDownBackground();
		void DrawDropDownBorder();

	public:
		DropDown(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2D1_RECT_F parentRect);

		// Pure Virtual functions from Control class
		void Render();
		void CreateDeviceDependentResources(bool includingControls);
		void ReleaseDeviceDependentResources();
		void UpdateOnPointerHover();
		void UpdateOnPointerDown();
		void Finalize();
		void PointerNotOver();
		bool OnPointerPressed(CoreWindow w, PointerEventArgs const& e);
		void OnPointerMoved(CoreWindow w, PointerEventArgs const& e);
		void OnPointerReleased(CoreWindow w, PointerEventArgs const& e);

		// Virtual functions that are optionally overridden
		bool PointerOverControl(Point p);

		// Click events
		void HeaderButtonClick(const winrt::Windows::Foundation::IInspectable, int);

		// Set header button and add drop down item
		void SetHeaderButton(winrt::com_ptr<Button> button) 
		{ 
			m_headerButton = button; 
			m_headerButton->ButtonClick({ this, &DropDown::HeaderButtonClick });
		}
		void AddDropDownItem(winrt::com_ptr<Control> control) { m_dropDownControls.push_back(control); }

		// Set Methods for the drop down background
		void DropDownBackgroundColor(D2D1::ColorF color) { m_finalized = false; m_dropDownBackgroundColor[0] = color; }
		void DropDownBackgroundColorOnPointerHover(D2D1::ColorF color) { m_finalized = false; m_dropDownBackgroundColor[1] = color; }
		void DropDownBackgroundColorOnPointerDown(D2D1::ColorF color) { m_finalized = false; m_dropDownBackgroundColor[2] = color; }

		void DropDownBorderColor(D2D1::ColorF color) { m_finalized = false; m_dropDownBorderColor[0] = color; }
		void DropDownBorderColorOnPointerHover(D2D1::ColorF color) { m_finalized = false; m_dropDownBorderColor[1] = color; }
		void DropDownBorderColorOnPointerDown(D2D1::ColorF color) { m_finalized = false; m_dropDownBorderColor[2] = color; }

		void DropDownBorderThickness(float thickness) { m_finalized = false; m_dropDownBorderThickness[0] = thickness; }
		void DropDownBorderThicknessOnPointerHover(float thickness) { m_finalized = false; m_dropDownBorderThickness[1] = thickness; }
		void DropDownBorderThicknessOnPointerDown(float thickness) { m_finalized = false; m_dropDownBorderThickness[2] = thickness; }

		void DropDownWidth(float width) { m_finalized = false; m_dropDownRect.right = m_dropDownRect.left + width; }

		// Get Methods
		D2D1::ColorF DropDownBackgroundColor() { return m_dropDownBackgroundColor[0]; }
		D2D1::ColorF DropDownBackgroundColorOnPointerHover() { return m_dropDownBackgroundColor[1]; }
		D2D1::ColorF DropDownBackgroundColorOnPointerDown() { return m_dropDownBackgroundColor[2]; }

		D2D1::ColorF DropDownBorderColor() { return m_dropDownBorderColor[0]; }
		D2D1::ColorF DropDownBorderColorOnPointerHover() { return m_dropDownBorderColor[1]; }
		D2D1::ColorF DropDownBorderColorOnPointerDown() { return m_dropDownBorderColor[2]; }

		float DropDownBorderThickness() { return m_dropDownBorderThickness[0]; }
		float DropDownBorderThicknessOnPointerHover() { return m_dropDownBorderThickness[1]; }
		float DropDownBorderThicknessOnPointerDown() { return m_dropDownBorderThickness[2]; }
	};
}
