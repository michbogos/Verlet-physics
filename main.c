#include<raylib.h>
#include<raymath.h>
#include"rlights.h"
#include<stdlib.h>

struct Ball{
    Vector3 pos;
    Vector3 pos_prev;
    Vector3 acc;
};

int main(void)
{
    struct Ball balls[1000];
    balls[0].pos = (Vector3){2, 1, 2};
    balls[1].pos = (Vector3){-2, 3, -2};
    int num_balls = 2;
    float RADIUS = 7;
    float DT = 0.1;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(800, 450, "raylib [core] example - basic window");

    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 10.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Shader shader = LoadShader("./basic.vs", "./basic.fs");  

    Model sphere = LoadModelFromMesh(GenMeshSphere(1, 20, 20));
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    sphere.materials[0].shader = shader;

    while (!WindowShouldClose())
    {
        //Bounds
        for(int i = 0; i < num_balls; i++){
            if(Vector3Distance(balls[i].pos, (Vector3){0,0,0})> RADIUS-0.5){
                balls[i].pos = Vector3Scale(balls[i].pos, (RADIUS-0.5)/Vector3Distance(balls[i].pos, (Vector3){0, 0, 0}));
            }
        }
        // Integrate
        for(int i = 0 ; i < num_balls; i++){
            balls[i].acc.y -= 0.01;
            Vector3 v = Vector3Subtract(balls[i].pos, balls[i].pos_prev);
            balls[i].pos_prev = balls[i].pos;
            balls[i].pos = Vector3Add(balls[i].pos, Vector3Add(v, Vector3Scale(balls[i].acc, DT*DT)));
            balls[i].acc = Vector3Zero();
        }

        //Collide
        for(int i = 0; i < num_balls; i++){
            for(int j = 0; j < num_balls; j++){
                if(!Vector3Equals(balls[i].pos, balls[j].pos)){
                    float dist = Vector3Distance(balls[i].pos, balls[j].pos);
                    if(dist < 2.0f){
                        Vector3 n = Vector3Normalize(Vector3Subtract(balls[i].pos, balls[j].pos));
                        float delta = 2-dist;
                        balls[i].pos = Vector3Add(balls[i].pos, Vector3Scale(n, delta*0.5));
                        balls[j].pos = Vector3Subtract(balls[j].pos, Vector3Scale(n, delta*0.5));
                    }
                }
            }
        }

        //Draw
        //UpdateCamera(&camera, CAMERA_ORBITAL);
        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
                DrawGrid(20, 1);
                for(int i = 0; i < num_balls; i++){
                    DrawModel(sphere, balls[i].pos, 1.0, WHITE);
                }
            EndMode3D();
        EndDrawing();

        //Handle Input
        if(IsKeyPressed(KEY_SPACE)){
            balls[num_balls].pos = (Vector3){((float)rand()/(float)(RAND_MAX/10))-5.0f, 0.5f, ((float)rand()/(float)(RAND_MAX/10))-5.0f};
            num_balls += 1;
        }
    }

    UnloadShader(shader);
    UnloadModel(sphere);

    CloseWindow();
    return 0;
}