#include "UMSControl.h"
#include "tinyxml2.h"
#include <fstream>
#include <experimental/filesystem>
#include <CoreMinimal.h>
#include <Engine/Texture.h>
namespace fs = std::experimental::filesystem;

namespace rpr
{
	DEFINE_LOG_CATEGORY_STATIC(LogUMSControl, Log, All);

	
	UMSControl::UMSControl()
	{
	}

	UMSControl::~UMSControl()
	{

	}

	void UMSControl::Clear()
	{
		materialEnables.clear();
	}

	void UMSControl::LoadControlData(const std::string & filename)
	{
		// Allow for reload
		materialEnables.clear();

		// Exclude by default
		matchAll = false;

		// Open the xml configuration file.
		// If the file is missing then UMS remains disabled
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(filename.c_str())== tinyxml2::XML_SUCCESS)
		{
			UE_LOG(LogUMSControl, Log, TEXT("Loading UMS Control File %s"), UTF8_TO_TCHAR(filename.c_str()));

			// First, check for UMSControl
			auto elem = doc.FirstChildElement("umscontrol");
			if (elem) {
				// Now - do we have a matchall node?
				auto matchAllNode = elem->FirstChildElement("matchall");
				if (matchAllNode) {
					auto matchAllValueString = matchAllNode->Attribute("value");
					if (matchAllValueString && std::string(matchAllValueString) == "true") {
						this->matchAll = true;
					}
					else {
						this->matchAll = false;
					}
					UE_LOG(LogUMSControl, Log, TEXT("MatchAll <= %s"), UTF8_TO_TCHAR(matchAll?"true":"false"));
				}

				// Finally - look for enables (no value => true)
				auto enableNode = elem->FirstChildElement("enable");
				while (enableNode) {
					auto enableMaterial = enableNode->Attribute("material");
					if (enableMaterial) {
						auto matchAllValueString = enableNode->Attribute("value");
						bool isEnabled = false;
						if (!matchAllValueString || std::string(matchAllValueString) == "true") {
							isEnabled = true;
						}
						materialEnables.emplace(std::make_pair(enableMaterial, isEnabled));
						UE_LOG(LogUMSControl, Log, TEXT("%s UMS for %s"), UTF8_TO_TCHAR(isEnabled ? " ENABLED" : "DISABLED"), UTF8_TO_TCHAR(enableMaterial));
					}
					enableNode = enableNode->NextSiblingElement();
				}
			}
			else {
				UE_LOG(LogUMSControl, Error, TEXT("UMS Control File %s is missing umscontrol node"), UTF8_TO_TCHAR(filename.c_str()));

			}
		}
		else {
			UE_LOG(LogUMSControl, Warning, TEXT("UMS File %s NOT FOUND OR INVALID"), UTF8_TO_TCHAR(filename.c_str()));
		}

	}

	bool UMSControl::IsMaterialUMSEnabled(const std::string & name)
	{
		bool foundValue = matchAll;
		auto itr = materialEnables.find(name);
		if (itr != materialEnables.end())
		{
			foundValue = itr->second;
		}
		return foundValue;
	}	
}