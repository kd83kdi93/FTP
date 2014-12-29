server:chat.o
	@g++ chat.cc -o chat
clean:
	@rm *.o chat client
client:client.o
	@g++ client.cc -o client
all:chat.o client.o
	@g++ client.cc -o client
	@g++ chat.cc -o chat
