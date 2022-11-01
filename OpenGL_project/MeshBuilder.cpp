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

uint32_t layerInfo[Chunk::area];
//uint32_t layerInfoAdv[Chunk::area];
uint32_t prevLayerInfo[Chunk::area];
ivec3 up[6] =
{
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
    { -1, 0, 0},
    {0, -1, 0},
    {0, 0, -1}
};

uint32_t refZero = 0;
uint32_t refSide = Chunk::side-1;


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


void MeshBuilder::buildMesh(Vertexpool* vertexpool, Chunk* chunk, RingBuffer3<Chunk*> ring)
{
    int vertex = 0;
    uint8_t* const data = chunk->getData();
    ivec3 ringPos = chunk->getPosition();
    //cout << "a: " << chunkOffset.x << " " << chunkOffset.y << " " << chunkOffset.z << "\n";
    //chunkOffset = chunk->getPosition();
    //cout << "b: " << chunkOffset.x << " " << chunkOffset.y << " " << chunkOffset.z << "\n";
    ivec3 offset = chunk->getPosition() * int(chunk->side);
    //cout << chunk->getPosition().x << " " << chunk->getPosition().y << " " << chunk->getPosition().z << ": mesh started\n";
    int gg = chunk->getMeshID();
    //cout << gg << "\n";
    MeshAttribPack* attrib;
    for (int i = 0; i < 6; i++)
    {
        attrib = new MeshAttribPack({ i, offset.x, offset.y, offset.z });
        vertexpool->startPortionMeshing(chunk->getMeshID() * 6 + i);
        vertexpool->placeAttribs(chunk->getMeshID() * 6 + i, attrib, sizeof(MeshAttribPack));
        delete attrib;
    }
    for (int s = 0; s < 6; s++)
    {
        uint32_t x = 0, z = 0, height = Chunk::side - 1;
        uvec3Ref nextChunkPositions[6] =
        {
            {&refZero, &x, &z},
            {&x, &refZero, &z},
            {&x, &z, &refZero},
            {&refSide, &x, &z},
            {&x, &refSide, &z},
            {&x, &z, &refSide}
        };
        uvec3Ref positions[6] =
        {
            {&height, &x, &z}, //
            {&x, &height, &z},
            {&x, &z, &height}, //
            {&height, &x, &z}, //
            {&x, &height, &z},
            {&x, &z, &height}, //
        };

        for (x = 0; x < Chunk::side; x++)
            for (z = 0; z < Chunk::side; z++)
                prevLayerInfo[z * Chunk::side + x] = ring.getAbs(ringPos+up[s])->getData()[Chunk::dataIndex({ nextChunkPositions[s] })];


        for (; int32_t(height)-1 >= -1; height--)
        {
            //cout << height << "\n";
            for(x=0; x<Chunk::side; x++)
                for (z=0; z < Chunk::side; z++)
                {
                    //cout << x << " " << height << " " << z << "\n";
                    if(prevLayerInfo[z*Chunk::side+x]>0)
                        layerInfo[z * Chunk::side + x] = -1;
                    layerInfo[z * Chunk::side + x] = data[Chunk::dataIndex(positions[s])];
                    if (layerInfo[z * Chunk::side + x] > 0)
                    {
                        if (getBlock(chunk, up[s] + positions[s].toIvec3(), ring) > 0)
                            layerInfo[z * Chunk::side + x] = -1;
                    }
                        /*if (height != (s < 3 ? Chunk::side - 1 : 0))
                        {
                            if (data[Chunk::dataIndex(up[s] + positions[s].toIvec3())] > 0)
                                layerInfo[z * Chunk::side + x] = -1;
                        }
                        else
                        {
                            if (ring.getAbs(ringPos + up[s]) == nullptr)
                                layerInfo[z * Chunk::side + x] = -1;
                            else if (ring.getAbs(ringPos + up[s])->getData()[Chunk::dataIndex({ nextChunkPositions[s] })] > 0)
                                layerInfo[z * Chunk::side + x] = -1;
                        }*/
                    if (layerInfo[z * Chunk::side + x] != -1)
                    {
                        uint8_t neighbors = 0;
                        for (int k = 0; k < 8; k++)
                        {
                            if (getBlock(chunk, positions[s].toIvec3()+dirAO[s][k], ring) > 0)
                                neighbors+=1<<k;
                        }
                        layerInfo[z * Chunk::side + x] += neighbors * 1000;
                    }
                }
            deque<ivec2> startingVertices;
            startingVertices.push_back({ 0,0 });
            while (!startingVertices.empty())
            {
                ivec2 start = startingVertices.front();
                int type = layerInfo[start.y * Chunk::side + start.x];
                //int infoAdv = layerInfoAdv[start.y * Chunk::side + start.x];
                startingVertices.pop_front();
                if (type != -2)
                {
                    ivec2 borders = ivec2(Chunk::side);
                    for (int i = start.x + 1; i < Chunk::side; i++)
                        if (layerInfo[start.y * Chunk::side + i] != type)// || layerInfoAdv[start.y * Chunk::side + i] != infoAdv)
                        {
                            borders.x = i;
                            break;
                        }
                    for (int j = start.y; j < Chunk::side; j++)
                        for (int i = start.x; i < borders.x; i++)
                            if (layerInfo[j * Chunk::side + i] != type)// || layerInfoAdv[start.y * Chunk::side + i] != infoAdv)
                                borders.y = j, i = borders.x, j = Chunk::side;

                    for (int i = start.x; i < borders.x; i++)
                        for (int j = start.y; j < borders.y; j++)
                            layerInfo[j * Chunk::side + i] = -2;


                    int dx = borders.x - start.x, dy = borders.y - start.y;

                    if (dx * dy > 0)
                    {
                        if (type % 1000 > 0)
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
                            testV0.texID = type % 1000 - 1;
                            testV0.infoAdv = type / 1000;

                            ivec2 ll[6]
                            {
                                {dy, dx},
                                {dx,dy},
                                {dx, dy},
                                {dy, dx},
                                {dx,dy},
                                {dx, dy},
                            };

                            uint32_t info = testV0.posX + (testV0.posY << 6) + (testV0.posZ << 12) +
                                (ll[s].x << 18) + (ll[s].y << 24);
                            CompactVertex cv = { info, testV0.texID, testV0.infoAdv };
                            vertexpool->placeData(chunk->getMeshID() * 6 + s, &cv, sizeof(CompactVertex));
                        }
                        if (borders.x < Chunk::side)
                            startingVertices.push_back({ borders.x, start.y });
                        if (borders.y < Chunk::side)
                            startingVertices.push_back({ start.x, borders.y });
                    }
                }

            }
            //memcpy(prevLayerInfo, layerInfo, sizeof(layerInfo));
            //cout << "height enden " << height << "\n";
        }
    }
    //cout << "MESH EDNEDED\n";

    for (int i = 0; i < 6; i++) 
    {
        vertexpool->endPortionMeshing(chunk->getMeshID() * 6 + i);
    }
    //cout << "portion meshing ended\n";
}

