#include "pch.h"
#include "Pane.h"

namespace ChemLive
{
	Pane::Pane() :
		m_left(0.0f),
		m_top(0.0f),
		m_width(1.0f),
		m_height(1.0f),
		m_backgroundColor(DirectX::Colors::Black),
		m_pointerCaptured(false)
	{
	}

	Pane::Pane(float left, float top, float width, float height, DirectX::XMVECTORF32 backgroundColor) :
		m_left(left),
		m_top(top),
		m_width(width),
		m_height(height),
		m_backgroundColor(backgroundColor),
		m_pointerCaptured(false)
	{
	}
}