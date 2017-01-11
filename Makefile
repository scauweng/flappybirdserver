EPOLLOBJECTS=EpollServer.o Game.o MessageBuffer.o Message.o
GAMEOBJECTS=GameServer.o Game.o MessageBuffer.o Message.o

target:EpollServer GameServer


EpollServer: $(EPOLLOBJECTS)
	g++ -o $@ $^

GameServer: $(GAMEOBJECTS)
	g++ -o $@ $^

Game.o: Game.cpp
	g++ -c Game.cpp 

MessageBuffer.o:libmessage/MessageBuffer.cpp
	g++ -c libmessage/MessageBuffer.cpp 

Message.o:libmessage/Message.cpp
	g++ -c libmessage/Message.cpp 

GameServer.o: GameServer.cpp
	g++ -c GameServer.cpp 

EpollServer.o: EpollServer.cpp
	g++ -c EpollServer.cpp	

TestJson.o:TestJson.cpp
	g++ -c TestJson.cpp 

install:
	

clean:
	rm -rf $(EPOLLOBJECTS) $(GAMEOBJECTS) EpollServer GameServer

