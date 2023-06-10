run: main.cpp
	g++ -o verlet main.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 -Ofast