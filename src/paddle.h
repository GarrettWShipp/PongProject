#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"

typedef struct {
    int var;
    bool leftPlayer;
} Paddle;

void PaddleStart(AppContext* _app, Entity* _entity) {
    _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);
    _entity->transform.rotation = 0.0f;
    _entity->transform.scale = InitVector3(32.0f, 128.0f, 1.0f);
}

void PaddleUpdate(AppContext* _app, Entity* _entity) {
    const bool* keys = SDL_GetKeyboardState(NULL);
    Vector2 direction = (Vector2){0.0f, 1.0f};
    float speed = 150.0f;

    if(_entity->leftPlayer){
        if(keys[SDL_SCANCODE_W]){
            _entity->velocity = Vec2Mul(direction, speed);
        }
        else if(keys[SDL_SCANCODE_S]){
            _entity->velocity = Vec2Mul(direction, speed * -1);
        }
        else{
            _entity->velocity = Vec2Mul((Vector2){0.0f, 0.0f}, 0.0f);
        }
    }
    else{
        if(keys[SDL_SCANCODE_UP]){
            _entity->velocity = Vec2Mul(direction, speed);
        }
        else if(keys[SDL_SCANCODE_DOWN]){
            _entity->velocity = Vec2Mul(direction, speed * -1);
        }
        else{
            _entity->velocity = Vec2Mul((Vector2){0.0f, 0.0f}, 0.0f);
        }
    }

    if (_entity->transform.position.y - _entity->transform.scale.y * 0.5f <= 0.0f && _entity->velocity.y < 0.0f)
        _entity->velocity.y = 0.0f; 
    
    if (_entity->transform.position.y + _entity->transform.scale.y * 0.5f >= _app->windowHeight && _entity->velocity.y > 0.0f)
        _entity->velocity.y = 0.0f; 

    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);
}

void PaddleDraw(AppContext* _app, Entity* _entity) {
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
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);
    DrawModel(*_entity->model);

    UnBindShader();
}

void PaddleOnDestroy(AppContext* _app, Entity* _entity) {

}