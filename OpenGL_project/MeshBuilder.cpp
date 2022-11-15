#include "MeshBuilder.h"




const MeshBuilder::Vertex MeshBuilder::sides[6][6] =
{
    { //right
        {1, 0, 0},
        {1,  1, 0},
        {1,  1,  1},


        {1, 0, 0},
        {1, 0,  1},
        {1,  1,  1},
    },
    { //up
        {0,  1, 0},
        {1,  1, 0,},
        {1,  1,  1},
        {1,  1,  1},
        {0,  1,  1},
        {0,  1, 0},
    },
    { //back
        //{1,  1,  1},
        {1, 0, 1},
        {1, 0,  1},
        {0, 0,  1},

        {0, 0,  1},
        {0,  1,  1},
        {1,  1,  1},
    },
    { //left
        //{0, 0, 0, },
        {0, 0, 1},
        {0,  1, 0,},
        {0,  1,  1},

        {0, 0, 0, },
        {0, 0,  1,},
        {0,  1,  1},
    },
    { //down
        { 0, 0,  1},
        {1, 0, 0, },
        {0, 0, 0, },

        {0, 0, 0, },
        {0, 0,  1,},
        {1, 0,  1,},
    },
    { //front
        {0, 0, 0, },
        {1, 0, 0, },
        {1,  1, 0,},

        {1,  1, 0,},
        {0,  1, 0,},
        {0, 0, 0, },
    },
};

const ivec3 MeshBuilder::sideDirections[6] =
{
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
    {-1, 0, 0},
    {0, -1, 0},
    {0, 0, -1},
};

ivec3 normal[6] = { ivec3(1,0,0), ivec3(0,1,0), ivec3(0,0,1), ivec3(-1,0,0), ivec3(0,-1,0), ivec3(0,0,-1) };
ivec3 tangent[6] = { ivec3(0,0,1), ivec3(1,0,0), ivec3(-1,0,0), ivec3(0,0,-1), ivec3(1,0,0), ivec3(1,0,0) };
ivec3 bitangent[6] = { ivec3(0,1,0), ivec3(0,0,1), ivec3(0,1,0), ivec3(0,1,0), ivec3(0,0,-1), ivec3(0,1,0) };

//uint32_t layerInfoAdv[Chunk::area];
//int32_t prevLayerInfo[Chunk::area];
ivec3 up[6] =
{
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
    { -1, 0, 0},
    {0, -1, 0},
    {0, 0, -1}
};

int32_t refZero = 0;
int32_t refSide = Chunk::side-1;


//ivec3 sidePositions[6], directions[6];

uint32_t getBlock(Chunk* chunk, ivec3 pos, RingBuffer3<Chunk*> ring)
{
    ivec3 delta = ivec3(0);
    if (pos.x < 0)
        delta.x--, pos.x += Chunk::side;
    if (pos.y < 0)
        delta.y--, pos.y += Chunk::side;
    if (pos.z < 0)
        delta.z--, pos.z += Chunk::side;
    if (pos.x >= Chunk::side)
        delta.x++, pos.x -= Chunk::side;
    if (pos.y >= Chunk::side)
        delta.y++, pos.y -= Chunk::side;
    if (pos.z >= Chunk::side)
        delta.z++, pos.z -= Chunk::side;

    if (delta != ivec3(0))
        chunk = ring.getAbs(chunk->getPosition() + delta);
    return chunk->getBlock(pos);
}

uint32_t getNextBlock(Chunk* chunk, ivec3 pos, Chunk* neighbors[3][3][3])
{
    ivec3 delta = ivec3(0);
    if (pos.x < 0)
        delta.x--, pos.x += Chunk::side;
    if (pos.y < 0)
        delta.y--, pos.y += Chunk::side;
    if (pos.z < 0)
        delta.z--, pos.z += Chunk::side;
    if (pos.x >= Chunk::side)
        delta.x++, pos.x -= Chunk::side;
    if (pos.y >= Chunk::side)
        delta.y++, pos.y -= Chunk::side;
    if (pos.z >= Chunk::side)
        delta.z++, pos.z -= Chunk::side;

    if (delta != ivec3(0))
        chunk = neighbors[delta.x+1][delta.y+1][delta.z+1];
    return chunk->getBlock(pos);
}


ivec3** calcDirAO()
{
    ivec3** dirAO = new ivec3 * [6];
    for (int s = 0; s < 6; s++)
    {
        dirAO[s] = new ivec3[8];
        vec3 orth = tangent[s];
        for (int k = 0; k < 8; k++)
        {
            dirAO[s][k] = sign(ivec3(orth * 6.f)) + normal[s];
            orth = rotate(orth, radians(45.f), vec3(normal[s]));
        }
    }
    return dirAO;
}

ivec3** dirAO = calcDirAO();


int aoCoeffs[6][8] = { {16, 15, 14, 13, 12, 11, 10, 17 }, 
    {10, 11, 12, 13, 14, 15, 16, 17}, 
    {14, 13, 12, 11, 10, 17, 16, 15}, 
    {16, 17, 10, 11, 12, 13, 14, 15}, 
    {10, 17, 16, 15, 14, 13, 12, 11},
    {10, 11, 12, 13, 14, 15, 16, 17} };


