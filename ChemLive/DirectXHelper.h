#pragma once

#include "pch.h"
#include <ppltasks.h>	// For create_task
//#include <future>

using winrt::Windows::ApplicationModel::Package;
using winrt::Windows::Storage::FileIO;
using winrt::Windows::Storage::StorageFile;
using winrt::Windows::Storage::StorageFolder;
using winrt::Windows::Storage::Streams::IBuffer;
using winrt::Windows::Storage::Streams::DataReader;

namespace DX
{
	template <typename From, typename To>
	void As(From const& from, winrt::com_ptr<To>& to)
	{
		to = from.as<To>();
	}

	inline void ThrowIfFailed(HRESULT hr)
	{
		winrt::check_hresult(hr);
		/*
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch Win32 API errors.
			throw winrt::hresult_error(hr);
		}
		*/
	}

	// Function that reads from a binary file asynchronously.
	inline concurrency::task<std::vector<byte>> ReadDataAsync(const std::wstring& filename)
	{
		return concurrency::create_task([filename]
			{
				StorageFolder storageFolder{ Package::Current().InstalledLocation() };

				// These cause blocking of the thread which could be improved!!!
				// Also, should I be using co_await??
				StorageFile file = storageFolder.GetFileAsync(winrt::hstring(filename.c_str())).get();
				IBuffer buffer = FileIO::ReadBufferAsync(file).get();

				std::vector<byte> returnBuffer;
				returnBuffer.resize(buffer.Length());

				DataReader::FromBuffer(buffer).ReadBytes(winrt::array_view<byte>(returnBuffer));

				return returnBuffer;
			});
	}
	/*
	inline std::future<std::vector<byte>> ReadDataAsync(const std::wstring& filename)
	{
		try {
			StorageFolder folder = Package::Current().InstalledLocation();
			auto file = co_await folder.GetFileAsync(winrt::hstring(filename.c_str()));
			IBuffer fileBuffer = co_await FileIO::ReadBufferAsync(file);
			std::vector<byte> returnBuffer;
			returnBuffer.resize(fileBuffer.Length());
			DataReader::FromBuffer(fileBuffer).ReadBytes(winrt::array_view<byte>(returnBuffer));
			co_return returnBuffer;
		}
		catch (...) {

		}
	}
	*/

	// Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
	inline float ConvertDipsToPixels(float dips, float dpi)
	{
		static const float dipsPerInch = 96.0f;
		return floorf(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
	}

#if defined(_DEBUG)
	// Check for SDK Layer support.
	inline bool SdkLayersAvailable()
	{
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
			0,
			D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
			nullptr,                    // Any feature level will do.
			0,
			D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
			nullptr,                    // No need to keep the D3D device reference.
			nullptr,                    // No need to know the feature level.
			nullptr                     // No need to keep the D3D device context reference.
		);

		return SUCCEEDED(hr);
	}
#endif
}