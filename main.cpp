#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include<iostream>
#include<vector>
#include<list>

#define SUBSTEPS 8
#define SUB_COEFF 1/SUBSTEPS/SUBSTEPS
#define CELL_SIZE 4

float radius = 2.0f;
int frame = 0;

struct Ball{
    olc::vf2d pos;
    olc::vf2d pos_prev;
    olc::vf2d acc;
    olc::vi2d pos_grid;
};

std::vector<std::vector<std::list<Ball>>> grid(700/CELL_SIZE, std::vector<std::list<Ball>>(700/CELL_SIZE, std::list<Ball>()));

class Example : public olc::PixelGameEngine
{ 
public:
    std::vector<Ball> balls; 
public:
	Example()
	{
		sAppName = "Example";
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
            for(int row = 0; row < 700/CELL_SIZE; row++){
                for(int cell = 0; cell < 700/CELL_SIZE; cell++){
                    for(auto it = grid[row][cell].begin(); it != grid[row][cell].end(); it++){
                        it->acc.y += 1000;

                        olc::vf2d to = it->pos-olc::vf2d(350.0, 350.0);
                        float dist = to.mag();
                        if(dist > 300-2*radius){
                            it->pos -= to.norm()*(dist-300.0+radius);
                        }

                        olc::vf2d vel = it->pos-it->pos_prev;
                        it->pos_prev = it->pos;
                        it->pos += vel + it->acc*fElapsedTime*fElapsedTime*SUB_COEFF;

                        it->acc.y = 0;
                    }
                }
            }

            for(int row = 1; row < (700/CELL_SIZE)-1; row++){
                for(int cell = 1; cell < (700/CELL_SIZE)-1; cell++){
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
            for(int row = 0; row<(700/CELL_SIZE); row++){
                for(int cell = 0; cell<(700/CELL_SIZE); cell++){
                    for(auto it = grid[row][cell].begin(); it != grid[row][cell].end(); it++){
                        if((int)(it->pos.x/CELL_SIZE) != it->pos_grid.x || (int)(it->pos.y/CELL_SIZE) != it->pos_grid.y){
                            Ball obj = *it;
                            auto del = it;
                            it = std::prev(it);
                            grid[row][cell].erase(del);
                            obj.pos_grid = {(int)(obj.pos.x/CELL_SIZE), (int)(obj.pos.y/CELL_SIZE)};
                            grid[obj.pos_grid.x][obj.pos_grid.y].push_back(obj);
                        }
                    }
                }
            }
        }

        int counter = 0;
        Clear({0, 0, 0});
        //DrawCircle({350, 350}, 300);
        for(int row = 0; row < 700/CELL_SIZE; row++){
            for(int cell = 0; cell < 700/CELL_SIZE; cell++){
                for(auto ball : grid[row][cell]){
                    counter ++;
                    DrawCircle(ball.pos, radius);
                }
            }
        }
        frame ++;

        if(frame % 30 == 0){
            printf("%f FPS @ %d objects\n", 1/fElapsedTime, counter);
            grid[370/CELL_SIZE][80/CELL_SIZE].push_back({{370, 80}, {370, 80}, {0, 0}, {370/CELL_SIZE, 80/CELL_SIZE}});
        }
		return true;
	}
};


int main()
{
	Example demo;
	if (demo.Construct(700, 700, 1, 1))
		demo.Start();
	return 0;
}