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
    shader = Shader("pbrShaderVert.glsl", "pbrShaderFrag.glsl");
    glDepthMask(GL_TRUE);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    albedo = Texture("textures/texture.png", true);
    albedo = Texture("textures/brick/albedo.png", true);
    normal = Texture("textures/brick/normal.png", true);
    metallic = Texture("textures/brick/metallic.png", true);
    roughness = Texture("textures/brick/roughness.png", true);
    ambientOccluison = Texture("textures/brick/AO.png", true);
    vertexpool = new Vertexpool(162, 132000/2, { {3, GL_INT, sizeof(int)}});
    vertexpool->setPortionAttributes({ {1, GL_INT, sizeof(int)}, {3, GL_INT, sizeof(int)} });
    skybox = Skybox("textures/skybox");
    camera.translateAbs(glm::vec3(0, 0, -3));
    model = mat4(1.f);
    gameTime = 0;
    movementInput = vec3(0);
    rotationInput = vec2(0);
    chunkholder = new ChunkHolder(3, vec3(0), vertexpool);
    vertexpool->update();
    //WorldGenerator::fillChunk(&chunk);
    //MeshBuilder::buildMesh(vertexpool, &chunk);
    //vertexpool->update();

}

void Game::render()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    skybox.render(scale(camera.getView(), vec3(-1, -1, 1)), camera.getProjection());
    shader.setMat4(scale(mat4(1.f), vec3(-1,-1,1)), "view");
    shader.setMat4(mat4(1.f), "model");
    shader.setMat4(camera.getProjection()*camera.getView(), "projection");
    shader.setVec3(camera.getPosition(), "camPos");
    shader.setIvec4(ivec4(chunkholder->getChunkOffset(), chunkholder->loadCubeSide), "offsetInfo");
    shader.setInt(0, "albedoMap");
    shader.setInt(1, "normalMap");
    shader.setInt(2, "metallicMap");
    shader.setInt(3, "roughnessMap");
    shader.setInt(4, "AOMap");

    vertexpool->setShaderprogram(make_shared<Shader>(shader));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedo.getTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normal.getTexture());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, metallic.getTexture());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, roughness.getTexture());
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, ambientOccluison.getTexture());
    //vertexpool->update();
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
    gameTime += 2;

    if (rotationInput.y != 0)
        camera.rotateRel(rotationInput.y, vec3(1.f, 0.f, 0.f));
    if(rotationInput.x!=0)
       camera.rotateAbs(rotationInput.x, vec3(0.f, 1.f, 0.f)); 
    if (movementInput.length > 0)
        camera.translateRel(movementInput*0.4f);
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

