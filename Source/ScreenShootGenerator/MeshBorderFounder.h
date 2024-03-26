// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeshBorderFounder.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCREENSHOOTGENERATOR_API UMeshBorderFounder : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TEnumAsByte<ECollisionChannel> TraceChannel;
	
	UFUNCTION(BlueprintCallable, Category="Trace")
	TArray<FVector2D> TracePoint();

	UPROPERTY(EditAnywhere)
	float Lenght = 1000.f;

	UPROPERTY(EditAnywhere)
	bool RelativeProject = false;
};
