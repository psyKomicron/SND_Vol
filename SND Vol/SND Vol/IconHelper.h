#pragma once

#include <filesystem>

namespace Imaging
{
	class IconHelper
	{
	public:
		void WriteHICONToFile(const std::filesystem::path& filePath);
	};
}

