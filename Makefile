run: main.cpp
	g++ -o verlet main.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 -Ofast

3d: 3d.cpp
	g++ 3d.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o verlet -Ofast

web: 3d.cpp
	em++ -o game.html 3d.cpp -Os  -L. ./libraylib.a -s ASYNCIFY -s USE_GLFW=3 -I ./ -DPLATFORM_WEB --preload-file basic.fs --preload-file basic.vs -sASSERTIONS

clean:
	rm -rf verlet