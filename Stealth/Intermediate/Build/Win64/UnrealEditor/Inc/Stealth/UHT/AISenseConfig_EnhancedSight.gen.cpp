// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Stealth/Public/AISenseConfig_EnhancedSight.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAISenseConfig_EnhancedSight() {}

// Begin Cross Module References
AIMODULE_API UClass* Z_Construct_UClass_UAISenseConfig();
COREUOBJECT_API UClass* Z_Construct_UClass_UClass();
STEALTH_API UClass* Z_Construct_UClass_UAISense_EnhancedSight_NoRegister();
STEALTH_API UClass* Z_Construct_UClass_UAISenseConfig_EnhancedSight();
STEALTH_API UClass* Z_Construct_UClass_UAISenseConfig_EnhancedSight_NoRegister();
UPackage* Z_Construct_UPackage__Script_Stealth();
// End Cross Module References

// Begin Class UAISenseConfig_EnhancedSight
void UAISenseConfig_EnhancedSight::StaticRegisterNativesUAISenseConfig_EnhancedSight()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UAISenseConfig_EnhancedSight);
UClass* Z_Construct_UClass_UAISenseConfig_EnhancedSight_NoRegister()
{
	return UAISenseConfig_EnhancedSight::StaticClass();
}
struct Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "DisplayName", "Enhanced Sight" },
		{ "IncludePath", "AISenseConfig_EnhancedSight.h" },
		{ "ModuleRelativePath", "Public/AISenseConfig_EnhancedSight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Implementation_MetaData[] = {
		{ "Category", "Sense" },
		{ "ModuleRelativePath", "Public/AISenseConfig_EnhancedSight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SightRadius_MetaData[] = {
		{ "Category", "Sense" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Distance a pawn can see */" },
#endif
		{ "ModuleRelativePath", "Public/AISenseConfig_EnhancedSight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Distance a pawn can see" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LoseSightRadius_MetaData[] = {
		{ "Category", "Sense" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Distance a pawn loses sight */" },
#endif
		{ "ModuleRelativePath", "Public/AISenseConfig_EnhancedSight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Distance a pawn loses sight" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PeripheralVisionAngle_MetaData[] = {
		{ "Category", "Sense" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Angle of vision */" },
#endif
		{ "ModuleRelativePath", "Public/AISenseConfig_EnhancedSight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Angle of vision" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FClassPropertyParams NewProp_Implementation;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_SightRadius;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_LoseSightRadius;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_PeripheralVisionAngle;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAISenseConfig_EnhancedSight>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_Implementation = { "Implementation", nullptr, (EPropertyFlags)0x0014000002014015, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAISenseConfig_EnhancedSight, Implementation), Z_Construct_UClass_UClass, Z_Construct_UClass_UAISense_EnhancedSight_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Implementation_MetaData), NewProp_Implementation_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_SightRadius = { "SightRadius", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAISenseConfig_EnhancedSight, SightRadius), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SightRadius_MetaData), NewProp_SightRadius_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_LoseSightRadius = { "LoseSightRadius", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAISenseConfig_EnhancedSight, LoseSightRadius), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LoseSightRadius_MetaData), NewProp_LoseSightRadius_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_PeripheralVisionAngle = { "PeripheralVisionAngle", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAISenseConfig_EnhancedSight, PeripheralVisionAngle), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PeripheralVisionAngle_MetaData), NewProp_PeripheralVisionAngle_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_Implementation,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_SightRadius,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_LoseSightRadius,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_PeripheralVisionAngle,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UAISenseConfig,
	(UObject* (*)())Z_Construct_UPackage__Script_Stealth,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::ClassParams = {
	&UAISenseConfig_EnhancedSight::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::PropPointers),
	0,
	0x001010A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::Class_MetaDataParams), Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UAISenseConfig_EnhancedSight()
{
	if (!Z_Registration_Info_UClass_UAISenseConfig_EnhancedSight.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAISenseConfig_EnhancedSight.OuterSingleton, Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UAISenseConfig_EnhancedSight.OuterSingleton;
}
template<> STEALTH_API UClass* StaticClass<UAISenseConfig_EnhancedSight>()
{
	return UAISenseConfig_EnhancedSight::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UAISenseConfig_EnhancedSight);
UAISenseConfig_EnhancedSight::~UAISenseConfig_EnhancedSight() {}
// End Class UAISenseConfig_EnhancedSight

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_Laia_Documents_UE5_Stealth_Stealth_Source_Stealth_Public_AISenseConfig_EnhancedSight_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UAISenseConfig_EnhancedSight, UAISenseConfig_EnhancedSight::StaticClass, TEXT("UAISenseConfig_EnhancedSight"), &Z_Registration_Info_UClass_UAISenseConfig_EnhancedSight, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAISenseConfig_EnhancedSight), 1155873088U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Laia_Documents_UE5_Stealth_Stealth_Source_Stealth_Public_AISenseConfig_EnhancedSight_h_2987973874(TEXT("/Script/Stealth"),
	Z_CompiledInDeferFile_FID_Users_Laia_Documents_UE5_Stealth_Stealth_Source_Stealth_Public_AISenseConfig_EnhancedSight_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Laia_Documents_UE5_Stealth_Stealth_Source_Stealth_Public_AISenseConfig_EnhancedSight_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
