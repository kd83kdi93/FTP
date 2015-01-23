server:server.o
	@g++ server.cc -g -o server
clean:
	@rm *.o server client
client:client.o
	@g++ client.cc -o client
all:server.o client.o
	@g++ client.cc -o client
	@g++ server.cc -o server
