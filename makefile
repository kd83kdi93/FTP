server:server.o
	@g++ server.cc -o server
clean:
	@rm *.o server client client_resv
client:client.o
	@g++ client.cc -o client
client_resv:client_resv.o
	@g++ client_resv.cc -o client_resv
all:server.o client.o
	@g++ client.cc -o client
	@g++ server.cc -o server
	@g++ client_resv.cc -o client_resv
