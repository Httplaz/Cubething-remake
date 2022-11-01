#include "ChunkHolder.h"

ivec3 ChunkHolder::update(vec3 loaderPos)
{
    ivec3 delta1 = ((loadSide-1) * Chunk::side / 2 - (ivec3)loaderPos) / int32_t(Chunk::side);
    delta1.y = 0;
    ivec3 delta2 = delta1;
    ivec3 delta;
    while (delta1 != ivec3(0))
    {
        if (delta1.x != 0)
            delta = ivec3(delta1.x, 0, 0), delta1.x = 0;
        else if (delta1.y != 0)
            delta = ivec3(0, delta1.y, 0), delta1.y = 0;
        else if (delta1.z != 0)
            delta = ivec3(0, 0, delta1.z), delta1.z = 0;
        chunkOffset -= delta;

        for (int x = 0; x < loadSide.x; x++)
            for (int y = 0; y < loadSide.y; y++)
                for (int z = 0; z < loadSide.z; z++)
                    if (
                        ((delta.x > 0 && x >= loadSide.x - delta.x) || (delta.x < 0 && x < -delta.x)) ||
                        ((delta.y > 0 && y >= loadSide.y - delta.y) || (delta.y < 0 && y < -delta.y)) ||
                        ((delta.z > 0 && z >= loadSide.z - delta.z) || (delta.z < 0 && z < -delta.z))
                        )
                    {
                        chunkRing.get({ x, y, z })->worldMove(-delta * loadSide);
                        refillChunk(chunkRing.get({ x, y, z }));
                    }
        chunkRing.shift(delta);
        for (int x = 0; x < loadSide.x; x++)
            for (int y = 0; y < loadSide.y; y++)
                for (int z = 0; z < loadSide.z; z++)
                {
                    Chunk* a = chunkRing.get({ x, y, z });
                    if ((x == 0 || x == loadSide.x - 1) || (z == 0 || z == loadSide.z - 1))
                    {
                        if (a->properMesh)
                            a->properMesh = false, MeshBuilder::disableMesh(vertexpool, a);
                    }
                    else
                    {
                        if (!a->properMesh)
                        {
                            a->properMesh = true, MeshBuilder::disableMesh(vertexpool, chunkRing.get({ x,y,z }));
                            //toUpdate.insert(chunkRing.get({ x,y,z }));
                            rebuildChunk(a);
                        }
                    }
                }

                
    }
    return ivec3(delta2 * int32_t(Chunk::side));
}

ChunkHolder::ChunkHolder(const uint32_t loadSide, vec3 loaderPos, Vertexpool* pool) : ChunkHolder({ loadSide, loadSide, loadSide }, loaderPos, pool)
{

}

ChunkHolder::ChunkHolder(const ivec3 loadSide, vec3 loaderPos, Vertexpool* pool) : loadSide(loadSide), chunkOffset(loaderPos / vec3(Chunk::side)), vertexpool(pool), chunkRing(loadSide)
{
    workFinished = false;
    nextChunkID = 0;

    for (int x = 0; x < loadSide.x; x++)
        for (int y = 0; y < loadSide.y; y++)
            for (int z = 0; z < loadSide.z; z++)
            {
                chunkRing.set({ x,y,z }, loadChunk({ x,y,z }));
            }
    for (int x = 0; x < loadSide.x; x++)
        for (int y = 0; y < loadSide.y; y++)
            for (int z = 0; z < loadSide.z; z++)
            {
                MeshBuilder::buildMesh(vertexpool, chunkRing.get({ x,y,z }), chunkRing);
                chunkRing.get({ x,y,z })->properMesh = true;
                if ((x == 0 || x == loadSide.x - 1) || (z == 0 || z == loadSide.z - 1))
                {
                    chunkRing.get({ x,y,z })->properMesh = false, MeshBuilder::disableMesh(vertexpool, chunkRing.get({ x,y,z }));
                }
            }
    chunkOffset = ivec3(loaderPos) / Chunk::side;
    thread thr(handleChunks, this);
    thr.detach();
}

ivec3 ChunkHolder::getChunkOffset()
{
    return chunkRing.getOffset();
}

void ChunkHolder::finish()
{
    workFinished = true;
}

void ChunkHolder::handleChunks(ChunkHolder* holder)
{
    while (!holder->workFinished)
    {
        if (!holder->toUpdate.empty())
        {
            Chunk* a = *(holder->toUpdate.begin());
            holder->rebuildChunk(a);
            holder->toUpdate.erase(a);
            if (!a->properMesh)
                MeshBuilder::disableMesh(holder->vertexpool, a);
        }
        else
            this_thread::sleep_for(chrono::milliseconds(int(1000.f / 30.f)));
    }
}

Chunk* ChunkHolder::loadChunk(ivec3 pos)
{
    Chunk* chunk = new Chunk((nextChunkID++), pos, pos + chunkOffset * int(Chunk::side));
    WorldGenerator::fillChunk(chunk);
    return chunk;
}

void ChunkHolder::refillChunk(Chunk* chunk)
{
    WorldGenerator::fillChunk(chunk);
}

void ChunkHolder::rebuildChunk(Chunk* chunk)
{
    MeshBuilder::rebuildMesh(vertexpool, chunk, chunkRing);
}

void ChunkHolder::unloadChunk(Chunk* chunk)
{
    MeshBuilder::destroyMesh(vertexpool, chunk);
}

unsigned ChunkHolder::chunkIndex(ivec3 chunkPos)
{
    return chunkPos.x + loadSide.x * chunkPos.y + loadAreaZ * chunkPos.z;
}
