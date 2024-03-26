// Fill out your copyright notice in the Description page of Project Settings.


#include "ScreenShootTools.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMathLibrary.h"
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
#if WITH_EDITOR
	Texture2D->PostEditChange();
#endif
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
	FVector2D ViewportSize;
	FDrawToRenderTargetContext Context;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderTarget, Canvas, Size, Context);
	GetWorld()->GetFirstPlayerController()->GetLocalPlayer()->ViewportClient->GetViewportSize(ViewportSize);
	UE_LOG(LogTemp, Warning, TEXT("Size = %s"), *Size.ToString());
	UE_LOG(LogTemp, Warning, TEXT("ViewportSize = %s"), *ViewportSize.ToString());

	for (FVector2D Position : Positions)
	{
		float AlphaResolutionX = InverseLerp(0, ViewportSize.X, Position.X);
		float AlphaResolutionY = InverseLerp(0, ViewportSize.Y, Position.Y);
		FVector2D ResultPos = FVector2D(FMath::Lerp(0, Size.X, AlphaResolutionX),
		                                FMath::Lerp(0, Size.Y, AlphaResolutionY));
		UE_LOG(LogTemp, Warning, TEXT("AlphaX = %f"), AlphaResolutionX);
		UE_LOG(LogTemp, Warning, TEXT("AlphaY = %f"), AlphaResolutionY);
		UE_LOG(LogTemp, Warning, TEXT("Position = %s"), *Position.ToString());
		/*Canvas->K2_DrawTexture(Texture, Position, FVector2d(BrushSize, BrushSize),
		                       FVector2d::Zero(), FVector2d::UnitVector, FLinearColor::Yellow);*/
		UE_LOG(LogTemp, Warning, TEXT("ResultPos = %s"), *ResultPos.ToString());
		Canvas->K2_DrawTexture(Texture, ResultPos, FVector2d(BrushSize, BrushSize),
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
#if WITH_EDITOR
	bool bSaved = UPackage::SavePackage(Package, NewTexture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone,
	                                    *PackageFileName, GError, nullptr, true, true, SAVE_NoError);
#endif
	UKismetRenderingLibrary::ExportRenderTarget(GetWorld(), RenderTarget,
	                                            FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()) +
	                                            "/ScreenShoots/", FileName + ".png");
}

double UScreenShootTools::InverseLerp(float A, float B, float Value)
{
	return (Value - A) / (B - A);
}
