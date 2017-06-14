// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "DisplaceState.h"


UDisplaceState::UDisplaceState() {
    PrimaryComponentTick.bCanEverTick = true;

    _Block = false;
    _Direction = 1;
    _VelocityLocation = 20;
    _VelocityRotation = 20;

    _LocationFinished = true;
    _RotationFinished = true;

    _State = EDisplaceState::InitialPosition;
}

void UDisplaceState::BeginPlay() {
    Super::BeginPlay();

    _InitialLocation = GetOwner()->GetActorLocation();
    _InitialRotation = GetOwner()->GetActorRotation();
    _FinalLocation = _InitialLocation + _DisplaceLocation;
    _FinalRotation = _InitialRotation + _DisplaceRotation;

    SetComponentTickEnabled(false);
}

void UDisplaceState::TickComponent(float DeltaTime, ELevelTick TickType,
                                   FActorComponentTickFunction* ThisTickFunction) {
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    FVector NewLocation;
    FRotator NewRotation;

    if (!_LocationFinished) {
        NewLocation = FMath::VInterpConstantTo(GetOwner()->GetActorLocation(),
                                               _Direction == 1 ? _FinalLocation : _InitialLocation,
                                               DeltaTime, _VelocityLocation);
        GetOwner()->SetActorLocation(NewLocation);
    }

    if (!_RotationFinished) {
        NewRotation = FMath::RInterpConstantTo(GetOwner()->GetActorRotation(),
                                               _Direction == 1 ? _FinalRotation : _InitialRotation,
                                               DeltaTime, _VelocityRotation);
        GetOwner()->SetActorRotation(NewRotation);
    }

    if (FVector::Dist(NewLocation, _Direction == 1 ? _FinalLocation : _InitialLocation) <= 0) {
        _LocationFinished = true;
    }
    if (NewRotation.Equals(_Direction == 1 ? _FinalRotation : _InitialRotation)) {
        _RotationFinished = true;
    }

    if (_LocationFinished && _RotationFinished) {
        _State = _Direction == 1 ? EDisplaceState::FinalPosition : EDisplaceState::InitialPosition;
        SetComponentTickEnabled(false);
    }
}

/********************************* INTERFACES ****************************************************/
int UDisplaceState::SwitchState_Implementation() {
    if (!_Block) {
        switch (_State) {
            case EDisplaceState::InitialPosition:
            {
                _State = EDisplaceState::Moving;
                _Direction = 1;
                break;
            };
            case EDisplaceState::FinalPosition:
            {
                _State = EDisplaceState::MovingReverse;
                _Direction = -1;
                break;
            };
            case EDisplaceState::Moving:
            {
                _State = EDisplaceState::MovingReverse;
                _Direction = -1;
                break;
            };
            default:
            {
                _State = EDisplaceState::Moving;
                _Direction = -_Direction;
                break;
            };
        }
        _LocationFinished = false;
        _RotationFinished = false;

        SetComponentTickEnabled(true);
    }
    return 0;
}