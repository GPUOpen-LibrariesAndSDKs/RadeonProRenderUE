
#ifndef __RADEONPRORENDERTOOLS_H
#define __RADEONPRORENDERTOOLS_H

enum RPR_TOOLS_OS
{
	RPRTOS_WINDOWS,
	RPRTOS_LINUX,
	RPRTOS_MACOS,
};

//if in a project we don't want to include RadeonProRender.h, and we only need to use  IsDeviceNameWhitelisted()
//then we define RADEONPRORENDERTOOLS_DONTUSERPR 
#ifndef RADEONPRORENDERTOOLS_DONTUSERPR


#include "RadeonProRender.h"
#include "RPR_SDKModule.h"


enum RPR_TOOLS_DEVICE
{
	RPRTD_GPU0,
	RPRTD_GPU1,
	RPRTD_GPU2,
	RPRTD_GPU3,
	RPRTD_GPU4,
	RPRTD_GPU5,
	RPRTD_GPU6,
	RPRTD_GPU7,
	RPRTD_CPU,
};

enum RPR_TOOLS_COMPATIBILITY
{
	// device is compatible
	RPRTC_COMPATIBLE = 0,     

	// device is incompatible for unknown reason (mostly because error occurs while testing compatibility)
	RPRTC_INCOMPATIBLE_UNKNOWN,      

	// device is incompatible because not tested by Radeon ProRender team.
	RPRTC_INCOMPATIBLE_UNCERTIFIED,      

	// device is incompatible because a Radeon ProRender creation is reporting a UNSUPPORTED error. It could be because device is not OpenCL 1.2 compatible. 
	RPRTC_INCOMPATIBLE_CONTEXT_UNSUPPORTED, 

	// device is incompatible because a Radeon ProRender creation is reporting an error.
	RPRTC_INCOMPATIBLE_CONTEXT_ERROR,
};

// 'rendererDLL' is the path to plugin DLL : example : "Tahoe64.dll"
// 'tahoePluginID' is the ID of a registered plugin from rprRegisterPlugin("Tahoe64.dll")
RPR_TOOLS_COMPATIBILITY rprIsDeviceCompatible(const rpr_char* rendererDLL, RPR_TOOLS_DEVICE device, rpr_char const * cache_path, bool doWhiteListTest, RPR_TOOLS_OS os, rpr_creation_flags additionalflags = 0);
RPR_TOOLS_COMPATIBILITY rprIsDeviceCompatible(rpr_int tahoePluginID      , RPR_TOOLS_DEVICE device, rpr_char const * cache_path, bool doWhiteListTest, RPR_TOOLS_OS os, rpr_creation_flags additionalflags = 0);


// get the list of Compatible devices in 'devicesUsed'
//
// 'devicesUsed' input value.
// 'devicesCompatibleOut' output value.
// 'rendererDLL' is the path to plugin DLL : example : "Tahoe64.dll"
// 'tahoePluginID' is the ID of a registered plugin from rprRegisterPlugin("Tahoe64.dll")
//
// example : devicesUsed=RPR_CREATION_FLAGS_ENABLE_GPU0|RPR_CREATION_FLAGS_ENABLE_GPU1|RPR_CREATION_FLAGS_ENABLE_GPU2
//           if GPU1 is incompatible
//           devicesCompatibleOut will be return with  RPR_CREATION_FLAGS_ENABLE_GPU0|RPR_CREATION_FLAGS_ENABLE_GPU2
//
RPR_SDK_API void rprAreDevicesCompatible(const rpr_char* rendererDLL, rpr_char const * cache_path, bool doWhiteListTest, rpr_creation_flags devicesUsed,  rpr_creation_flags* devicesCompatibleOut, RPR_TOOLS_OS os);
RPR_SDK_API void rprAreDevicesCompatible(rpr_int tahoePluginID      , rpr_char const * cache_path, bool doWhiteListTest, rpr_creation_flags devicesUsed,  rpr_creation_flags* devicesCompatibleOut, RPR_TOOLS_OS os);

#endif

//return TRUE if device is known as unsupported.
//
// example of 'deviceName' :   "AMD FirePro W8000"
//
bool IsDeviceNameWhitelisted(const char* deviceName, RPR_TOOLS_OS os);


#endif
