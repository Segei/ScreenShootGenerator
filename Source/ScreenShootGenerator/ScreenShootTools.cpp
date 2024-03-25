// Fill out your copyright notice in the Description page of Project Settings.


#include "ScreenShootTools.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"

void UScreenShootTools::MakeScreenShoot(TArray<FVector2D> Positions)
{
	if (!IsValid(SceneCaptureComponent))
	{
		return;
	}
	SceneCaptureComponent->CaptureScene();
	DrawPoints(Positions);
	FString Name = FString::FromInt((ScreenShootName++) * 10);
	SaveScreenShoot(Name);
}

void UScreenShootTools::BeginPlay()
{
	Super::BeginPlay();
	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 1920, 1080, FormatRenderTarget);
	RenderTarget->bForceLinearGamma = true;
	SceneCaptureComponent = GetOwner()->GetComponentByClass<USceneCaptureComponent2D>();
	SceneCaptureComponent->TextureTarget = RenderTarget;
	ScreenShootName = 0;
}

void UScreenShootTools::GetTextureRenderTarget(UTextureRenderTarget2D* RTarget, UTexture2D*& Texture2D)
{
	const ETextureSourceFormat TextureFormat = RTarget->GetTextureFormatForConversionToTexture2D();
	RTarget->UpdateTexture2D(Texture2D, TextureFormat);
#if WITH_EDITORONLY_DATA
	Texture2D->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
	Texture2D->CompressionSettings = Texture2DCompressionSettings;
	Texture2D->Modify();
	Texture2D->MarkPackageDirty();
	Texture2D->PostEditChange();
	Texture2D->UpdateResource();
}

void UScreenShootTools::DrawPoints(TArray<FVector2D> Positions)
{
	if (!IsValid(RenderTarget))
	{
		return;
	}
	UCanvas* Canvas;
	FVector2d Size;
	FDrawToRenderTargetContext Context;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderTarget, Canvas, Size, Context);
	for (FVector2D Position : Positions)
	{
		Canvas->K2_DrawTexture(Texture, Position, FVector2d(BrushSize, BrushSize),
		                       FVector2d::Zero(), FVector2d::UnitVector, FLinearColor::Red);
	}
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
}

void UScreenShootTools::SaveScreenShoot(FString FileName)
{
	FString PackageName = TEXT("/Game/ScreenShoots/");
	PackageName += FileName;
	UPackage* Package = CreatePackage(*PackageName);
	Package->FullyLoad();

	UTexture2D* NewTexture = NewObject<UTexture2D>(Package, *FileName, RF_Public | RF_Standalone | RF_MarkAsRootSet);
	GetTextureRenderTarget(RenderTarget, NewTexture);
	Package->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(NewTexture);

	FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName,
	                                                                  FPackageName::GetAssetPackageExtension());
	bool bSaved = UPackage::SavePackage(Package, NewTexture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone,
	                                    *PackageFileName, GError, nullptr, true, true, SAVE_NoError);
	UKismetRenderingLibrary::ExportRenderTarget(GetWorld(), RenderTarget,
	                                            FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()) +
	                                            "/ScreenShoots/", FileName + ".png");
}
