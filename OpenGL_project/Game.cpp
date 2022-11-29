#include "Game.h"

float vertices[] =
{
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

//Chunk chunk(0);


Shader modelShader;

int32_t blockCoolDown = 0;

Game::Game()
{
    shader = Shader("pbrShaderVert.glsl", "pbrShaderGeom.glsl", "pbrShaderFrag.glsl");
    modelShader = Shader("outlineShaderVert.glsl", "outlineShaderFrag.glsl");
    //shader = Shader("pbrShaderVert.glsl", "pbrShaderFrag.glsl");
    glDepthMask(GL_TRUE);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);

    ivec3 cr = { 14, 2, 14 };
    uint32_t meshSizeLimit = 32768;

    ResourseManager::configure(1024, 10, { {3, "albedo"}, {1, "metallic"}, {3, "normal"},
        {1, "roughness"}, {1, "height"}, {1, "AO"} });
    ResourseManager::loadTile("textures/stone");
    ResourseManager::atlasProcess->finishTile();
    ResourseManager::loadTile("textures/grass");
    ResourseManager::atlasProcess->finishTile();
    ResourseManager::loadTile("textures/brick");
    ResourseManager::atlasProcess->finishTile();
    ResourseManager::loadTile("textures/gold");
    ResourseManager::atlasProcess->finishTile();
    cout << "befor load finish\n";
    ResourseManager::finishLoading();

    cout << "before vp\n";
    vertexpool = new Vertexpool<CompactVertex, MeshAttribPack>(6 * cr.x * cr.y * cr.z, meshSizeLimit, { {1, GL_INT, sizeof(int), false}, {1, GL_INT, sizeof(int), false}, {1, GL_INT, sizeof(int), false} }, { {1, GL_INT, sizeof(int), 0}, {3, GL_INT, sizeof(int), 0} });

    modelRenderer = new ModelRenderer<vec3, vec3>(100, { {3, GL_FLOAT, sizeof(float), true } }, { { 3, GL_FLOAT, sizeof(float), true } });


    vector<vec3> vertices =
    {
        {0, 0, 0},
        {1, 0, 0},
        {1, 1, 0},
        {0, 1, 0},
        {0, 0, 1},
        {1, 0, 1},
        {1, 1, 1},
        {0, 1, 1},
    };
    vector<uint32_t> indices = 
    {
        0, 1, 2,  2, 3, 0,
        1, 5, 6,  6, 2, 1,
        5, 4, 7,  7, 6, 5,
        4, 0, 3,  3, 7, 4,
        3, 2, 6,  6, 7, 3,
        4, 5, 1,  1, 0, 4
    };

    vec3 offsetX = vec3(256, 32, 256);

    modelRenderer->addModel(vertices, indices);
    modelRenderer->addModelInstance(0);
    modelRenderer->setInstanceAttribute(0, 0, offsetX);

    cout << "after vp\n";
    skybox = Skybox("textures/skybox");
    camera.translateAbs(cr*Chunk::side/2);
    camera.rotateAbs(0., { 0.,1., 0. });
    model = mat4(1.f);
    gameTime = 0;
    movementInput = vec3(0);
    rotationInput = vec2(0);
    chunkholder = new ChunkHolder(cr, vec3(0), vertexpool);
    vertexpool->update();
}

