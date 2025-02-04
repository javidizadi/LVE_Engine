CFLAGS = -std=c++17 -O2
# LDFLAGS = -lglfw -lvulkan
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lwayland-client -lX11 -lXxf86vm -lXrandr -lXi
# LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lwayland-client 

a.out: *.cpp 
	g++ $(CFLAGS) -o ./a.out *.cpp $(LDFLAGS)

.PHONY: clean

clean:
	rm -f a.out
