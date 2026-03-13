#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/io.h"
#include "cpup/inputmanager.h"
#include <SDL3/SDL.h>

typedef struct {
    int score;
} ScoreBall;

Entity* SpawnScoreBall(AppContext* _app, Entity* _entity, bool _leftPlayer, int _currentScore);

void ScoreBallStart(AppContext* _app, Entity* _entity) {
    ScoreBall* scoreBall = (ScoreBall*)_entity->data;
    _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);

    _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);

    if(_entity->leftPlayer){
        _entity->transform.position = InitVector3(0.0f + scoreBall->score * _entity->transform.scale.x, _app->windowHeight - _entity->transform.scale.y * 0.5f, 0.0f);
    }
    if(!_entity->leftPlayer){
        _entity->transform.position = InitVector3(_app->windowWidth - scoreBall->score * _entity->transform.scale.x, _app->windowHeight - _entity->transform.scale.y, 0.0f);
    }

}

void ScoreBallDraw(AppContext* _app, Entity* _entity) {
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

void ScoreBallOnDestroy(AppContext* _app, Entity* _entity) {

}

Entity* SpawnScoreBall(AppContext* _app, Entity* _entity, bool _leftPlayer, int _score) {
    //void** scene = &(_app->scene);
    Entity* ball = Spawn(&(_app->scene));
    ball->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
    ball->data = calloc(1, sizeof(ScoreBall));
    ball->image = _entity->image;
    ball->model = _entity->model;
    ball->shaderId = _entity->shaderId;
    ball->Start = ScoreBallStart;
    ball->Draw = ScoreBallDraw;
    ball->OnDestroy = ScoreBallOnDestroy;
    ball->leftPlayer = _leftPlayer;
    ScoreBall* scoreBall = (ScoreBall*)ball->data;
    scoreBall->score = _score;
    ball->name = "scoreball";
    return ball;
}