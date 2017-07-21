#pragma once

#include <string>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace rpr
{
	class UMSControl
	{
	public:
		UMSControl();

		~UMSControl();

		void Clear();

		void LoadControlData(const std::string& filename);

		bool IsMaterialUMSEnabled(const std::string& name);

	private:

		bool matchAll = false;
		std::unordered_map<std::string, bool> materialEnables;
	};
}