Chunk* neighbors[3][3][3];


ivec3 posCashe;

void MeshBuilder::buildMesh(Vertexpool<CompactVertex, MeshAttribPack>* vertexpool, Chunk* chunk, RingBuffer3<Chunk*> ring, bool restart)
{
    int32_t layerInfo[Chunk::side + 2][Chunk::side + 2];
    for (int x=0; x<3; x++)
        for (int y=0; y<3; y++)
            for (int z = 0; z < 3; z++)
            {
                neighbors[x][y][z] = ring.getAbs(chunk->getPosition() + ivec3(x-1,y-1,z-1));
            }

    int vertex = 0;
    uint8_t* const data = chunk->getData();
    ivec3 ringPos = chunk->getPosition();
    ivec3 offset = chunk->getPosition() * int(chunk->side);
    int gg = chunk->getMeshID();
    MeshAttribPack attrib;
    for (int i = 0; i < 6; i++)
    {
        attrib = { i, offset.x, offset.y, offset.z };
        if(!restart)
            vertexpool->startPortionMeshing(chunk->getMeshID() * 6 + i);
        else
            vertexpool->restartPortionMeshing(chunk->getMeshID() * 6 + i);
        vertexpool->placeAttribs(chunk->getMeshID() * 6 + i, attrib);
    }
    std::vector<CompactVertex> cm(32768);
    for (int s = 0; s < 6; s++)
    {
        cm.clear();
        uint32_t meshingOffset = 0;
        int32_t x = 0, z = 0, height = Chunk::side - 1;
        ivec3Ref nextChunkPositions[6] =
        {
            {&refZero, &x, &z},
            {&x, &refZero, &z},
            {&x, &z, &refZero},
            {&refSide, &x, &z},
            {&x, &refSide, &z},
            {&x, &z, &refSide}
        };
        ivec3Ref positions[6] =
        {
            {&height, &x, &z}, //
            {&x, &height, &z},
            {&x, &z, &height}, //
            {&height, &x, &z}, //
            {&x, &height, &z},
            {&x, &z, &height}, //
        };

        for (; int32_t(height)-1 >= -1; height--)
        {
            for (x = -1; x < Chunk::side+1; x++)
                for (z = -1; z < Chunk::side+1; z++)
                {
                    positions[s].toIvec3(posCashe);
                    if (getNextBlock(chunk, up[s] + posCashe, neighbors) > 0)
                        layerInfo[z+1][x+1] = -1;
                    else
                    {
                        if (x >= 0 && z >= 0 && x + 1 <= Chunk::side && z + 1 <= Chunk::side)
                            layerInfo[z + 1][(x + 1)] = data[Chunk::dataIndex(positions[s])];
                        else
                            layerInfo[(z + 1)][(x + 1)] = getNextBlock(chunk, posCashe, neighbors);
                    }
                }


            for (int x = -1; x < Chunk::side + 1; x ++)
                for (int z = -1; z < Chunk::side + 1; z ++)
                {
                    if (layerInfo[(z + 1)][(x + 1)] == -1)
                    {
                        if (x > -1)
                            if (layerInfo[(z + 1)][(x + 1) - 1] > 0)
                                layerInfo[(z + 1)][(x + 1) - 1] += (1 << aoCoeffs[s][0]);

                        if (x > -1 && z < Chunk::side)
                            if (layerInfo[(z + 1 + 1)][(x + 1) - 1] > 0)
                                layerInfo[(z + 1 + 1)][(x + 1) - 1] += (1 << aoCoeffs[s][1]);

                        if (z < Chunk::side)
                            if (layerInfo[(z + 1 + 1)][(x + 1)] > 0)
                                layerInfo[(z + 1 + 1)][(x + 1)] += (1 << aoCoeffs[s][2]);

                        if (z < Chunk::side && x < Chunk::side)
                            if (layerInfo[(z + 1 + 1)][(x + 1) + 1] > 0)
                                layerInfo[(z + 1 + 1)][(x + 1) + 1] += (1 << aoCoeffs[s][3]);

                        if (x < Chunk::side)
                            if (layerInfo[(z + 1)][(x + 1) + 1] > 0)
                                layerInfo[(z + 1)][(x + 1) + 1] += (1 << aoCoeffs[s][4]);

                        if (z > -1 && x < Chunk::side)
                            if (layerInfo[(z + 1 - 1)][(x + 1) + 1] > 0)
                                layerInfo[(z + 1 - 1)][(x + 1) + 1] += (1 << aoCoeffs[s][5]);

                        if (z > -1)
                            if (layerInfo[(z + 1 - 1)][(x + 1)] > 0)
                                layerInfo[(z + 1 - 1)][(x + 1)] += (1 << aoCoeffs[s][6]);
                        if (z > -1 && x > -1)
                            if (layerInfo[(z + 1 - 1)][(x + 1) - 1] > 0)
                                layerInfo[(z + 1 - 1)][(x + 1) - 1] += (1 << aoCoeffs[s][7]);
                    }
                }
            deque<ivec2> startingVertices;
            startingVertices.push_back({ 0,0 });
            while (!startingVertices.empty())
            {
                ivec2 start = startingVertices.front();
                int type = layerInfo[(start.y+1)][(start.x+1)];
                startingVertices.pop_front();
                if (type != -2)
                {
                    ivec2 borders = ivec2(Chunk::side);
                    for (int i = start.x + 1; i < Chunk::side; i++)
                        if (layerInfo[(start.y+1)][(i+1)] != type)
                        {
                            borders.x = i;
                            break;
                        }
                    for (int j = start.y; j < Chunk::side; j++)
                        for (int i = start.x; i < borders.x; i++)
                            if (layerInfo[(j+1)][(i+1)] != type)
                                borders.y = j, i = borders.x, j = Chunk::side;

                    for (int i = start.x; i < borders.x; i++)
                        for (int j = start.y; j < borders.y; j++)
                            layerInfo[(j+1)][(i+1)] = -2;


                    int dx = borders.x - start.x, dy = borders.y - start.y;

                    if (dx * dy > 0)
                    {
                        if (type % 1024 > 0)
                        {
                            Vertex testV0;
                            memcpy(&testV0, sides[s], sizeof(Vertex));
                            ivec3 dv;
                            switch (s)
                            {
                            case 0:
                                dv = { height + 1, start.x + dx * testV0.posY, start.y + dy * testV0.posZ };
                                break;
                            case 1:
                                dv = { start.x + dx * testV0.posX, height + 1, start.y + dy * testV0.posZ };
                                break;
                            case 2:
                                dv = { start.x + dx * testV0.posX, start.y + dy * testV0.posY, height + 1 };
                                break;
                            case 3:
                                dv = { height, start.x + dx * testV0.posY, start.y + dy * testV0.posZ };
                                break;
                            case 4:
                                dv = { start.x + dx * testV0.posX, height, start.y + dy * testV0.posZ };
                                break;
                            case 5:
                                dv = { start.x + dx * testV0.posX, start.y + dy * testV0.posY, height };
                                break;

                            };
                            testV0.posX = dv.x;
                            testV0.posY = dv.y;
                            testV0.posZ = dv.z;
                            testV0.texID = (type & 1023) - 1;
                            testV0.infoAdv = type >> 10;
                            uint32_t info = testV0.posX + (testV0.posY << 6) + (testV0.posZ << 12) +
                                ((s%3==0? dy:dx) << 18) + ((s % 3 == 0 ? dx : dy) << 24);
                            CompactVertex cv = { info, testV0.texID, testV0.infoAdv };
                            cm[meshingOffset / sizeof(CompactVertex)] = cv;
                            //vertexpool->placeVertex(chunk->getMeshID() * 6 + s, meshingOffset, cv);
                            meshingOffset += sizeof(CompactVertex);
                        }
                        if (borders.x < Chunk::side)
                            startingVertices.push_back({ borders.x, start.y });
                        if (borders.y < Chunk::side)
                            startingVertices.push_back({ start.x, borders.y });
                    }
                }

            }
        }
        //cm.resize(meshingOffset/sizeof(CompactVertex));
        //cout << cm.size() << "\n";
        vertexpool->placeVertices(chunk->getMeshID() * 6 + s, 0, cm, meshingOffset / sizeof(CompactVertex));
    }
    //cout << "MESH EDNEDED\n";
    //cout << "portion meshing ended\n";
}