void Game::render()
{
    gameTime ++;
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    skybox.render(scale(camera.getView(), vec3(-1, -1, 1)), camera.getProjection());

    modelShader.setMat4(camera.getProjection() * camera.getView() * scale(mat4(1.f), vec3(-1, -1, 1)), "transform");
    modelRenderer->setShaderProgram(modelShader);
    shader.setMat4(camera.getProjection()*camera.getView() * scale(mat4(1.f), vec3(-1, -1, 1)), "transform");
    shader.setVec3(camera.getPosition(), "camPos");
    shader.setIvec3(chunkholder->getChunkOffset(), "loadOffset");
    shader.setIvec3(chunkholder->getWorldOffset(), "worldOffset");
    shader.setIvec3(chunkholder->loadSide, "loadSide");
    ivec3 normal[6] = { ivec3(1,0,0), ivec3(0,1,0), ivec3(0,0,1), ivec3(-1,0,0), ivec3(0,-1,0), ivec3(0,0,-1) };
    vector<int> vb(6);
    for (int i = 0; i < 6; i++)
        //cout << glm::angle(camera.getDirection(), vec3(normal[i])) << " ";
        vb[i] = (glm::angle(camera.getDirection(), vec3(normal[i]))>radians(60.f));

    shader.setIntArray(vb, "activeSides");
    shader.setInt(1, "p");
    shader.setFloat((float)ResourseManager::pixelAtlasSize/(float)ResourseManager::tileSize, "tileAtlasSize");
    shader.setInt(0, "albedoMetallicMap");
    shader.setInt(1, "normalRoughnessMap");
    shader.setInt(2, "heightAmbientOcclusionMap");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ResourseManager::atlases[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ResourseManager::atlases[1]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ResourseManager::atlases[2]);

    vertexpool->setShaderProgram(shader);
    vertexpool->update();
    vertexpool->render();

    glClear(GL_DEPTH_BUFFER_BIT);

    modelRenderer->update();
    modelRenderer->render();
}

void Game::update()
{
    ivec3 normal[6] = { ivec3(1,0,0), ivec3(0,1,0), ivec3(0,0,1), ivec3(-1,0,0), ivec3(0,-1,0), ivec3(0,0,-1) };
    ivec4 cube = WorldInteraction::raycast(camera.getPosition()-vec3(chunkholder->getWorldOffset()), camera.getDirection(), *chunkholder);
    vec3 a(vec3(cube)+vec3(chunkholder->getWorldOffset()));
    vec3 t(277, 0, 224);
    blockCoolDown--;
    //cout << mouseButtonInput.x << " " << mouseButtonInput.y << "\n";
    if (blockCoolDown <= 0)
        if (cube.x >= 0)
        {
            //cout << cube.x << " " << cube.y << " " << cube.z << "\n";
                           //for(int i=0; i<32; i++)
            //chunkholder->setBlock(camera.getPosition(), 2);
            if (mouseButtonInput.y == 1)
                chunkholder->setBlock(ivec3(cube) + normal[cube.w], 4);
            if (mouseButtonInput.x == 1)
                chunkholder->setBlock(ivec3(cube), 0);
            blockCoolDown = 1;
        }

    camera.translateAbs(chunkholder->update(camera.getPosition() - glm::vec3(chunkholder->getWorldOffset())));
    modelRenderer->setInstanceAttribute(0, 0, a);
    vec3 normales[6] =
    {
        vec3(1, 0, 0),
        vec3(0, 1, 0),
        vec3(0, 0, 1),
        vec3(-1, 0, 0),
        vec3(0, -1, 0),
        vec3(0, 0, -1)
    };
    if (rotationInput.y != 0)
        camera.rotateRel(rotationInput.y, vec3(1.f, 0.f, 0.f));
    if(rotationInput.x!=0)
       camera.rotateAbs(rotationInput.x, vec3(0.f, 1.f, 0.f)); 
    if (movementInput.length() > 0)
        camera.translateRel(movementInput*2.f);
}

void Game::updateAspectRatio(float ratio)
{
    camera.setAspectRatio(ratio);
}

void Game::setMovementInput(vec3 input)
{
    movementInput = input;
}

void Game::setRotationInput(vec2 input)
{
    rotationInput = input;
}

void Game::addMovementInput(vec3 input)
{
    movementInput += input;
}

void Game::addRotationInput(vec2 input)
{
    rotationInput += input;
}

void Game::addMouseButtonInput(ivec2 input)
{
    mouseButtonInput += input;
}

void Game::terminate()
{
}

