#include "pch.h"
#include "IconHelper.h"

#include <Shlwapi.h>
#include <wincodec.h>
#include <ocidl.h>

using namespace winrt;
using namespace std;

namespace Imaging
{
	void IconHelper::WriteHICONToFile(const std::filesystem::path& filePath)
	{
		if (!PathFileExists(filePath.c_str()))
		{
			throw std::filesystem::filesystem_error(filePath.string().append(" does not exists"), std::error_code());
		}

		// Get HICON from path.
		HANDLE hImage = LoadImage(nullptr, filePath.c_str(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
		check_bool(hImage);
		HICON hIcon = static_cast<HICON>(hImage);

		// Create factory.
		com_ptr<IWICImagingFactory> wicImagingFactory{};
		check_hresult(
			CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicImagingFactory))
		);

		// Get bitmap and bitmap source from HICON.
		com_ptr<IWICBitmap> wicBitmap{};
		check_hresult(
			wicImagingFactory->CreateBitmapFromHICON(hIcon, wicBitmap.put())
		);
		com_ptr<IWICBitmapSource> wicBitmapSource = wicBitmap;

		// Load bitmap into a stream.
		//	Create stream.
		IStreamPtr fileStream{};
		check_hresult(
			CreateStreamOnHGlobal(nullptr, TRUE, &fileStream)
		);

		//	Load into stream.
		IStreamPtr bitmapStream{};
		com_ptr<IWICBitmapEncoder> wicBitmapEncoder{};
		check_hresult(
			wicImagingFactory->CreateEncoder(GUID_ContainerFormatPng, nullptr, wicBitmapEncoder.put())
		);
		check_hresult(
			wicBitmapEncoder->Initialize(bitmapStream, WICBitmapEncoderNoCache)
		);

	}
}