// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "GrabItem.generated.h"

DECLARE_EVENT(UGrabItem, FGrabEvent);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LISTENTOMYVOICE_API UGrabItem : public UActorComponent {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category=Movement)
    float _AttachVelocity;
    UPROPERTY(EditAnywhere, Category = Movement)
    float _AttachDistance;

    UPROPERTY(EditAnywhere)
    FVector _locationAttach_L;
    UPROPERTY(EditAnywhere)
    FRotator _rotationAttach_L;
    UPROPERTY(EditAnywhere)
    FVector _locationAttach_R;
    UPROPERTY(EditAnywhere)
    FRotator _rotationAttach_R;
    UPROPERTY(EditAnyWhere)
    FVector _locationAttach_C;
    UPROPERTY(EditAnywhere)
    FRotator _rotationAttach_C;

    FGrabEvent _GrabItemEvent;
    FDelegateHandle _OnGrabItemDelegateHandle;

    UGrabItem();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    void BeginGrab(USceneComponent* Target, FName SocketName);
    void EndGrab(bool IsReleased = false);

    void AddOnGrabDelegate(FGrabDelegate& GrabDelegate);
    void ClearOnGrabDelegate();

protected:
    virtual void BeginPlay() override;

private:
    bool _IsBeingTaked;
    UMeshComponent* _SourceMesh;
    UMeshComponent* _TargetMesh;
    FName _SocketName;
};
