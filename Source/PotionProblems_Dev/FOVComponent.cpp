// Fill out your copyright notice in the Description page of Project Settings.

#include "FOVComponent.h"
#include "ProceduralMeshComponent.h"
#include "PotProbPlayerController.h"
#include "PotProbZDCharacter.h"
#include "Components/PostProcessComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values for this component's properties
UFOVComponent::UFOVComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false; ////// true
	SetIsReplicated(false);
}

void UFOVComponent::GenerateParams(TArray<TObjectPtr<class AActor>> Players)
{
	Params.ClearIgnoredActors();
	Params.AddIgnoredActors(Players);

	OtherPlayers.Empty();
	if (APotProbPlayerController* Controller = Cast<APotProbPlayerController>(GetOwner()->GetInstigatorController()))
	{
		for (AActor* Actor : Players)
		{
			if (Actor == GetOwner())
			{
				continue;
			}
			Controller->HiddenActors.Empty();
			//Controller->HiddenActors.AddUnique(Actor);
			OtherPlayers.AddUnique(Actor);
			
		}
	}
}


// Called when the game starts
void UFOVComponent::BeginPlay()
{
	Super::BeginPlay();

	//ProceduralMesh = GetOwner()->GetComponentByClass<UProceduralMeshComponent>();
	// Set true RenderInMainPass on the Procedural Mesh Component to debug draw mesh while in game
}


// Called every frame
void UFOVComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Only do if not clarivoyant?
	if (APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(GetOwner()))
	{
		if (Character->IsLocallyControlled())
		{
			PerformLineTraces();
			GenerateMesh();
			if (Character->GetIsClarivoyant())
			{
				SetPlayersVisible();
				if (UPostProcessComponent* ProcessComp = GetOwner()->GetComponentByClass<UPostProcessComponent>())
				{
					//Disable Post Process - Doesn't Work
					ProcessComp->bHiddenInGame = true;
				}
			}
			else
			{
				PreformLineTracesPlayers();
				if (UPostProcessComponent* ProcessComp = GetOwner()->GetComponentByClass<UPostProcessComponent>())
				{
					if (ProcessComp->bHiddenInGame)
					{
						ProcessComp->bHiddenInGame = false;
					}
				}
			}
		}
	}
}

void UFOVComponent::PerformLineTraces()
{
	// Clear Vectors each tick
	Vertices.Empty();
	ProceduralMesh->ClearAllMeshSections();

	// Center Vector
	Vertices.Add(FVector(0, 0, TraceHeight + MeshHeight));

	// Get Actor Position
	AActor* Owner = GetOwner();
	FVector Start = Owner->GetActorLocation() + FVector(0, 0, TraceHeight);

	// Get Actor Rotation
	FRotator Rotation = Owner->GetActorRotation();
	FMatrix RotationMatrix = FRotationMatrix(Rotation.GetInverse());

	// Do raycast and store end points as vertices
	for (int32 i = 0; i < NumRays; ++i)
	{
		// Calculate the vertex at end of raycast
		FVector Forward = Owner->GetActorForwardVector();
		Forward = Forward.RotateAngleAxis((360 / NumRays) * -1 * i, FVector(0, 0, 1));
		FVector End = Start + (Forward * RayLength);
		
		// Create vertex at first point of collision
		FHitResult HitResult;
		/*
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
		{
			End = HitResult.Location;
		}
		*/
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Camera, Params))
		{
				End = HitResult.Location;
		}
		FVector NewVertex = End - Start;

		NewVertex += FVector(0, 0, TraceHeight + MeshHeight);

		NewVertex = RotationMatrix.TransformPosition(NewVertex);
		Vertices.Add(NewVertex);

		// Optional: Draw debug lines
		//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 1.0f);
	}
}

void UFOVComponent::GenerateMesh()
{
	// Generate the procedural mesh based on stored hit locations
	TArray<int32> Triangles;

	// Triangles
	for (int i = 0; i < NumRays; ++i)
	{
		int VertexIndex = (i + 1);
		int VertexIndex2 = (i + 2);
		if (VertexIndex2 > NumRays)
		{
			VertexIndex2 = VertexIndex2 % NumRays;
		}

		Triangles.Add(0); //  (0,0, Height)
		Triangles.Add(VertexIndex);
		Triangles.Add(VertexIndex2);
	}

	// Debug to print out all triangle indexes
	/*if (GEngine)
	{
		int index = 0;
		for (int i = 0; i < Triangles.Num(); i += 3)
		{
			FString VecStr = FString::Printf(TEXT("Triangle %i: (%d %d %d)"), index, Triangles[i], Triangles[i+1], Triangles[i+2]);
			GEngine->AddOnScreenDebugMessage(-1, 120.0f, FColor::Yellow, VecStr);
			index++;
		}
	}*/

	ProceduralMesh->ClearAllMeshSections();
	ProceduralMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
	//DynamicMesh->Create
}


void UFOVComponent::PreformLineTracesPlayers()
{
	// GETOWNER() DOESNT WORK SINCE ACTORS DONT EXISTS LOCALLY
	// Gets pawn assocaited with local player controller
	AActor* Owner = GetOwner();
	APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(Owner->GetInstigatorController());
	FVector Start = Owner->GetActorLocation();
	//Start += FVector(0.0f,0.0f, TraceHeight);
	for (AActor* Player : OtherPlayers)
	{
		if (!Player) {
			return;
		}
		bool MakeVisible = false;
		FVector PlayerLoc = Player->GetActorLocation();
		//PlayerLoc.Z = 0.1f;
		//Start.Z = 0.1f;
		double Distance = FVector::Distance(Start, PlayerLoc);
		// Determines if other player is even within range
		if (FMath::Abs(Distance) <= RayLength)
		{
			// Does a ray trace to determine line of sight
			// Create vertex at first point of collision
			FHitResult HitResult;
			if (!GetWorld()->LineTraceSingleByChannel(HitResult, Start, PlayerLoc, ECC_Camera, Params))
			{
				MakeVisible = true;
			}
			/*else
			{
				AActor* AA = HitResult.GetActor();
				if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(AA))
				{
					MakeVisible = true;
				}
			}*/
			//DrawDebugLine(GetWorld(), Start, PlayerLoc, FColor::Red, false, 1.0f, 0, 1.0f);
		}

		//For clarivoyance, add here another check see if player is clarivoyant
		if (MakeVisible)
		{
			PlayerController->HiddenActors.Remove(Player);
			if (APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(Player))
			{
				Character->RevealPlayerComponents();
			}
		}
		else
		{
			PlayerController->HiddenActors.AddUnique(Player);
			if (APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(Player))
			{
				Character->RevealPlayerComponents();
			}
		}
	}
}

void UFOVComponent::SetPlayersVisible()
{
	if (APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(GetOwner()->GetInstigatorController()))
	{
		for (AActor* Player : OtherPlayers)
		{
			PlayerController->HiddenActors.Remove(Player);
			if (APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(Player))
			{
				Character->RevealPlayerComponents();
			}
		}
	}
	
}
