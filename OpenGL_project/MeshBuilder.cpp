#include "MeshBuilder.h"


const MeshBuilder::Vertex MeshBuilder::sides[6][6] =
{
    { //right
        {1,  1,  1},
        {1,  1, 0},
        {1, 0, 0},
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
        {1,  1,  1},
        {1, 0,  1},
        {0, 0,  1},

        {0,  1,  1},
        {1,  1,  1},
        {0, 0,  1},
    },
    { //left
        {0,  1,  1},
        {0, 0, 0, },
        {0,  1, 0,},

        {0, 0,  1,},
        {0, 0, 0, },
        {0,  1,  1},
    },
    { //down
        { 1, 0,  1},
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

void MeshBuilder::buildMesh(Vertexpool* vertexpool, Chunk* chunk, Chunk** neighbors)
{
    //cout << chunk->getMeshID() * 6 << "\n";
    int vertex = 0;
    uint8_t* const data = chunk->getData();
    ivec3 offset = chunk->getPosition() * int(chunk->side);


    MeshAttribPack* attrib;
    for (int i = 0; i < 6; i++)
    {
        attrib = new MeshAttribPack({ i, offset.x, offset.y, offset.z });
        vertexpool->startPortionMeshing(chunk->getMeshID() * 6 + i);
        vertexpool->placeAttribs(chunk->getMeshID() * 6 + i, attrib, sizeof(MeshAttribPack));
        delete attrib;
    }
    int* layerInfo = new int[Chunk::area];
    memset(layerInfo, 0, sizeof(int) * Chunk::area);
    for (int s = 0; s < 6; s++) 
    {

        ivec3 advDirections[6] =
        {
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 1},
            { -1, 0, 0},
            {0, -1, 0},
            {0, 0, -1}
        };


        for (int y = 0; y < Chunk::side; y++)
        {
            //std::cout << "layer started ";
            for (int x = 0; x < Chunk::side; x++)
                for (int z = 0; z < Chunk::side; z++)
                {
                    ivec3 sidePositions[6] =
                    {
                        {0, x, z},
                        {x, 0, z},
                        {x, z, 0},
                        {Chunk::side - 1, x, z},
                        {x, Chunk::side - 1, z},
                        {x, z, Chunk::side - 1}
                    };

                    ivec3 directions[6] =
                    {
                        {y, x, z},
                        {x, y, z},
                        {x, z, y},
                        {y, x, z},
                        {x, y, z},
                        {x, z, y},
                    };

                    layerInfo[z * Chunk::side + x] = data[Chunk::dataIndex(directions[s])] > 0;
                    if(layerInfo[z * Chunk::side + x]>0)
                        if (y > 0 && y + 1 < Chunk::side)
                        {
                            if (data[Chunk::dataIndex(advDirections[s]+directions[s])] > 0)
                                layerInfo[z * Chunk::side + x] = -1;
                        }
                        else
                        {
                            if (neighbors[s] == nullptr)
                                layerInfo[z * Chunk::side + x] = -1;
                            else if (neighbors[s]->getData()[Chunk::dataIndex({ sidePositions[s] })] > 0)
                                layerInfo[z * Chunk::side + x] = -1;
                            //else
                                //cout << "NOT BAD ";
                        }
                }


            deque<ivec2> startingVertices;
            startingVertices.push_back({ 0,0 });
            while (!startingVertices.empty())
            {
                ivec2 start = startingVertices.front();
                int type = layerInfo[start.y * Chunk::side + start.x];
                startingVertices.pop_front();
                ivec2 borders = ivec2(Chunk::side);
                for (int i = start.x + 1; i < Chunk::side; i++)
                    if (layerInfo[start.y * Chunk::side + i] != type)
                    {
                        borders.x = i;
                        break;
                    }
                for (int j = start.y; j < Chunk::side; j++)
                    for (int i = start.x; i < borders.x; i++)
                        if (layerInfo[j * Chunk::side + i] != type)
                            borders.y = j, i = borders.x, j = Chunk::side;

                for (int i = start.x; i < borders.x; i++)
                    for (int j = start.y; j < borders.y; j++)
                        layerInfo[j * Chunk::side + i] = -1;


                int dx = borders.x - start.x, dy = borders.y - start.y;

                if (dx * dy > 0)
                {
                    if (type > 0)
                    {
                        Vertex* testV = new Vertex[6];
                        memcpy(testV, sides[s], sizeof(sides[s]));
                        for (int i = 0; i < 6; i++)
                        {
                            ivec3 dv[6]
                            {
                                {y + 1, start.x + dx * testV[i].posY, start.y + dy * testV[i].posZ},
                                {start.x + dx * testV[i].posX, y + 1, start.y + dy * testV[i].posZ},
                                {start.x + dx * testV[i].posX, start.y + dy * testV[i].posY, y + 1},
                                {y, start.x + dx * testV[i].posY, start.y + dy * testV[i].posZ},
                                {start.x + dx * testV[i].posX, y, start.y + dy * testV[i].posZ},
                                {start.x + dx * testV[i].posX, start.y + dy * testV[i].posY, y},
                            };


                            testV[i].posX = dv[s].x;
                            testV[i].posY = dv[s].y;
                            testV[i].posZ = dv[s].z;
                        }
                        vertexpool->placeData(chunk->getMeshID() * 6 + s, testV, sizeof(Vertex) * 6);
                    }
                    if (borders.x < Chunk::side)
                        startingVertices.push_back({ borders.x, start.y });
                    if (borders.y < Chunk::side)
                        startingVertices.push_back({ start.x, borders.y });
                }

            }
        }
    }


    cout << "MESH EDNEDED";

    for (int i = 0; i < 6; i++) 
    {
        vertexpool->endPortionMeshing(chunk->getMeshID() * 6 + i);
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