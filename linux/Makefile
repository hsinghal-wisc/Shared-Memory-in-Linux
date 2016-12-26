CC=gcc
CFLAGS=-Wall

all: lib stats_client stats_server  

stats_client: stats_client.c stats.h
	$(CC) -L. $(CFLAGS) -pthread -o stats_client  stats_client.c -lstats

stats_server: stats_server.c stats.h
	$(CC) -o stats_server $(CFLAGS) stats_server.c -pthread

lib: 
	$(CC) -c -fpic stats_lib.c $(CFLAGS)
	$(CC) -shared -o libstats.so stats_lib.o

export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

clean:
	$(RM) stats_client stats_server stats_lib.o libstats.so
