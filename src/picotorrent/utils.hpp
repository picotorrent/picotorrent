#pragma once

#include <string>

namespace pt
{
	class Utils
	{
	public:
		static std::wstring ToHumanFileSize(int64_t bytes);
	};
}
