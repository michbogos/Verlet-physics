#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include<iostream>
#include<vector>
#include<list>
#include<thread>

#define SUBSTEPS 8
#define SUB_COEFF 1/SUBSTEPS/SUBSTEPS
#define CELL_SIZE 8
#define WIDTH 1000
#define HEIGHT 1000

constexpr float radius = 2;
float frame = 0;

struct Ball{
    olc::vf2d pos;
    olc::vf2d pos_prev;
    olc::vf2d acc;
    olc::vi2d pos_grid;
};

std::vector<std::vector<std::vector<Ball>>> grid(WIDTH/CELL_SIZE, std::vector<std::vector<Ball>>(HEIGHT/CELL_SIZE, std::vector<Ball>()));

void collide1(){
    for(int row = 1; row < (WIDTH/CELL_SIZE/2); row++){
                for(int cell = 1; cell < (HEIGHT/CELL_SIZE)-1; cell++){
                    for(int dx = -1; dx <= 1; dx++){
                        for(int dy = -1; dy <= 1; dy++){
                            for(auto b1 = grid[row+dx][cell+dy].begin(); b1 != grid[row+dx][cell+dy].end(); b1++){
                                for(auto b2 = grid[row][cell].begin(); b2 != grid[row][cell].end(); b2++){
                                    if(b1->pos != b2->pos){
                                        olc::vf2d axis = b1->pos-b2->pos;
                                        float dist = axis.mag();
                                        if(dist < 2*radius){
                                            olc::vf2d n = axis.norm();
                                            b1->pos += 0.5*n*(2*radius-dist);
                                            b2->pos -= 0.5*n*(2*radius-dist);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
}

void collide2(){
    for(int row = (WIDTH/CELL_SIZE/2); row < (WIDTH/CELL_SIZE)-1; row++){
                for(int cell = 1; cell < (HEIGHT/CELL_SIZE)-1; cell++){
                    for(int dx = -1; dx <= 1; dx++){
                        for(int dy = -1; dy <= 1; dy++){
                            for(auto b1 = grid[row+dx][cell+dy].begin(); b1 != grid[row+dx][cell+dy].end(); b1++){
                                for(auto b2 = grid[row][cell].begin(); b2 != grid[row][cell].end(); b2++){
                                    if(b1->pos != b2->pos){
                                        olc::vf2d axis = b1->pos-b2->pos;
                                        float dist = axis.mag();
                                        if(dist < 2*radius){
                                            olc::vf2d n = axis.norm();
                                            b1->pos += 0.5*n*(2*radius-dist);
                                            b2->pos -= 0.5*n*(2*radius-dist);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
}

void collideMP(){
    #pragma omp parallel for
    for(int row =1; row < (WIDTH/CELL_SIZE)-1; row++){
                for(int cell = 1; cell < (HEIGHT/CELL_SIZE)-1; cell++){
                    for(int dx = -1; dx <= 1; dx++){
                        for(int dy = -1; dy <= 1; dy++){
                            for(auto b1 = grid[row+dx][cell+dy].begin(); b1 != grid[row+dx][cell+dy].end(); b1++){
                                for(auto b2 = grid[row][cell].begin(); b2 != grid[row][cell].end(); b2++){
                                    if(b1->pos != b2->pos){
                                        olc::vf2d axis = b1->pos-b2->pos;
                                        float dist = axis.mag();
                                        if(dist < 2*radius){
                                            olc::vf2d n = axis.norm();
                                            b1->pos += 0.5*n*(2*radius-dist);
                                            b2->pos -= 0.5*n*(2*radius-dist);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
}

class Example : public olc::PixelGameEngine
{ 
public:
public:
	Example()
	{
		sAppName = "Verlet";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame
        if(GetMouse(olc::Mouse::LEFT).bPressed){
            grid[GetMouseX()/CELL_SIZE][GetMouseY()/CELL_SIZE].push_back({GetMousePos(), GetMousePos(), {0, 0}, {GetMouseX()/CELL_SIZE, GetMouseY()/CELL_SIZE}});

        }
		
        for(int substep = 0; substep < SUBSTEPS; substep++){
            for(int row = 0; row < WIDTH/CELL_SIZE; row++){
                for(int cell = 0; cell < HEIGHT/CELL_SIZE; cell++){
                    for(int ball = 0; ball < grid[row][cell].size(); ball++){
                        grid[row][cell][ball].acc.y += 1000;
                        if(grid[row][cell][ball].pos.y > HEIGHT-100){
                            grid[row][cell][ball].pos.y = HEIGHT-100;
                        }
                        if(grid[row][cell][ball].pos.y < 100){
                            grid[row][cell][ball].pos.y = 100;
                        }
                        if(grid[row][cell][ball].pos.x < 100){
                            grid[row][cell][ball].pos.x = 100;
                        }
                        if(grid[row][cell][ball].pos.x > WIDTH-100){
                            grid[row][cell][ball].pos.x = WIDTH-100;
                        }

                        olc::vf2d vel = grid[row][cell][ball].pos-grid[row][cell][ball].pos_prev;
                        grid[row][cell][ball].pos_prev = grid[row][cell][ball].pos;
                        grid[row][cell][ball].pos += vel + grid[row][cell][ball].acc*fElapsedTime*fElapsedTime*SUB_COEFF;

                        grid[row][cell][ball].acc.y = 0;
                    }
                }
            }

            for(int row = 1; row < (WIDTH/CELL_SIZE)-1; row++){
                for(int cell = 1; cell < (HEIGHT/CELL_SIZE)-1; cell++){
                    for(int dx = -1; dx <= 1; dx++){
                        for(int dy = -1; dy <= 1; dy++){
                            for(auto b1 = grid[row+dx][cell+dy].begin(); b1 != grid[row+dx][cell+dy].end(); b1++){
                                for(auto b2 = grid[row][cell].begin(); b2 != grid[row][cell].end(); b2++){
                                    if(b1->pos != b2->pos){
                                        olc::vf2d axis = b1->pos-b2->pos;
                                        float dist = axis.mag();
                                        if(dist < 2*radius){
                                            olc::vf2d n = axis.norm();
                                            b1->pos += 0.5*n*(2*radius-dist);
                                            b2->pos -= 0.5*n*(2*radius-dist);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            for(int row = 0; row<(WIDTH/CELL_SIZE); row++){
                for(int cell = 0; cell<(HEIGHT/CELL_SIZE); cell++){
                    for(int ball = 0; ball < grid[row][cell].size(); ball++){
                        if((int)(grid[row][cell][ball].pos.x/CELL_SIZE) != grid[row][cell][ball].pos_grid.x || (int)(grid[row][cell][ball].pos.y/CELL_SIZE) != grid[row][cell][ball].pos_grid.y){
                            Ball obj = grid[row][cell][ball];
                            grid[row][cell].erase(grid[row][cell].begin()+ball);
                            obj.pos_grid = {(int)(obj.pos.x/CELL_SIZE), (int)(obj.pos.y/CELL_SIZE)};
                            grid[obj.pos_grid.x][obj.pos_grid.y].push_back(obj);
                            ball --;
                        }
                    }
                }
            }
        }

        int counter = 0;
        Clear({0, 0, 0});
        //DrawCircle({350, 350}, 300);
        for(int row = 0; row < WIDTH/CELL_SIZE; row++){
            for(int cell = 0; cell < HEIGHT/CELL_SIZE; cell++){
                for(auto ball : grid[row][cell]){
                    counter ++;
                    DrawCircle(ball.pos, radius);
                }
            }
        }
        frame += fElapsedTime;

        if(frame >= 0.3){
            printf("%f FPS @ %d objects\n", 1/fElapsedTime, counter);
            for(int i = 0; i < 1; i++){
                grid[(WIDTH-100+(2*radius*i))/CELL_SIZE][100/CELL_SIZE].push_back({{WIDTH-100+(2*radius*i), 100}, {(WIDTH-99.5)+(2*radius*i), 99.5}, {0, 0}, {(WIDTH-100+(2*radius*i))/CELL_SIZE, 100/CELL_SIZE}});
            }
        }
		return true;
    }
};


int main()
{
	Example demo;
	if (demo.Construct(WIDTH, HEIGHT, 1, 1))
		demo.Start();
	return 0;
}