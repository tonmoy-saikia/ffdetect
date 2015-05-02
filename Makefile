CC=g++
CFLAGS+=`pkg-config --cflags opencv`
LDFLAGS+=`pkg-config --libs opencv`

% : %.cpp
	$(CC) $< $(CFLAGS) $(LDFLAGS) -o $@
