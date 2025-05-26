// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Stealth/Public/AISense_EnhancedSight.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAISense_EnhancedSight() {}

// Begin Cross Module References
AIMODULE_API UClass* Z_Construct_UClass_UAISense();
STEALTH_API UClass* Z_Construct_UClass_UAISense_EnhancedSight();
STEALTH_API UClass* Z_Construct_UClass_UAISense_EnhancedSight_NoRegister();
UPackage* Z_Construct_UPackage__Script_Stealth();
// End Cross Module References

// Begin Class UAISense_EnhancedSight
void UAISense_EnhancedSight::StaticRegisterNativesUAISense_EnhancedSight()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UAISense_EnhancedSight);
UClass* Z_Construct_UClass_UAISense_EnhancedSight_NoRegister()
{
	return UAISense_EnhancedSight::StaticClass();
}
struct Z_Construct_UClass_UAISense_EnhancedSight_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "ClassGroupNames", "AI" },
		{ "IncludePath", "AISense_EnhancedSight.h" },
		{ "ModuleRelativePath", "Public/AISense_EnhancedSight.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAISense_EnhancedSight>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UAISense_EnhancedSight_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UAISense,
	(UObject* (*)())Z_Construct_UPackage__Script_Stealth,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAISense_EnhancedSight_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UAISense_EnhancedSight_Statics::ClassParams = {
	&UAISense_EnhancedSight::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x001000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAISense_EnhancedSight_Statics::Class_MetaDataParams), Z_Construct_UClass_UAISense_EnhancedSight_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UAISense_EnhancedSight()
{
	if (!Z_Registration_Info_UClass_UAISense_EnhancedSight.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAISense_EnhancedSight.OuterSingleton, Z_Construct_UClass_UAISense_EnhancedSight_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UAISense_EnhancedSight.OuterSingleton;
}
template<> STEALTH_API UClass* StaticClass<UAISense_EnhancedSight>()
{
	return UAISense_EnhancedSight::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UAISense_EnhancedSight);
UAISense_EnhancedSight::~UAISense_EnhancedSight() {}
// End Class UAISense_EnhancedSight

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_Laia_Documents_UE5_Stealth_Stealth_Source_Stealth_Public_AISense_EnhancedSight_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UAISense_EnhancedSight, UAISense_EnhancedSight::StaticClass, TEXT("UAISense_EnhancedSight"), &Z_Registration_Info_UClass_UAISense_EnhancedSight, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAISense_EnhancedSight), 954545061U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Laia_Documents_UE5_Stealth_Stealth_Source_Stealth_Public_AISense_EnhancedSight_h_958592070(TEXT("/Script/Stealth"),
	Z_CompiledInDeferFile_FID_Users_Laia_Documents_UE5_Stealth_Stealth_Source_Stealth_Public_AISense_EnhancedSight_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Laia_Documents_UE5_Stealth_Stealth_Source_Stealth_Public_AISense_EnhancedSight_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
