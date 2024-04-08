run: main.cpp
	g++ -o verlet main.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 -Ofast -fopenmp

3d: 3d.cpp
	g++ 3d.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o verlet -Ofast -march=native -mtune=native -funroll-loops -fno-signed-zeros -fno-trapping-math -fopenmp -D_GLIBCXX_PARALLEL

3d-blaze: 3d-blaze.cpp
	g++ 3d-blaze.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o verlet -Ofast -march=native -mtune=native -funroll-loops -fno-signed-zeros -fno-trapping-math -fopenmp -D_GLIBCXX_PARALLEL

3d-array: 3d-array.cpp
	g++ 3d-array.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o verlet -Ofast -march=native -mtune=native -funroll-loops -fno-signed-zeros -fno-trapping-math -fopenmp -D_GLIBCXX_PARALLEL -g

web: 3d.cpp
	em++ -o game.html 3d_web.cpp -Os  -L. ./libraylib.a -s ASYNCIFY -s USE_GLFW=3 -I ./ -DPLATFORM_WEB --preload-file web_basic.fs --preload-file web_basic.vs -sASSERTIONS --shell-file=template.html

hashgrid: hashgrid.cpp
	g++ hashgrid.cpp -o verlet -Ofast -fopenmp -std=c++17 -ltbb -Wall -Wextra -pedantic -lX11 -lGL -lpthread -lpng -lstdc++fs -march=native -mtune=native -funroll-loops -fno-signed-zeros -fno-trapping-math -fopenmp -D_GLIBCXX_PARALLEL

clean:
	rm -rf verlet