#pragma once

namespace ChemLive
{
	class EventArgs : public winrt::implements<EventArgs, winrt::Windows::Foundation::IInspectable>
	{
	public:
		EventArgs();
	};
}