// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "RPRPlugin.generated.dep.h"
PRAGMA_DISABLE_OPTIMIZATION
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCode1RPRPlugin() {}
#if USE_COMPILED_IN_NATIVES
// Cross Module References
	ENGINE_API class UClass* Z_Construct_UClass_AActor();

	RPRPLUGIN_API class UFunction* Z_Construct_UFunction_AactorRPRHandler_2_Init();
	RPRPLUGIN_API class UFunction* Z_Construct_UFunction_AactorRPRHandler_2_RenderTestScene();
	RPRPLUGIN_API class UClass* Z_Construct_UClass_AactorRPRHandler_2_NoRegister();
	RPRPLUGIN_API class UClass* Z_Construct_UClass_AactorRPRHandler_2();
	RPRPLUGIN_API class UPackage* Z_Construct_UPackage__Script_RPRPlugin();
	void AactorRPRHandler_2::StaticRegisterNativesAactorRPRHandler_2()
	{
		UClass* Class = AactorRPRHandler_2::StaticClass();
		static const TNameNativePtrPair<ANSICHAR> AnsiFuncs[] = {
			{ "Init", (Native)&AactorRPRHandler_2::execInit },
			{ "RenderTestScene", (Native)&AactorRPRHandler_2::execRenderTestScene },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, AnsiFuncs, 2);
	}
	UFunction* Z_Construct_UFunction_AactorRPRHandler_2_Init()
	{
		UObject* Outer=Z_Construct_UClass_AactorRPRHandler_2();
		static UFunction* ReturnFunction = NULL;
		if (!ReturnFunction)
		{
			ReturnFunction = new(EC_InternalUseOnlyConstructor, Outer, TEXT("Init"), RF_Public|RF_Transient|RF_MarkAsNative) UFunction(FObjectInitializer(), NULL, 0x04020401, 65535);
			ReturnFunction->Bind();
			ReturnFunction->StaticLink();
#if WITH_METADATA
			UMetaData* MetaData = ReturnFunction->GetOutermost()->GetMetaData();
			MetaData->SetValue(ReturnFunction, TEXT("Category"), TEXT("RPR"));
			MetaData->SetValue(ReturnFunction, TEXT("ModuleRelativePath"), TEXT("Public/actorRPRHandler_2.h"));
#endif
		}
		return ReturnFunction;
	}
	UFunction* Z_Construct_UFunction_AactorRPRHandler_2_RenderTestScene()
	{
		UObject* Outer=Z_Construct_UClass_AactorRPRHandler_2();
		static UFunction* ReturnFunction = NULL;
		if (!ReturnFunction)
		{
			ReturnFunction = new(EC_InternalUseOnlyConstructor, Outer, TEXT("RenderTestScene"), RF_Public|RF_Transient|RF_MarkAsNative) UFunction(FObjectInitializer(), NULL, 0x04020401, 65535);
			ReturnFunction->Bind();
			ReturnFunction->StaticLink();
#if WITH_METADATA
			UMetaData* MetaData = ReturnFunction->GetOutermost()->GetMetaData();
			MetaData->SetValue(ReturnFunction, TEXT("Category"), TEXT("RPR"));
			MetaData->SetValue(ReturnFunction, TEXT("ModuleRelativePath"), TEXT("Public/actorRPRHandler_2.h"));
#endif
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_AactorRPRHandler_2_NoRegister()
	{
		return AactorRPRHandler_2::StaticClass();
	}
	UClass* Z_Construct_UClass_AactorRPRHandler_2()
	{
		static UClass* OuterClass = NULL;
		if (!OuterClass)
		{
			Z_Construct_UClass_AActor();
			Z_Construct_UPackage__Script_RPRPlugin();
			OuterClass = AactorRPRHandler_2::StaticClass();
			if (!(OuterClass->ClassFlags & CLASS_Constructed))
			{
				UObjectForceRegistration(OuterClass);
				OuterClass->ClassFlags |= 0x20900080;

				OuterClass->LinkChild(Z_Construct_UFunction_AactorRPRHandler_2_Init());
				OuterClass->LinkChild(Z_Construct_UFunction_AactorRPRHandler_2_RenderTestScene());

				OuterClass->AddFunctionToFunctionMapWithOverriddenName(Z_Construct_UFunction_AactorRPRHandler_2_Init(), "Init"); // 4243270511
				OuterClass->AddFunctionToFunctionMapWithOverriddenName(Z_Construct_UFunction_AactorRPRHandler_2_RenderTestScene(), "RenderTestScene"); // 1585207292
				static TCppClassTypeInfo<TCppClassTypeTraits<AactorRPRHandler_2> > StaticCppClassTypeInfo;
				OuterClass->SetCppTypeInfo(&StaticCppClassTypeInfo);
				OuterClass->StaticLink();
#if WITH_METADATA
				UMetaData* MetaData = OuterClass->GetOutermost()->GetMetaData();
				MetaData->SetValue(OuterClass, TEXT("IncludePath"), TEXT("actorRPRHandler_2.h"));
				MetaData->SetValue(OuterClass, TEXT("ModuleRelativePath"), TEXT("Public/actorRPRHandler_2.h"));
#endif
			}
		}
		check(OuterClass->GetClass());
		return OuterClass;
	}
	IMPLEMENT_CLASS(AactorRPRHandler_2, 1931930279);
	static FCompiledInDefer Z_CompiledInDefer_UClass_AactorRPRHandler_2(Z_Construct_UClass_AactorRPRHandler_2, &AactorRPRHandler_2::StaticClass, TEXT("/Script/RPRPlugin"), TEXT("AactorRPRHandler_2"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AactorRPRHandler_2);
	UPackage* Z_Construct_UPackage__Script_RPRPlugin()
	{
		static UPackage* ReturnPackage = nullptr;
		if (!ReturnPackage)
		{
			ReturnPackage = CastChecked<UPackage>(StaticFindObjectFast(UPackage::StaticClass(), nullptr, FName(TEXT("/Script/RPRPlugin")), false, false));
			ReturnPackage->SetPackageFlags(PKG_CompiledIn | 0x00000000);
			FGuid Guid;
			Guid.A = 0x1EB35038;
			Guid.B = 0x038859ED;
			Guid.C = 0x00000000;
			Guid.D = 0x00000000;
			ReturnPackage->SetGuid(Guid);

		}
		return ReturnPackage;
	}
#endif
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
PRAGMA_ENABLE_OPTIMIZATION
