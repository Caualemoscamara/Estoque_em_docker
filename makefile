CC = gcc
CFLAGS = -Wall -Wextra -pthread

# Targets
all: server client clientDocker clientNGROK

server: server.c protocol.h
	$(CC) $(CFLAGS) server.c -o server

client: client.c protocol.h
	$(CC) $(CFLAGS) client.c -o client

clientDocker: clientDocker.c protocol.h
	$(CC) $(CFLAGS) clientDocker.c -o clientDocker

clientNGROK: clientNGROK.c protocol.h
	$(CC) $(CFLAGS) clientNGROK.c -o clientNGROK

clean:
	rm -f server client clientDocker clientNGROK