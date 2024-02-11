#include<iostream>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include<iostream>
#include<vector>
#include<list>
#include<thread>
#include<algorithm>
#include<execution>

#define SUBSTEPS 16
#define SUB_COEFF 1/SUBSTEPS/SUBSTEPS
#define CELL_SIZE 50
#define WIDTH 1000
#define HEIGHT 1000

constexpr int radius = 2;
int frame = 0;

struct Ball{
    olc::vf2d pos;
    olc::vf2d pos_prev;
    olc::vf2d acc;
    unsigned long long spatial_lookup;
};

std::vector<Ball> particles;
std::vector<int> start_indices(1, 0);

unsigned long long spatial_hash(olc::vf2d pos, float grid_size){
    return (unsigned long long)((int)(pos.x/grid_size))* 92837111^(unsigned long long)((int)(pos.y/grid_size))*689287499;
}

void collide(){
    for(int i = 0; i < start_indices.size()-1; i++){
        for(int a = 0; a < particles.size();a++){
            for(int b = a; b < particles.size();b++){
                if(particles[a].pos!=particles[b].pos){
                    olc::vf2d axis = particles[a].pos-particles[b].pos;
                    float dist = axis.mag();
                    if(dist < 2*radius){
                        olc::vf2d n = axis.norm();
                        particles[a].pos += 0.5*n*(2*radius-dist);
                        particles[b].pos -= 0.5*n*(2*radius-dist);
                    }
                }
            }
        }
    }
}

// void collideMP(){
//     #pragma omp parallel for
//     for(int row = 1; row < (WIDTH/CELL_SIZE)-1; row++){
//         for(int cell = 1; cell < (HEIGHT/CELL_SIZE)-1; cell++){
//             for(int dx = -1; dx <= 1; dx++){
//                 for(int dy = -1; dy <= 1; dy++){
//                     for(int b1 = 0; b1 < grid[row+dx][cell+dy].size(); b1++){
//                         for(int b2 = 0; b2 < grid[row][cell].size(); b2++){
//                             if(grid[row+dx][cell+dy][b1].pos != grid[row][cell][b2].pos){
//                                 olc::vf2d axis = grid[row+dx][cell+dy][b1].pos-grid[row][cell][b2].pos;
//                                 float dist = axis.mag();
//                                 if(dist < 2*radius){
//                                     olc::vf2d n = axis.norm();
//                                     grid[row+dx][cell+dy][b1].pos += 0.5*n*(2*radius-dist);
//                                     grid[row][cell][b2].pos -= 0.5*n*(2*radius-dist);
//                                 }
//                             }
//                         }
//                     }
//                 }
//             }
//         }
//     }
// }

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
		
        for(int substep = 0; substep < SUBSTEPS; substep++){
            for(int i =0; i < particles.size(); i++){
                        particles[i].acc.y += 1000;
                        if(particles[i].pos.y > HEIGHT-123){
                            particles[i].pos.y = HEIGHT-123;
                        }
                        if(particles[i].pos.y < 123){
                            particles[i].pos.y = 123;
                        }
                        if(particles[i].pos.x < 123){
                            particles[i].pos.x = 123;
                        }
                        if(particles[i].pos.x > WIDTH-123){
                            particles[i].pos.x = WIDTH-123;
                        }

                        olc::vf2d vel = particles[i].pos-particles[i].pos_prev;
                        particles[i].pos_prev = particles[i].pos;
                        particles[i].pos += vel + particles[i].acc*fElapsedTime*fElapsedTime*SUB_COEFF;

                        particles[i].acc.y = 0;
            }

            // for(int row = 1; row < (WIDTH/CELL_SIZE)-1; row++){
            //     for(int cell = 1; cell < (HEIGHT/CELL_SIZE)-1; cell++){
            //         for(int dx = -1; dx <= 1; dx++){
            //             for(int dy = -1; dy <= 1; dy++){
            //                 for(int b1 = 0; b1 < grid[row+dx][cell+dy].size(); b1++){
            //                     for(int b2 = 0; b2 < grid[row][cell].size(); b2++){
            //                         if(grid[row+dx][cell+dy][b1].pos != grid[row][cell][b2].pos){
            //                             olc::vf2d axis = grid[row+dx][cell+dy][b1].pos-grid[row][cell][b2].pos;
            //                             float dist = axis.mag();
            //                             if(dist < 2*radius){
            //                                 olc::vf2d n = axis.norm();
            //                                 grid[row+dx][cell+dy][b1].pos += 0.5*n*(2*radius-dist);
            //                                 grid[row][cell][b2].pos -= 0.5*n*(2*radius-dist);
            //                             }
            //                         }
            //                     }
            //                 }
            //             }
            //         }
            //     }
            // }

        
        frame ++;

        start_indices.clear();
        start_indices.push_back(0);
        for(int i = 1; i < particles.size();i++){
            if(particles[i].spatial_lookup!= particles[i-1].spatial_lookup){
                start_indices.push_back(i);
            }
        }

        start_indices.push_back(particles.size());

        collide();
        
        }

        for(int i = 0; i < particles.size();i++){
            particles[i].spatial_lookup = spatial_hash(particles[i].pos, CELL_SIZE)%particles.size();
        }
        std::sort(std::execution::par_unseq, particles.begin(), particles.end(), [](Ball const &a, Ball const &b) {
        return a.spatial_lookup < b.spatial_lookup; });

        if(frame%30==0){
           particles.push_back({{300, 300}, {299.5, 299.9}, {0, 0}, (spatial_hash({300, 300}, CELL_SIZE)%(particles.size()+1))});
        }
        
        int counter = 0;
        Clear({0, 0, 0});
        //DrawCircle({350, 350}, 300);
                for(int i = 0; i < particles.size(); i++){
                    counter ++;
                    DrawCircle(particles[i].pos, radius);
        }

		return true;
    }
};


int main()
{
    particles.push_back({{300, 300}, {299.5, 299.9}, {0, 0}, (spatial_hash({300, 300}, CELL_SIZE)%(particles.size()+1))});
	Example demo;
	if (demo.Construct(WIDTH, HEIGHT, 1, 1))
		demo.Start();
	return 0;
}