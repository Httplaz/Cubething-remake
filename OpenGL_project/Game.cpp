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

Game::Game()
{
    shader = Shader("pbrShaderVert.glsl", "pbrShaderGeom.glsl", "pbrShaderFrag.glsl");
    //shader = Shader("pbrShaderVert.glsl", "pbrShaderFrag.glsl");
    glDepthMask(GL_TRUE);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    ivec3 cr = { 10, 2, 10 };
    uint32_t meshSizeLimit = 30000;

    ResourseManager::configure(1024, 10, { {3, "albedo"}, {1, "metallic"}, {3, "normal"},
        {1, "roughness"}, {1, "height"}, {1, "AO"} });
    ResourseManager::loadTile("textures/brick");
    ResourseManager::atlasProcess->finishTile();
    ResourseManager::loadTile("textures/blueBrick");
    ResourseManager::atlasProcess->finishTile();
    ResourseManager::loadTile("textures/wood");
    ResourseManager::atlasProcess->finishTile();
    ResourseManager::loadTile("textures/orangeBrick");
    ResourseManager::atlasProcess->finishTile();
    cout << "befor load finish\n";
    ResourseManager::finishLoading();

    cout << "before vp\n";
    vertexpool = new Vertexpool(6 * cr.x * cr.y * cr.z, meshSizeLimit, { {1, GL_INT, sizeof(int), false}, {1, GL_INT, sizeof(int), false}, {1, GL_INT, sizeof(int), false} });
    cout << "after vp\n";
    vertexpool->setPortionAttributes({ {1, GL_INT, sizeof(int), 0}, {3, GL_INT, sizeof(int), 0} });
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
    shader.setMat4(camera.getProjection()*camera.getView() * scale(mat4(1.f), vec3(-1, -1, 1)), "transform");
    shader.setVec3(camera.getPosition(), "camPos");
    shader.setIvec3(chunkholder->getChunkOffset(), "loadOffset");
    shader.setIvec3(chunkholder->loadSide, "loadSide");
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

    vertexpool->setShaderprogram(make_shared<Shader>(shader));
    vertexpool->update();
    vertexpool->render();
}

void Game::update()
{
    camera.translateAbs(chunkholder->update(camera.getPosition()));
    vec3 normales[6] =
    {
        vec3(1, 0, 0),
        vec3(0, 1, 0),
        vec3(0, 0, 1),
        vec3(-1, 0, 0),
        vec3(0, -1, 0),
        vec3(0, 0, -1)
    };
    //vertexpool->cullFaceMask(6, normales, vec3(vec4(0, 0, 1, 0) * scale(camera.getRotation(), vec3(-1, -1, 1))), camera.getView());
    //gameTime += 2;

    if (rotationInput.y != 0)
        camera.rotateRel(rotationInput.y, vec3(1.f, 0.f, 0.f));
    if(rotationInput.x!=0)
       camera.rotateAbs(rotationInput.x, vec3(0.f, 1.f, 0.f)); 
    if (movementInput.length > 0)
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

void Game::terminate()
{
}

