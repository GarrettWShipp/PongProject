#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/io.h"

#include <SDL3/SDL.h>

typedef struct {
    Image* noise; 
} Grid;

Entity* SpawnGrid(AppContext* _app, Entity* _entity);
void GridStart(AppContext* _app, Entity* _entity) {
    
}

void GridUpdate(AppContext* _app, Entity* _entity) {
    _entity->velocity.x = 1.0f;
    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);
}

void GridDraw(AppContext* _app, Entity* _entity) {
    ENTITY_DATA(Grid, grid, _entity);
    Matrix4 transform = IdentityMatrix4(); // the order is important
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Rotate(&transform, _entity->transform.rotation * DEG2RAD, InitVector3(0.0f, 0.0f, 1.0f));
    Mat4Scale(&transform, InitVector3(_entity->transform.scale.x, _entity->transform.scale.y, _entity->transform.scale.z));

    BindShader(_entity->shaderId);

    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);

    ShaderSetVector4(_entity->shaderId, "COLOR", _entity->color);
    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    ShaderBindTexture(_entity->shaderId, grid->noise->id, "NOISE_TEXTURE", 1);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);
    DrawModel(*_entity->model);

    UnBindShader();
}

void GridOnDestroy(AppContext* _app, Entity* _entity) {

}

Entity* SpawnGrid(AppContext* _app, Entity* _entity) {
    //void** scene = &(_app->scene);
    Entity* grid = Spawn(&(_app->scene));
    grid->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, -1.0f);
    grid->image = _entity->image;
    grid->model = _entity->model;
    grid->shaderId = _entity->shaderId;
    grid->Start = GridStart;
    grid->Update = GridUpdate;
    grid->Draw = GridDraw;
    grid->OnDestroy = GridOnDestroy;
    return grid;
}