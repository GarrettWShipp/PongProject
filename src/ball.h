#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/inputmanager.h"
#include "scoreBall.h"
#include "ballTrail.h"
#include "ballEffect.h"

#include <SDL3/SDL.h>

typedef struct {
    int leftScore;
    int rightScore;
    float timer;
    int winningScore;
} Ball;

Entity* SpawnBall(AppContext* _app, Entity* _entity);
void ResetBall(AppContext* _app, Entity* _entity);
int GetScore(Entity* _ball, bool _leftplayer);
void Winner(AppContext* _app, Entity* _entity, bool _leftPlayer);

void BallStart(AppContext* _app, Entity* _entity) {

    _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);

}

void BallUpdate(AppContext* _app, Entity* _entity) {
    Ball* ball = (Ball*)_entity->data;
    ball->winningScore = 5;
    Transform leftPaddle = Find(&_app->scene, "leftPaddle")->transform;
    Transform rightPaddle = Find(&_app->scene, "rightPaddle")->transform;
    if(_entity->velocity.x > 0){
        _entity->color = (Vector4){0,0,1,0.75f};
        SpawnBallTrail(_app, _entity, _entity->transform.position, (Vector4){0,0,1,0.75f});
    }
    else{
        _entity->color = (Vector4){1,0,0,0.75f};
        SpawnBallTrail(_app, _entity, _entity->transform.position, (Vector4){1,0,0,0.75f});
    }

    if (GetKeyDown(_app, SDL_SCANCODE_P))
    {
        ResetBall(_app, _entity);
    }

    if (Vec2EqualsZero(_entity->velocity) && GetKeyDown(_app, SDL_SCANCODE_SPACE))
    {
        i32 startingDirection = rand() % 4;

        static Vector2 directions[4] = {
            (Vector2){0.72f, 0.72f},
            (Vector2){0.72f, -0.72f},
            (Vector2){-0.72f, 0.72f},
            (Vector2){-0.72f, -0.72f},
        };

        _entity->velocity = Vec2Mul(directions[startingDirection], 150.0f);
    }

    // check if ball is heading below the screen
    if (_entity->transform.position.y - _entity->transform.scale.y * 0.5f <= 0.0f && _entity->velocity.y < 0.0f)
        _entity->velocity.y *= -1.0f; 
    
    // check if ball is heading above the screen
    if (_entity->transform.position.y + _entity->transform.scale.y * 0.5f >= _app->windowHeight && _entity->velocity.y > 0.0f)
        _entity->velocity.y *= -1.0f;

    if (_entity->transform.position.x + _entity->transform.scale.x * 0.5f >= _app->windowWidth){
        ball->leftScore++;
        SpawnScoreBall(_app, _entity, true, ball->leftScore);
        
        ResetBall(_app, _entity);
    }
    if (_entity->transform.position.x - _entity->transform.scale.x * 0.5f <= 0.0f ){
        ball->rightScore++;
        SpawnScoreBall(_app, _entity, false, ball->rightScore);
        
        ResetBall(_app, _entity);
    }
    
    if(ball->rightScore == ball->winningScore){
        _entity->velocity = (Vector2){0.0f};
        ball->timer = ball->timer - _app->deltaTime;
        if(ball->timer <= 0){
            Winner(_app, _entity, false);
            ball->timer = 0.25f;
        }
    }
    if(ball->leftScore == ball->winningScore){
        _entity->velocity = (Vector2){0.0f};
        ball->timer = ball->timer - _app->deltaTime;
        if(ball->timer <= 0){
            Winner(_app, _entity, true);
            ball->timer = 0.25f;
        }
     }
    if(_entity->transform.position.x - _entity->transform.scale.x * 0.5f <= leftPaddle.position.x + leftPaddle.scale.x * 0.5f
        && _entity->transform.position.y - _entity->transform.scale.y * 0.5f <= leftPaddle.position.y + leftPaddle.scale.y * 0.5f
        && _entity->transform.position.y + _entity->transform.scale.y * 0.5f >= leftPaddle.position.y - leftPaddle.scale.y * 0.5f 
        && _entity->velocity.x < 0.0f){
        
        _entity->velocity.x *= -1.0f;
        _entity->velocity = Vec2Mul(_entity->velocity, 1.1);
    }

    if(_entity->transform.position.x + _entity->transform.scale.x * 0.5f >= rightPaddle.position.x - rightPaddle.scale.x * 0.5f
        && _entity->transform.position.y - _entity->transform.scale.y * 0.5f <= rightPaddle.position.y + rightPaddle.scale.y * 0.5f 
        && _entity->transform.position.y + _entity->transform.scale.y * 0.5f >= rightPaddle.position.y - rightPaddle.scale.y * 0.5f 
        && _entity->velocity.x > 0.0f){

        _entity->velocity.x *= -1.0f;
        _entity->velocity = Vec2Mul(_entity->velocity, 1.1);
    }

    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);
}

void BallDraw(AppContext* _app, Entity* _entity) {
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

void BallOnDestroy(AppContext* _app, Entity* _entity) {

}

Entity* SpawnBall(AppContext* _app, Entity* _entity) {
    //void** scene = &(_app->scene);
    Entity* ball = Spawn(&(_app->scene));
    ball->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
    ball->data = calloc(1, sizeof(Ball));
    ball->image = _entity->image;
    ball->model = _entity->model;
    ball->shaderId = _entity->shaderId;
    ball->Start = BallStart;
    ball->Update = BallUpdate;
    ball->Draw = BallDraw;
    ball->OnDestroy = BallOnDestroy;
    return ball;
}

void ResetBall(AppContext* _app, Entity* _entity){
    _entity->velocity = (Vector2){0.0f};
    _entity->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);

}
int GetScore(Entity* _ball, bool _leftplayer){
    Ball* ball = (Ball*)_ball->data;
    if(_leftplayer){
        return ball->leftScore;
    }
    return ball->rightScore;

}

void Winner(AppContext* _app, Entity* _entity, bool _leftPlayer){
        for(int x = 0; x < _app->windowWidth; x += (_entity->transform.scale.x)){
        SpawnBallEffect(_app, _entity, (Vector3){x, _app->windowHeight - 5, 0.0f}, _leftPlayer);
    }
    
}