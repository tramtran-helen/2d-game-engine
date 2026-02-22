build:
	g++ -std=c++17 -Wall \
		-Ilibs/glm \
		-Ilibs/imgui \
		-Ilibs/lua \
		-Ilibs/sol \
		-I/opt/homebrew/include/SDL2 \
		src/Main.cpp -o testMain \
		-L/opt/homebrew/lib \
		-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer \
		-llua

run:
	./testMain

clean:
	rm -f testMain
