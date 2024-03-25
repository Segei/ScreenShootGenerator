// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ScreenShootTools.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCREENSHOOTGENERATOR_API UScreenShootTools : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Settings brush")
	float BrushSize = 10.f;
	

	UPROPERTY(EditDefaultsOnly, Category = "Settings brush")
	UTexture2D* Texture;

	UPROPERTY(EditAnywhere, Category = "Settings brush")
	TEnumAsByte<ETextureRenderTargetFormat> FormatRenderTarget = RTF_RGBA8;

	UPROPERTY(EditAnywhere, Category = "Settings brush")
	TEnumAsByte<TextureCompressionSettings> Texture2DCompressionSettings = TC_HDR_Compressed;
		
	UFUNCTION(BlueprintCallable, Category = "Texture")
	void MakeScreenShoot(TArray<FVector2D> Positions);

	UPROPERTY()
	USceneCaptureComponent2D* SceneCaptureComponent;

	virtual void BeginPlay() override;

protected:
	UPROPERTY()
	UTextureRenderTarget2D* RenderTarget;
	UPROPERTY()
	int32 ScreenShootName;

	void GetTextureRenderTarget(UTextureRenderTarget2D* RTarget, UTexture2D*& Texture2D);
	void DrawPoints(TArray<FVector2D> Positions);
	void SaveScreenShoot(FString FileName);
};
