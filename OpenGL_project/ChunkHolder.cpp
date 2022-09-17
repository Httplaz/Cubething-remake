#include "ChunkHolder.h"

ivec3 ChunkHolder::update(vec3 loaderPos)
{
    ivec3 delta = (ivec3(Chunk::side * loadCubeSide / 2) - (ivec3)loaderPos) / int32_t(Chunk::side);
    delta.y = 0;
    if (delta != ivec3(0))
    {
        chunkRing.shift(delta);
        return ivec3(delta*int32_t(Chunk::side));
    }
    else
        return ivec3(0);
    /*ivec3 newOffset = distance(loaderPos, vec3(Chunk::side * loadCubeSide / 2)) > Chunk::side * loadCubeSide / 2 ? ivec3((loaderPos) / float(Chunk::side)) : ivec3(chunkOffset);
    if (newOffset != chunkOffset)
    {
        cout << newOffset.x << " " << newOffset.y << " " << newOffset.z << "\n";
        ivec3 delta = newOffset - chunkOffset;
        chunkRing.shift(-delta);
        chunkOffset = newOffset;
        return -delta * int32_t(Chunk::side);
    }
    return ivec3(0);*/
}

ChunkHolder::ChunkHolder(const unsigned loadCubeSide, vec3 loaderPos, Vertexpool* pool) : loadCubeSide(loadCubeSide), loadCubeArea(loadCubeSide* loadCubeSide), loadCubeVolume(loadCubeArea* loadCubeSide), chunkOffset(loaderPos / vec3(Chunk::side)), vertexpool(pool), chunkBox(prepareChunkArray(), loadCubeSide, loadCubeSide, loadCubeSide), chunkRing(loadCubeSide)
{
    nextChunkID = 0;

    for (int x = 0; x < loadCubeSide; x++)
        for (int y = 0; y < loadCubeSide; y++)
            for (int z = 0; z < loadCubeSide; z++)
            {
                chunkRing.set({ x,y,z }, loadChunk({ x,y,z }));
            }
    //for (auto it = chunkBox.begin(); it != chunkBox.end(); ++it)
        //cout << "";
        //cout << te++ << " ";

    //cout << "ITERATION ENDS\n";
    //{
        //Chunk* neighbors[6] = { it->right->info, it->up->info, it->back->info, it->left->info, it->down->info, it->front->info };
        //MeshBuilder::buildMesh(vertexpool, it->info, neighbors);
    //}

    for (int x = 0; x < loadCubeSide; x++)
        for (int y = 0; y < loadCubeSide; y++)
            for (int z = 0; z < loadCubeSide; z++)
            {
                Chunk* neighbors[6] = { chunkRing.get({x + 1,y,z}), chunkRing.get({x,y + 1,z}), chunkRing.get({x,y,z + 1}), chunkRing.get({x - 1,y,z}), chunkRing.get({x,y - 1,z}), chunkRing.get({x,y,z - 1}) };
                MeshBuilder::buildMesh(vertexpool, chunkRing.get({ x,y,z }), neighbors);
            }



    chunkOffset = uvec3(loaderPos) / Chunk::side;

    //for (int x = 1; x < loadCubeSide - 1; x++)
        //for (int y = 1; y < loadCubeSide - 1; y++)
            //for (int z = 1; z < loadCubeSide - 1; z++)
                //unloadChunk(chunks[chunkIndex({ x,y,z })]);
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
    //int a = nextChunkID;
    //Chunk chunk(a, pos);
    //WorldGenerator::fillChunk(&chunk);
    //MeshBuilder::buildMesh(vertexpool, &chunk);
    Chunk* chunk = new Chunk(nextChunkID++, pos);
    WorldGenerator::fillChunk(chunk);
    return chunk;
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
