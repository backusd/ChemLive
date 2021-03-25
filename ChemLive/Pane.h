#pragma once

using winrt::Windows::Foundation::Point;

namespace ChemLive
{
	/* A pane simply represents a logical space within the app.
	   It only consists of a top left location, a height, a width,
	   and a background color.
	*/
	class Pane
	{
	private:
		float m_left;
		float m_top;
		float m_width;
		float m_height;
		DirectX::XMVECTORF32 m_backgroundColor;

		// The pointer should be captured when it is interacting with the pane
		// and released once it is done
		bool m_pointerCaptured;

	public:
		Pane();
		Pane(float left, float top, float width, float height, DirectX::XMVECTORF32 backgroundColor);

		float Left() { return m_left; }
		float Top() { return m_top; }
		float Width() { return m_width; }
		float Height() { return m_height; }
		float Right() { return m_left + m_width; }
		float Bottom() { return m_top + m_height; }

		void Left(float left) { m_left = left; }
		void Top(float top) { m_top = top; }
		void Width(float width) { m_width = width; }
		void Height(float height) { m_height = height; }

		DirectX::XMVECTORF32 BackgroundColor() { return m_backgroundColor; }
		void BackgroundColor(DirectX::XMVECTORF32 color) { m_backgroundColor = color; }

		bool PointerCaptured() { return m_pointerCaptured; }
		void PointerCaptured(bool captured) { m_pointerCaptured = captured; }

		// Pointer is over the pane
		bool PointerOverPane(Point p) { return p.X >= m_left && p.X <= Right() && p.Y >= m_top && p.Y <= Bottom(); }

	};

}