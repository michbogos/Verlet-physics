run: main.cpp
	g++ -o verlet main.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 -Ofast

3d: main.c
	cc main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o verlet

clean:
	rm -rf verlet