void MeshBuilder::rebuildMesh(Vertexpool* vertexpool, Chunk* chunk, RingBuffer3<Chunk*> ring)
{
    int vertex = 0;
    uint8_t* const data = chunk->getData();
    ivec3 ringPos = chunk->getPosition();
    //cout << "a: " << chunkOffset.x << " " << chunkOffset.y << " " << chunkOffset.z << "\n";
    //chunkOffset = chunk->getPosition();
    //cout << "b: " << chunkOffset.x << " " << chunkOffset.y << " " << chunkOffset.z << "\n";
    ivec3 offset = chunk->getPosition() * int(chunk->side);
    //cout << chunk->getPosition().x << " " << chunk->getPosition().y << " " << chunk->getPosition().z << ": mesh started\n";
    int gg = chunk->getMeshID();
    //cout << gg << "\n";
    MeshAttribPack* attrib;
    for (int i = 0; i < 6; i++)
    {
        attrib = new MeshAttribPack({ i, offset.x, offset.y, offset.z });
        vertexpool->restartPortionMeshing(chunk->getMeshID() * 6 + i);
        vertexpool->placeAttribs(chunk->getMeshID() * 6 + i, attrib, sizeof(MeshAttribPack));
        delete attrib;
    }
    for (int s = 0; s < 6; s++)
    {
        uint32_t x = 0, z = 0, height = Chunk::side - 1;
        uvec3Ref nextChunkPositions[6] =
        {
            {&refZero, &x, &z},
            {&x, &refZero, &z},
            {&x, &z, &refZero},
            {&refSide, &x, &z},
            {&x, &refSide, &z},
            {&x, &z, &refSide}
        };
        uvec3Ref positions[6] =
        {
            {&height, &x, &z}, //
            {&x, &height, &z},
            {&x, &z, &height}, //
            {&height, &x, &z}, //
            {&x, &height, &z},
            {&x, &z, &height}, //
        };

        for (x = 0; x < Chunk::side; x++)
            for (z = 0; z < Chunk::side; z++)
                prevLayerInfo[z * Chunk::side + x] = ring.getAbs(ringPos + up[s])->getData()[Chunk::dataIndex({ nextChunkPositions[s] })];


        for (; int32_t(height) - 1 >= -1; height--)
        {
            //cout << height << "\n";
            for (x = 0; x < Chunk::side; x++)
                for (z = 0; z < Chunk::side; z++)
                {
                    //cout << x << " " << height << " " << z << "\n";
                    if (prevLayerInfo[z * Chunk::side + x] > 0)
                        layerInfo[z * Chunk::side + x] = -1;
                    layerInfo[z * Chunk::side + x] = data[Chunk::dataIndex(positions[s])];
                    if (layerInfo[z * Chunk::side + x] > 0)
                    {
                        if (getBlock(chunk, up[s] + positions[s].toIvec3(), ring) > 0)
                            layerInfo[z * Chunk::side + x] = -1;
                    }
                    /*if (height != (s < 3 ? Chunk::side - 1 : 0))
                    {
                        if (data[Chunk::dataIndex(up[s] + positions[s].toIvec3())] > 0)
                            layerInfo[z * Chunk::side + x] = -1;
                    }
                    else
                    {
                        if (ring.getAbs(ringPos + up[s]) == nullptr)
                            layerInfo[z * Chunk::side + x] = -1;
                        else if (ring.getAbs(ringPos + up[s])->getData()[Chunk::dataIndex({ nextChunkPositions[s] })] > 0)
                            layerInfo[z * Chunk::side + x] = -1;
                    }*/
                    if (layerInfo[z * Chunk::side + x] != -1)
                    {
                        uint8_t neighbors = 0;
                        for (int k = 0; k < 8; k++)
                        {
                            if (getBlock(chunk, positions[s].toIvec3() + dirAO[s][k], ring) > 0)
                                neighbors += 1 << k;
                        }
                        layerInfo[z * Chunk::side + x] += neighbors * 1000;
                    }
                }
            deque<ivec2> startingVertices;
            startingVertices.push_back({ 0,0 });
            while (!startingVertices.empty())
            {
                ivec2 start = startingVertices.front();
                int type = layerInfo[start.y * Chunk::side + start.x];
                //int infoAdv = layerInfoAdv[start.y * Chunk::side + start.x];
                startingVertices.pop_front();
                if (type != -2)
                {
                    ivec2 borders = ivec2(Chunk::side);
                    for (int i = start.x + 1; i < Chunk::side; i++)
                        if (layerInfo[start.y * Chunk::side + i] != type)// || layerInfoAdv[start.y * Chunk::side + i] != infoAdv)
                        {
                            borders.x = i;
                            break;
                        }
                    for (int j = start.y; j < Chunk::side; j++)
                        for (int i = start.x; i < borders.x; i++)
                            if (layerInfo[j * Chunk::side + i] != type)// || layerInfoAdv[start.y * Chunk::side + i] != infoAdv)
                                borders.y = j, i = borders.x, j = Chunk::side;

                    for (int i = start.x; i < borders.x; i++)
                        for (int j = start.y; j < borders.y; j++)
                            layerInfo[j * Chunk::side + i] = -2;


                    int dx = borders.x - start.x, dy = borders.y - start.y;

                    if (dx * dy > 0)
                    {
                        if (type % 1000 > 0)
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
                            testV0.texID = type % 1000 - 1;
                            testV0.infoAdv = type / 1000;

                            ivec2 ll[6]
                            {
                                {dy, dx},
                                {dx,dy},
                                {dx, dy},
                                {dy, dx},
                                {dx,dy},
                                {dx, dy},
                            };

                            uint32_t info = testV0.posX + (testV0.posY << 6) + (testV0.posZ << 12) +
                                (ll[s].x << 18) + (ll[s].y << 24);
                            CompactVertex cv = { info, testV0.texID, testV0.infoAdv };
                            vertexpool->placeData(chunk->getMeshID() * 6 + s, &cv, sizeof(CompactVertex));
                        }
                        if (borders.x < Chunk::side)
                            startingVertices.push_back({ borders.x, start.y });
                        if (borders.y < Chunk::side)
                            startingVertices.push_back({ start.x, borders.y });
                    }
                }

            }
            //memcpy(prevLayerInfo, layerInfo, sizeof(layerInfo));
            //cout << "height enden " << height << "\n";
        }
    }
    //cout << "MESH EDNEDED\n";

    for (int i = 0; i < 6; i++)
    {
        vertexpool->endPortionMeshing(chunk->getMeshID() * 6 + i);
    }
    //cout << "portion meshing ended\n";
}

void MeshBuilder::disableMesh(Vertexpool* vertexpool, Chunk* chunk)
{
    for (int i = 0; i < 6; i++)
    {
        vertexpool->disableMesh(chunk->getMeshID() * 6 + i);
    }
}

void MeshBuilder::destroyMesh(Vertexpool* vertexpool, Chunk* chunk)
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
