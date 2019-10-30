/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#include "Scene/UMSControl.h"
#include "Scene/tinyxml2.h"
#include <fstream>
#include <filesystem>
#include <CoreMinimal.h>
#include <Engine/Texture.h>
namespace fs = std;

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
#ifdef RPR_VERBOSE
			UE_LOG(LogUMSControl, Log, TEXT("Loading UMS Control File %s"), UTF8_TO_TCHAR(filename.c_str()));
#endif

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
#ifdef RPR_VERBOSE
					UE_LOG(LogUMSControl, Log, TEXT("MatchAll <= %s"), UTF8_TO_TCHAR(matchAll?"true":"false"));
#endif
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
#ifdef RPR_VERBOSE
						UE_LOG(LogUMSControl, Log, TEXT("%s UMS for %s"), UTF8_TO_TCHAR(isEnabled ? " ENABLED" : "DISABLED"), UTF8_TO_TCHAR(enableMaterial));
#endif
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