void MeshBuilder::disableMesh(Vertexpool<CompactVertex, MeshAttribPack>* vertexpool, Chunk* chunk)
{
    for (int i = 0; i < 6; i++)
    {
        vertexpool->disableMesh(chunk->getMeshID() * 6 + i);
    }
}

void MeshBuilder::enableMesh(Vertexpool<CompactVertex, MeshAttribPack>* vertexpool, Chunk* chunk)
{
    for (int i = 0; i < 6; i++)
    {
        vertexpool->enableMesh(chunk->getMeshID() * 6 + i);
    }
}

void MeshBuilder::destroyMesh(Vertexpool<CompactVertex, MeshAttribPack>* vertexpool, Chunk* chunk)
{
    //cout << "unloading\n";
    vertexpool->unmeshPortion(chunk->getMeshID() * 6);
    vertexpool->unmeshPortion(chunk->getMeshID() * 6 + 1);
    vertexpool->unmeshPortion(chunk->getMeshID() * 6 + 2);
    vertexpool->unmeshPortion(chunk->getMeshID() * 6 + 3);
    vertexpool->unmeshPortion(chunk->getMeshID() * 6 + 4);
    vertexpool->unmeshPortion(chunk->getMeshID() * 6 + 5);
}

MeshBuilder::Vertex* MeshBuilder::vertexOffset(int x, int y, int z, Vertex* sample, int vertexCount)
{
    for (int i = 0; i < vertexCount; i++)
    {
        sample[i].posX += x;
        sample[i].posY += y;
        sample[i].posZ += z;
    }
    return sample;
}

mat3 MeshBuilder::rightRotationMatrix(ivec3 axis)
{
    return mat3();
}
