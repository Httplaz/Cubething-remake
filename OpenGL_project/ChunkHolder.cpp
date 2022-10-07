#include "ChunkHolder.h"

ivec3 ChunkHolder::update(vec3 loaderPos)
{
    ivec3 delta = (ivec3(Chunk::side * loadCubeSide / 2) - (ivec3)loaderPos) / int32_t(Chunk::side);
    delta.y = 0;
    if (delta != ivec3(0))
    {
        chunkOffset -= delta;
        //chunkRing.shift(delta);
        for (int x = 0; x < loadCubeSide; x++)
            for (int y = 0; y < 1; y++)
                for (int z = 0; z < loadCubeSide; z++)
                    replaceChunk({ x, y, z });
        return ivec3(delta*int32_t(Chunk::side));
    }
    else
        return ivec3(0);
}

ChunkHolder::ChunkHolder(const unsigned loadCubeSide, vec3 loaderPos, Vertexpool* pool) : loadCubeSide(loadCubeSide), loadCubeArea(loadCubeSide* loadCubeSide), loadCubeVolume(loadCubeArea* loadCubeSide), chunkOffset(loaderPos / vec3(Chunk::side)), vertexpool(pool), chunkRing(loadCubeSide)
{
    nextChunkID = 0;

    for (int x = 0; x < loadCubeSide; x++)
        for (int y = 0; y < loadCubeSide; y++)
            for (int z = 0; z < loadCubeSide; z++)
            {
                chunkRing.set({ x,y,z }, loadChunk({ x,y,z }));
            }
    for (int x = 0; x < loadCubeSide; x++)
        for (int y = 0; y < loadCubeSide; y++)
            for (int z = 0; z < loadCubeSide; z++)
            {
                Chunk* neighbors[6] = { chunkRing.get({x + 1,y,z}), chunkRing.get({x,y + 1,z}), chunkRing.get({x,y,z + 1}), chunkRing.get({x - 1,y,z}), chunkRing.get({x,y - 1,z}), chunkRing.get({x,y,z - 1}) };
                MeshBuilder::buildMesh(vertexpool, chunkRing.get({ x,y,z }), neighbors);
                //MeshBuilder::destroyMesh(vertexpool, chunkRing.get({ x,y,z }));
                //MeshBuilder::buildMesh(vertexpool, chunkRing.get({ x,y,z }), neighbors);
                //cout << "free portions: " << vertexpool->freePortions.size() << "\n";
                //cout << "meshed portions: " << vertexpool->meshedPortions.size() << "\n";
                //cout << "meshing process: " << vertexpool->meshingProcess.size() << "\n";
            }



    chunkOffset = uvec3(loaderPos) / Chunk::side;
}

ivec3 ChunkHolder::getChunkOffset()
{
    return chunkRing.getOffset();
}

Chunk** ChunkHolder::prepareChunkArray()
{
    Chunk** chunks = new Chunk * [loadCubeVolume];
    cout << loadCubeVolume;
    for (int x = 0; x < loadCubeSide; x++)
        for (int y = 0; y < loadCubeSide; y++)
            for (int z = 0; z < loadCubeSide; z++)
            {
                chunks[chunkIndex({ x,y,z })] = loadChunk({ x,y,z });
            }
    return chunks;
}

Chunk* ChunkHolder::loadChunk(ivec3 pos)
{
    Chunk* chunk = new Chunk(nextChunkID++, pos, pos + chunkOffset * int(Chunk::side));
    WorldGenerator::fillChunk(chunk);
    return chunk;
}

void ChunkHolder::replaceChunk(ivec3 pos)
{
    Chunk* chunk0 = chunkRing.get(pos);
    Chunk* chunk1 = new Chunk(chunk0->getMeshID(), chunk0->getPosition(), chunk0->getPosition() + chunkOffset);
    unloadChunk(chunk0);
    WorldGenerator::fillChunk(chunk1);
    delete chunk0;
    chunkRing.set(pos, chunk1);
    Chunk* neighbors[6] = { chunkRing.get(pos+ivec3(1,0,0)), chunkRing.get(pos+ivec3(0,1,0)), chunkRing.get(pos+ivec3(0,0,1)), chunkRing.get(pos+ivec3(-1, 0, 0)), chunkRing.get(pos + ivec3(0, -1, 0)), chunkRing.get(pos + ivec3(0,0,-1)) };
    MeshBuilder::buildMesh(vertexpool, chunkRing.get(pos), neighbors);
}

void ChunkHolder::integrateChunk(ivec3 pos)
{
    //Chunk* chunk = chunks[chunkIndex(pos)];
    //cout << chunk->getMeshID() << "\n";
    //Chunk* chunk3 = chunks[chunkIndex(pos)];
    //MeshBuilder::destroyMesh(vertexpool, chunk);
    //MeshBuilder::buildMesh(vertexpool, chunk);
}

void ChunkHolder::integrateChunk(Chunk* chunk)
{
    //WorldGenerator::fillChunk(chunk);
    //MeshBuilder::buildMesh(vertexpool, chunk);
}

void ChunkHolder::unloadChunk(Chunk* chunk)
{
    MeshBuilder::destroyMesh(vertexpool, chunk);
}

unsigned ChunkHolder::chunkIndex(ivec3 chunkPos)
{
    return chunkPos.x + loadCubeSide * chunkPos.y + loadCubeArea * chunkPos.z;
}
