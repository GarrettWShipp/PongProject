#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/io.h"

Entity* SpawnBallTrail(AppContext* _app, Entity* _entity, Vector3 _pos, Vector4 _color);

void TrailBallStart(AppContext* _app, Entity* _entity) {
    _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);
}
void BallTrailUpdate(AppContext* _app, Entity* _entity) {
    Vector3 delta = Vec2ToVec3(Vec2Mul((Vector2){50.0f}, _app->deltaTime));
    _entity->transform.scale = Vec3Sub(_entity->transform.scale, delta);
    if(_entity->transform.scale.x <= 0){
        Destroy(_app, &(_app->scene), _entity->id);
    }
}

void TrailBallDraw(AppContext* _app, Entity* _entity) {
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

void TrailBallOnDestroy(AppContext* _app, Entity* _entity) {

}

Entity* SpawnBallTrail(AppContext* _app, Entity* _entity, Vector3 _pos, Vector4 _color) {
    //void** scene = &(_app->scene);
    Entity* ball = Spawn(&(_app->scene));
    ball->transform.position = _pos;
    ball->color = _color;
    ball->image = _entity->image;
    ball->model = _entity->model;
    ball->shaderId = _entity->shaderId;
    ball->Start = TrailBallStart;
    ball->Update = BallTrailUpdate;
    ball->Draw = TrailBallDraw;
    ball->OnDestroy = TrailBallOnDestroy;
    ball->name = "ballTrail";
    return ball;
}