file1=chat.cc
file2=client.cc

exefile1=chat
exefile2=client

server:$(exefile1).o
	@g++ $(file1) -o $(exefile1)
clean:
	@rm *.o $(exefile1) $(exefile2)
client:$(exefile2).o
	@g++ $(file2) -o $(exefile2)
all:$(exefile1).o $(exefile2).o
	@g++ $(file1) -o $(exefile1)
	@g++ $(file2) -o $(exefile2)
