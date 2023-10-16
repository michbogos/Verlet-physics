#define CAMERA_IMPLEMENTATION
#define MAX_NUM  100000
#define INF 2000000000
#include<iostream>
#include<thread>
#include<raylib.h>
#include<algorithm>
#include<raymath.h>
#include<stdio.h>
#include<map>
#include<string.h>
#include"rcamera.h"

#define SUBDIV 40

struct Ball{
    Vector3 pos;
    Vector3 pos_prev;
    Vector3 acc;
};

float RADIUS = 7;
float SPHERE_RADIUS = 0.25;
float GRID_SIZE =100;
float ADD_CUBE_RADIUS = 2;
int SUBSTEPS = 1;
int NUM_BALLS = 1;

Ball balls[MAX_NUM];
int ids[MAX_NUM];
int start[MAX_NUM];
int aux[MAX_NUM];

int hash(Vector3 vec){
    return abs((((int)(vec.x*GRID_SIZE)*(73856093))^((int)(vec.y*GRID_SIZE)*(19349663))^((int)(vec.z*GRID_SIZE)*(83492791)))%NUM_BALLS);
}

void sort(){
    for(int i = 0; i < NUM_BALLS; i++){
        aux[ids[i]]++;
    }
    for(int i = 0; i < NUM_BALLS; i++){
        if(aux[i]){
            while(aux[i]--){
                ids[i] = i;
            }
            }
        }
    }


void collide(int start, int end){
    for(int i = start; i <= end; i++){
        for(int j = start; j < end; j++){
            Vector3 posi = balls[i].pos;
            Vector3 posj = balls[j].pos;
                if(!Vector3Equals(posj, posi)){
                    float dist = Vector3Distance(posj, posi);
                    if(dist < 2*SPHERE_RADIUS){
                        Vector3 n = Vector3Normalize(Vector3Subtract(posi, posj));
                        float delta = 2*SPHERE_RADIUS-dist;
                        balls[i].pos = Vector3Add(posi, Vector3Scale(n, delta*0.5));
                        balls[j].pos = Vector3Subtract(posj, Vector3Scale(n, delta*0.5));
                    }
                }
        }
    }
}

int main(void)
{
    // SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1200, 900, "raylib [core] example - basic window");

    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 20.0f, 20.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Shader shader = LoadShader("./basic.vs", "./basic.fs");  

    Model sphere = LoadModelFromMesh(GenMeshSphere(1, 5, 5));
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    sphere.materials[0].shader = shader;
    float timer = 0;

    // Image tex = LoadImage(const char *fileName);   

    while (!WindowShouldClose())
    {
        timer += GetFrameTime();
        //UpdateCamera(&camera, CAMERA_ORBITAL);
        
        //Draw
        //UpdateCamera(&camera, CAMERA_ORBITAL);
        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawFPS(10, 10); 
            BeginMode3D(camera);
                //DrawSphereWires(Vector3Zero(), RADIUS, 50, 50, (Color){200, 200, 200, 100});
                for(int i = 0; i < NUM_BALLS; i++){
                    DrawModel(sphere, balls[i].pos, SPHERE_RADIUS, WHITE);
                }
            EndMode3D();
            char s[255];
            snprintf(s, 255, "%d", NUM_BALLS);
            DrawText(s, 10, 40, 20, GREEN);
        EndDrawing();


        //Bounds
        for(int i = 0; i < SUBSTEPS; i++){
        for(int i = 0; i < NUM_BALLS; i++){
            if(Vector3Length(balls[i].pos)> RADIUS-SPHERE_RADIUS){
                balls[i].pos = Vector3Scale(balls[i].pos, (RADIUS-SPHERE_RADIUS)/Vector3Length(balls[i].pos));
            }
        }
        // Integrate
        float dt = GetFrameTime()/SUBSTEPS;
        for(int i = 0 ; i < NUM_BALLS; i++){
            balls[i].acc.y -= 10;
            Vector3 v = Vector3Subtract(balls[i].pos, balls[i].pos_prev);
            balls[i].pos_prev = balls[i].pos;
            balls[i].pos = Vector3Add(balls[i].pos, Vector3Add(v, Vector3Scale(balls[i].acc,  dt*dt)));
            balls[i].acc = Vector3Zero();
        }


        //Spatial Hash
        for(int i = 0; i < NUM_BALLS; i++){
            ids[i] = hash(balls[i].pos);
        }

        sort();

        for(int i = 0; i < NUM_BALLS; i++){
            int el = ids[i];
            int s = i;
            while(ids[i] != el || i < NUM_BALLS-1){
                i++;
            }
            start[i] = s;
        }

        //Collide
        for(int i = NUM_BALLS-1; i > 0;){
            collide(start[i], i);
            i = start[i];
        }
            // for(int i = 0; i < balls.size(); i++){
            //     for(int j = 0; j < balls.size(); j++){
            //         if(!Vector3Equals(balls[i].pos, balls[j].pos)){
            //             float dist = Vector3Distance(balls[i].pos, balls[j].pos);
            //             if(dist < 2*SPHERE_RADIUS){
            //                 Vector3 n = Vector3Normalize(Vector3Subtract(balls[i].pos, balls[j].pos));
            //                 float delta = 2*SPHERE_RADIUS-dist;
            //                 balls[i].pos = Vector3Add(balls[i].pos, Vector3Scale(n, delta*0.5));
            //                 balls[j].pos = Vector3Subtract(balls[j].pos, Vector3Scale(n, delta*0.5));
            //             }
            //         }
            //     }
            // }

            //clear vector
            // for(int i = 0; i < grid.size(); i++){
            //     for(int j = 0; j < grid.size(); j ++){
            //         for(int k = 0; k < grid.size(); k++){
            //             grid[i][j][k].clear();
            //         }
            //     }
            // }
        }

        //Handle Input
        if(IsKeyPressed(KEY_SPACE)){
            balls[NUM_BALLS] = {(Vector3){1, -1 ,1}, (Vector3){0, 0, 0}, Vector3Zero()};
            NUM_BALLS ++;
            for(float i = -2.0f; i < 3.0f; i+=0.5f){
                for(float j = -2.0f; j < 3.0f; j+=0.5f){
                    for(float k = -2.0f; k < 3.0f; k+=0.5f){
                        balls[NUM_BALLS] = {(Vector3){i, j, k}, (Vector3){i, j, k}, Vector3Zero()};
                        NUM_BALLS ++;
                    }
                }
            }
        }

        if(IsKeyDown(KEY_W)){
            camera.position.z -= 0.1;
        }
        if(IsKeyDown(KEY_A)){
            camera.position.x -= 0.1;
        }
        if(IsKeyDown(KEY_S)){
            camera.position.z += 0.1;
        }
        if(IsKeyDown(KEY_D)){
            camera.position.x += 0.1;
        }
    }

    UnloadShader(shader);
    UnloadModel(sphere);

    CloseWindow();
    return 0;
}