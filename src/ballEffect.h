#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "ballTrail.h"

#include <SDL3/SDL.h>

typedef struct {
    Vector4 color;
} BallEffect;
Entity* SpawnBallEffect(AppContext* _app, Entity* _entity, Vector3 _pos, bool _leftPlayer);

void BallEffectStart(AppContext* _app, Entity* _entity) {

    _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);

}

void BallEffectUpdate(AppContext* _app, Entity* _entity) {
    BallEffect* ballEffect = (BallEffect*)_entity->data;
    i32 startingDirection = rand() % 8;

        static Vector2 directions[8] = {
            (Vector2){0.25f, -1.0f},
            (Vector2){0.5f, -1.0f},
            (Vector2){0.75f, -1.0f},
            (Vector2){-0.25f, -1.0f},
            (Vector2){-0.50f, -1.0f},
            (Vector2){-0.75f, -1.0f}
        };

        _entity->velocity = Vec2Mul(directions[startingDirection], 150.0f);

    // if (_entity->transform.position.x - _entity->transform.scale.x * 0.5f <= 0.0f && _entity->velocity.x < 0.0f)
    //     _entity->velocity.y *= -1.0f; 
    
    // // check if ball is heading above the screen
    // if (_entity->transform.position.x + _entity->transform.scale.x * 0.5f >= _app->windowWidth && _entity->velocity.x > 0.0f)
    //     _entity->velocity.y *= -1.0f;

    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);
    SpawnBallTrail(_app, _entity, _entity->transform.position, ballEffect->color);
    if (_entity->transform.position.y - _entity->transform.scale.y * 0.5f <= 0.0f && _entity->velocity.y < 0.0f){
        Destroy(_app, &(_app->scene), _entity->id);
    }
}

void BallEffectDraw(AppContext* _app, Entity* _entity) {
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

void BallEffectOnDestroy(AppContext* _app, Entity* _entity) {

}

Entity* SpawnBallEffect(AppContext* _app, Entity* _entity, Vector3 _pos, bool _leftPlayer){
    //void** scene = &(_app->scene);
    Entity* ball = Spawn(&(_app->scene));
    ball->transform.position = _pos;
    ball->data = calloc(1, sizeof(BallEffect));
    BallEffect* ballEffect = (BallEffect*)ball->data;
    if(_leftPlayer){
        ball->color = (Vector4){0,0,1,.75f};
        ballEffect->color = (Vector4){0,0,1,.75f};
    }
    else{
        ball->color = (Vector4){1,0,0,.75f};
        ballEffect->color = (Vector4){1,0,0,.75f};
    }
    ball->image = _entity->image;
    ball->model = _entity->model;
    ball->shaderId = _entity->shaderId;
    ball->Start = BallEffectStart;
    ball->Update = BallEffectUpdate;
    ball->Draw = BallEffectDraw;
    ball->OnDestroy = BallEffectOnDestroy;
    return ball;
}