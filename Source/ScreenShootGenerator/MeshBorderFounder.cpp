// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshBorderFounder.h"
#include "CineCameraComponent.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


TArray<FVector2D> UMeshBorderFounder::TracePoint()
{
	TArray<FVector2D> ResultPoint;
	TArray<FVector> HitResults;
	UCameraComponent* Camera = GetOwner()->GetComponentByClass<UCameraComponent>();
	FHitResult HitResult;
	TraceChannel = ECC_GameTraceChannel1;
	FCollisionQueryParams Parameters = FCollisionQueryParams::DefaultQueryParam;
	
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, Camera->GetComponentLocation(),
	                                          Camera->GetComponentLocation() + (Camera->GetForwardVector() * Lenght),
	                                          TraceChannel))
	{
		UE_LOG(LogTemp, Warning, TEXT("Don't found trace objects"));
		return ResultPoint;
	}
	FVector PointCenter = HitResult.ImpactPoint;
	FCollisionShape Shape = FCollisionShape::MakeBox(FVector(1, 500, 500));
	float FOV = Camera->FieldOfView;
	FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(FVector::Zero(), Camera->GetUpVector());
	
	
	if (GetWorld()->SweepSingleByChannel(HitResult, PointCenter + (Camera->GetUpVector() * Lenght), PointCenter,
	                                     Rotator.Quaternion(), TraceChannel, Shape))
	{
		HitResults.Add(HitResult.ImpactPoint);
	}

	if (GetWorld()->SweepSingleByChannel(HitResult, PointCenter - (Camera->GetUpVector() * Lenght), PointCenter,
										 Rotator.Quaternion(), TraceChannel, Shape))
	{
		HitResults.Add(HitResult.ImpactPoint);
	}
	Rotator = UKismetMathLibrary::FindLookAtRotation(FVector::Zero(), Camera->GetRightVector());
	FRotator ResultAngle = UKismetMathLibrary::ComposeRotators(Rotator, FRotator(0, (FOV/10), 0));
	if (GetWorld()->SweepSingleByChannel(HitResult, PointCenter + (Camera->GetRightVector() * Lenght), PointCenter,
										 ResultAngle.Quaternion(), TraceChannel, Shape))
	{
		HitResults.Add(HitResult.ImpactPoint);
	}
	ResultAngle = UKismetMathLibrary::ComposeRotators(Rotator, FRotator(0,-(FOV/10),0));
	if (GetWorld()->SweepSingleByChannel(HitResult, PointCenter - (Camera->GetRightVector() * Lenght), PointCenter,
										 ResultAngle.Quaternion(), TraceChannel, Shape))
	{
		HitResults.Add(HitResult.ImpactPoint);
	}

	for (auto Result : HitResults)
	{
	    //UKismetSystemLibrary::DrawDebugSphere(GetWorld(), Result, 1.f, 6, FLinearColor::Blue, 0);
		UE_LOG(LogTemp, Warning, TEXT("Point 3D = %s"), *Result.ToString());
		FVector2D OnScreenPoint;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Result, OnScreenPoint, RelativeProject);
		//UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetWorld()->GetFirstPlayerController(), Result, OnScreenPoint, RelativeProject);
		UE_LOG(LogTemp, Warning, TEXT("Point 2D = %s"), *OnScreenPoint.ToString());
		ResultPoint.Add(OnScreenPoint);
	}
	return ResultPoint;
}
