#include "ChunkHolder.h"

ivec3 ChunkHolder::update(vec3 loaderPos)
{
    threadpool.update();
    //toUpdate.insert(chunkRing.get({ 7,0,7 }));
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
                            a->properMesh = true;
                            //toUpdate.insert(chunkRing.get({ x,y,z }));
                            threadpool.addTask({ vertexpool, this, a });
                            //rebuildChunk(a);
                            //MeshBuilder::enableMesh(vertexpool, chunkRing.get({ x,y,z }));
                        }
                    }
                }

                
    }
    return ivec3(delta2 * int32_t(Chunk::side));
}

ChunkHolder::ChunkHolder(const uint32_t loadSide, vec3 loaderPos, Vertexpool<CompactVertex, MeshAttribPack>* pool) : ChunkHolder({ loadSide, loadSide, loadSide }, loaderPos, pool)
{

}

ChunkHolder::ChunkHolder(const ivec3 loadSide, vec3 loaderPos, Vertexpool<CompactVertex, MeshAttribPack>* pool) : loadSide(loadSide), chunkOffset(loaderPos / vec3(Chunk::side)),
vertexpool(pool), chunkRing(loadSide),


threadpool([](updaterArgs args)
    {
        //cout << args.holder->toUpdate.size() << "\n";
        args.holder->rebuildChunk(args.chunk);
        //args.holder->toUpdate.erase(args.chunk);
        if (!args.chunk->properMesh)
            MeshBuilder::disableMesh(args.holder->vertexpool, args.chunk);
        else
            MeshBuilder::enableMesh(args.holder->vertexpool, args.chunk);
    })
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
                MeshBuilder::buildMesh(vertexpool, chunkRing.get({ x,y,z }), chunkRing, false);
                MeshBuilder::enableMesh(vertexpool, chunkRing.get({ x,y,z }));
                chunkRing.get({ x,y,z })->properMesh = true;
                if ((x == 0 || x == loadSide.x - 1) || (z == 0 || z == loadSide.z - 1))
                {
                    chunkRing.get({ x,y,z })->properMesh = false, MeshBuilder::disableMesh(vertexpool, chunkRing.get({ x,y,z }));
                }
            }
    chunkOffset = ivec3(loaderPos) / Chunk::side;
    //thread thr(handleChunks, this);
    //thr.detach();
}

uint32_t ChunkHolder::getBlock(ivec3 pos)
{
    ivec3 delta = ivec3(0);
    while (pos.x < 0)
        delta.x--, pos.x += Chunk::side;
    while (pos.y < 0)
        delta.y--, pos.y += Chunk::side;
    while (pos.z < 0)
        delta.z--, pos.z += Chunk::side;
    while (pos.x >= Chunk::side)
        delta.x++, pos.x -= Chunk::side;
    while (pos.y >= Chunk::side)
        delta.y++, pos.y -= Chunk::side;
    while (pos.z >= Chunk::side)
        delta.z++, pos.z -= Chunk::side;
    return chunkRing.get(delta)->getBlock(pos);
}

ivec3 ChunkHolder::getChunkOffset()
{
    return chunkRing.getOffset();
}

void ChunkHolder::setBlock(ivec3 pos, uint32_t block)
{
    ivec3 delta = ivec3(0);
    while (pos.x < 0)
        delta.x--, pos.x += Chunk::side;
    while (pos.y < 0)
        delta.y--, pos.y += Chunk::side;
    while (pos.z < 0)
        delta.z--, pos.z += Chunk::side;
    while (pos.x >= Chunk::side)
        delta.x++, pos.x -= Chunk::side;
    while (pos.y >= Chunk::side)
        delta.y++, pos.y -= Chunk::side;
    while (pos.z >= Chunk::side)
        delta.z++, pos.z -= Chunk::side;
    //cout << pos.x << " " << pos.y << " " << pos.z << "\n";
    //for(int i=0; i<20; i++)
    chunkRing.get(delta)->setBlock(pos, block);
    //rebuildChunk(chunkRing.get({ 7,7,7 }));
    threadpool.addTask({ vertexpool, this, chunkRing.get(delta)});
}

ivec3 ChunkHolder::getSize()
{
    return loadSide * Chunk::side;
}

void ChunkHolder::finish()
{
    threadpool.quit();
    workFinished = true;
}

Chunk* ChunkHolder::loadChunk(ivec3 pos)
{
    Chunk* chunk = new Chunk((nextChunkID++), pos, pos);
    WorldGenerator::fillChunk(chunk);
    return chunk;
}

void ChunkHolder::refillChunk(Chunk* chunk)
{
    WorldGenerator::fillChunk(chunk);
}

void ChunkHolder::rebuildChunk(Chunk* chunk)
{
    MeshBuilder::buildMesh(vertexpool, chunk, chunkRing, true);
}

void ChunkHolder::unloadChunk(Chunk* chunk)
{
    MeshBuilder::destroyMesh(vertexpool, chunk);
}

unsigned ChunkHolder::chunkIndex(ivec3 chunkPos)
{
    return chunkPos.x + loadSide.x * chunkPos.y + loadAreaZ * chunkPos.z;
}

bool operator < (const ChunkHolder::updaterArgs& b, const ChunkHolder::updaterArgs& a)
{
    return (b.chunk < a.chunk);
}
