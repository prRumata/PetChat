all:
	clang++ src/Server/Main.cpp src/Server/Server.cpp -o bin/Server
	clang++ src/Client/Main.cpp src/Client/Client.cpp -o bin/Client
