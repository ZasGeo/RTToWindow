#pragma once

#include <Math/Vector3i.hpp>

struct WorldPosition
{
	Vector3i m_Index;
	Vector3 m_Offset;
};

inline WorldPosition ZeroPos()
{
    WorldPosition result = {};
    return result;
}

inline WorldPosition FitIntoChunk(WorldPosition pos, Vector3 chunkDim)
{
    Vector3i offset = Vector3iFromVector3(pos.m_Offset / chunkDim);

    WorldPosition result;
    result.m_Index = pos.m_Index + offset;
    result.m_Offset = pos.m_Offset - offset * chunkDim;
    return result;
}

inline Vector3 Substruct(WorldPosition left, WorldPosition right, Vector3 chunkDim)
{
    Vector3 tileDiff = Vector3FromVector3i(left.m_Index - right.m_Index) * chunkDim;

    Vector3 result = tileDiff + (left.m_Offset - right.m_Offset);
    return result;
}

inline WorldPosition ToWorldPos(WorldPosition simOrigin, Vector3 simPos, Vector3 chunkDim)
{
    simOrigin.m_Offset += simPos;

    WorldPosition result = FitIntoChunk(simOrigin, chunkDim);

    return result;
}

inline Vector3 ToSimPos(WorldPosition pos, WorldPosition simOrigin, Vector3 chunkDim)
{
    return Substruct(pos, simOrigin, chunkDim);
}