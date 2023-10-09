run: main.cpp
	g++ -o verlet main.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 -Ofast

3d: 3d.cpp
	g++ 3d.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o verlet -Ofast

clean:
	rm -rf verlet