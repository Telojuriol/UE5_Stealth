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
		{ "DisplayName", "Enhanced Sight Config" },
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
		{ "Comment", "/** Maximum distance at which a pawn can initially see a target. */" },
#endif
		{ "ModuleRelativePath", "Public/AISenseConfig_EnhancedSight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Maximum distance at which a pawn can initially see a target." },
#endif
		{ "UIMin", "0.0" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LoseSightRadius_MetaData[] = {
		{ "Category", "Sense" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Distance at which a pawn loses sight of a previously seen target. Usually > SightRadius. */" },
#endif
		{ "ModuleRelativePath", "Public/AISenseConfig_EnhancedSight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Distance at which a pawn loses sight of a previously seen target. Usually > SightRadius." },
#endif
		{ "UIMin", "0.0" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PeripheralVisionAngle_MetaData[] = {
		{ "Category", "Sense" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Initial peripheral vision angle in degrees (total cone angle). */" },
#endif
		{ "ModuleRelativePath", "Public/AISenseConfig_EnhancedSight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Initial peripheral vision angle in degrees (total cone angle)." },
#endif
		{ "UIMax", "360.0" },
		{ "UIMin", "0.0" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FinalPeripheralVisionAngle_MetaData[] = {
		{ "Category", "Sense" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Final peripheral vision angle in degrees (total cone angle) after the threshold distance. */" },
#endif
		{ "ModuleRelativePath", "Public/AISenseConfig_EnhancedSight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Final peripheral vision angle in degrees (total cone angle) after the threshold distance." },
#endif
		{ "UIMax", "360.0" },
		{ "UIMin", "0.0" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FinalPeripheralVisionAngleThreesholdDistance_MetaData[] = {
		{ "Category", "Sense" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Horizontal distance at which the FOV angle transitions from PeripheralVisionAngle to FinalPeripheralVisionAngle. */" },
#endif
		{ "ModuleRelativePath", "Public/AISenseConfig_EnhancedSight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Horizontal distance at which the FOV angle transitions from PeripheralVisionAngle to FinalPeripheralVisionAngle." },
#endif
		{ "UIMin", "0.0" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaxDistUp_MetaData[] = {
		{ "Category", "Sense" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Maximum vertical distance upwards the pawn can see. */" },
#endif
		{ "ModuleRelativePath", "Public/AISenseConfig_EnhancedSight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Maximum vertical distance upwards the pawn can see." },
#endif
		{ "UIMin", "0.0" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaxDistDown_MetaData[] = {
		{ "Category", "Sense" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Maximum vertical distance downwards the pawn can see. */" },
#endif
		{ "ModuleRelativePath", "Public/AISenseConfig_EnhancedSight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Maximum vertical distance downwards the pawn can see." },
#endif
		{ "UIMin", "0.0" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bDrawDebug_MetaData[] = {
		{ "Category", "Debug" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** If true, debug shapes for this sense will be drawn in the world. */" },
#endif
		{ "ModuleRelativePath", "Public/AISenseConfig_EnhancedSight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "If true, debug shapes for this sense will be drawn in the world." },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FClassPropertyParams NewProp_Implementation;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_SightRadius;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_LoseSightRadius;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_PeripheralVisionAngle;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_FinalPeripheralVisionAngle;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_FinalPeripheralVisionAngleThreesholdDistance;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_MaxDistUp;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_MaxDistDown;
	static void NewProp_bDrawDebug_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bDrawDebug;
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
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_FinalPeripheralVisionAngle = { "FinalPeripheralVisionAngle", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAISenseConfig_EnhancedSight, FinalPeripheralVisionAngle), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FinalPeripheralVisionAngle_MetaData), NewProp_FinalPeripheralVisionAngle_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_FinalPeripheralVisionAngleThreesholdDistance = { "FinalPeripheralVisionAngleThreesholdDistance", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAISenseConfig_EnhancedSight, FinalPeripheralVisionAngleThreesholdDistance), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FinalPeripheralVisionAngleThreesholdDistance_MetaData), NewProp_FinalPeripheralVisionAngleThreesholdDistance_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_MaxDistUp = { "MaxDistUp", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAISenseConfig_EnhancedSight, MaxDistUp), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaxDistUp_MetaData), NewProp_MaxDistUp_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_MaxDistDown = { "MaxDistDown", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAISenseConfig_EnhancedSight, MaxDistDown), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaxDistDown_MetaData), NewProp_MaxDistDown_MetaData) };
void Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_bDrawDebug_SetBit(void* Obj)
{
	((UAISenseConfig_EnhancedSight*)Obj)->bDrawDebug = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_bDrawDebug = { "bDrawDebug", nullptr, (EPropertyFlags)0x0010000000014005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UAISenseConfig_EnhancedSight), &Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_bDrawDebug_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bDrawDebug_MetaData), NewProp_bDrawDebug_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_Implementation,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_SightRadius,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_LoseSightRadius,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_PeripheralVisionAngle,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_FinalPeripheralVisionAngle,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_FinalPeripheralVisionAngleThreesholdDistance,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_MaxDistUp,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_MaxDistDown,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAISenseConfig_EnhancedSight_Statics::NewProp_bDrawDebug,
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
		{ Z_Construct_UClass_UAISenseConfig_EnhancedSight, UAISenseConfig_EnhancedSight::StaticClass, TEXT("UAISenseConfig_EnhancedSight"), &Z_Registration_Info_UClass_UAISenseConfig_EnhancedSight, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAISenseConfig_EnhancedSight), 2852042337U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Laia_Documents_UE5_Stealth_Stealth_Source_Stealth_Public_AISenseConfig_EnhancedSight_h_157458559(TEXT("/Script/Stealth"),
	Z_CompiledInDeferFile_FID_Users_Laia_Documents_UE5_Stealth_Stealth_Source_Stealth_Public_AISenseConfig_EnhancedSight_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Laia_Documents_UE5_Stealth_Stealth_Source_Stealth_Public_AISenseConfig_EnhancedSight_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
