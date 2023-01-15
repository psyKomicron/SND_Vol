#pragma once
#include <wincodec.h>

namespace Imaging
{
	class IconHelper
	{
	public:
		void WriteHICONToFile(const HICON& hIcon, const std::filesystem::path& filePath);
		HICON LoadIconFromPath(const std::wstring_view& resourcePath);

	private:
		void SaveImage(IWICImagingFactory* imagingFactory, IWICBitmapSource* bitmapSource, const GUID& containerFormatGUID, IStream* hIconStream);
		IStream* GetBitmapSourceStream(IWICImagingFactory* imagingFactory, IWICBitmapSource* bitmapSource, const GUID& containerFormatGUID);
		void AddFrameToWICBitmap(IWICImagingFactory* imagingFactory, IWICBitmapSource* bitmapSource, IWICBitmapEncoder* bitmapEncoder);
		IWICBitmapSource* ConvertBitmapPixelFormat(IWICImagingFactory* imagingFactory, IWICBitmapSource* bitmapSource, const WICPixelFormatGUID& sourcePixelFormat, const WICBitmapDitherType& bitmapDitherType);
	};
}

