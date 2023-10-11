#define CAMERA_IMPLEMENTATION
#include<vector>
#include<iostream>
#include<thread>
#include<raylib.h>
#include<raymath.h>
#include<stdio.h>
#include<string.h>
#include"rcamera.h"

struct Ball{
    Vector3 pos;
    Vector3 pos_prev;
    Vector3 acc;
};

int SUBDIV = 20;
float RADIUS = 7;
float SPHERE_RADIUS = 0.25;
float GRID_SIZE = 9;
float ADD_CUBE_RADIUS = 2;
int SUBSTEPS = 8;

std::vector<Ball> balls;
std::vector<std::vector<std::vector<std::vector<Ball*>>>> grid(SUBDIV, std::vector<std::vector<std::vector<Ball*>>>(SUBDIV, std::vector<std::vector<Ball*>>(SUBDIV, std::vector<Ball*>())));
// std::vector<std::vector<int>> full;

void collide(){
    for(int x = 1; x < SUBDIV-1; x++){
            for(int y = 1; y < SUBDIV-1; y++){
                for(int z = 1; z < SUBDIV-1; z++){
                    for(int dx : {0,1,-1}){
                        for(int dy : {0, 1, -1}){
                            for(int dz : {0, 1, -1}){
                                for(int i = 0; i < grid[x][y][z].size(); i++){
                                    for(int j = 0; j < grid[x+dx][y+dy][z+dz].size(); j++){
                                        // Ball balli = grid[x][y][z][i];
                                        // Ball* ballj = grid[x+dx][y+dy][z+dz][j];
                                            if(!Vector3Equals(grid[x][y][z][i]->pos, grid[x+dx][y+dy][z+dz][j]->pos)){
                                                float dist = Vector3Distance(grid[x][y][z][i]->pos, grid[x+dx][y+dy][z+dz][j]->pos);
                                                if(dist < 2*SPHERE_RADIUS){
                                                    Vector3 n = Vector3Normalize(Vector3Subtract(grid[x][y][z][i]->pos, grid[x+dx][y+dy][z+dz][j]->pos));
                                                    float delta = 2*SPHERE_RADIUS-dist;
                                                    grid[x][y][z][i]->pos = Vector3Add(grid[x][y][z][i]->pos, Vector3Scale(n, delta*0.5));
                                                    grid[x+dx][y+dy][z+dz][j]->pos = Vector3Subtract(grid[x+dx][y+dy][z+dz][j]->pos, Vector3Scale(n, delta*0.5));
                                                }
                                            }
                                        // grid[x][y][z][i].pos = grid[x][y][z][i].pos;
                                        // grid[x+dx][y+dy][z+dz][j].pos = grid[x+dx][y+dy][z+dz][j].pos;
                                    }
                                }
                            }
                        }
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
                for(int i = 0; i < balls.size(); i++){
                    DrawModel(sphere, balls[i].pos, SPHERE_RADIUS, WHITE);
                }
            EndMode3D();
            char s[255];
            snprintf(s, 255, "%lu", balls.size());
            DrawText(s, 10, 40, 20, GREEN);
        EndDrawing();


        //Bounds
        for(int i = 0; i < SUBSTEPS; i++){
        for(int i = 0; i < balls.size(); i++){
            if(Vector3Distance(balls[i].pos, (Vector3){0,0,0})> RADIUS-SPHERE_RADIUS){
                balls[i].pos = Vector3Scale(balls[i].pos, (RADIUS-SPHERE_RADIUS)/Vector3Distance(balls[i].pos, (Vector3){0, 0, 0}));
            }
        }
        // Integrate
        float dt = GetFrameTime()/SUBSTEPS;
        for(int i = 0 ; i < balls.size(); i++){
            balls[i].acc.y -= 10;
            Vector3 v = Vector3Subtract(balls[i].pos, balls[i].pos_prev);
            balls[i].pos_prev = balls[i].pos;
            balls[i].pos = Vector3Add(balls[i].pos, Vector3Add(v, Vector3Scale(balls[i].acc,  dt*dt)));
            balls[i].acc = Vector3Zero();
        }


        //Spatial Hash
        for(int i = 0; i < balls.size(); i++){
            float x_adj = balls[i].pos.x + GRID_SIZE;
            float y_adj = balls[i].pos.y + GRID_SIZE;
            float z_adj = balls[i].pos.z + GRID_SIZE;
            int x = (int)((float)x_adj/((float)2*GRID_SIZE/(float)SUBDIV));
            int y = (int)((float)y_adj/((float)2*GRID_SIZE/(float)SUBDIV));
            int z = (int)((float)z_adj/((float)2*GRID_SIZE/(float)SUBDIV));
            grid[std::min(std::max(x, 0), SUBDIV-1)][std::min(std::max(y, 0), SUBDIV-1)][std::min(std::max(z, 0), SUBDIV-1)].push_back(&balls[i]);

            // BeginDrawing();
            //         char buf[255];
            //         sprintf(buf, "x:%d, y:%d, z:%d, posx:%f, posy:%f, posz:%f", x, y, z, x_adj, y_adj, z_adj);
            //         DrawText(buf, 100, 100, 20, BLACK);
            // EndDrawing();
        }

        //Collide
            collide();
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
        }

        for(int i = 0; i < grid.size(); i++){
            for(int j = 0; j < grid.size(); j ++){
                for(int k = 0; k < grid.size(); k++){
                    grid[i][j][k].clear();
                }
            }
        }

        //Handle Input
        if(IsKeyPressed(KEY_SPACE)){
            for(float i = -2.0f; i < 3.0f; i+=0.5f){
                for(float j = -2.0f; j < 3.0f; j+=0.5f){
                    for(float k = -2.0f; k < 3.0f; k+=0.5f){
                        balls.push_back({(Vector3){i, j, k}, (Vector3){i, j, k}, Vector3Zero()});
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