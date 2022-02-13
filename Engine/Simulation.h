#pragma once

#include <Engine/Entity.h>
#include <Engine/World.h>
#include <Engine/WorldPosition.h>
 
struct Simulation
{
    WorldPosition m_Origin;
    Vector3 m_SimulationDimMetres;

    EntityStorage m_Entities;
};

inline Simulation* CreateSimulation(MemoryBlock* mem, WorldPosition pos, Vector3 dim)
{
    Simulation* result = PushStruct(*mem, Simulation);
    result->m_Origin = pos;
    result->m_SimulationDimMetres = dim;

    InitializeEntiityStorage(&(result->m_Entities));

    return result;
}

void UpdateSimulation(Simulation* sim, World* world, WorldPosition newOrigin);