all: protocolAnalyzer packetMirror

protocolAnalyzer: protocolAnalyzer.o
	g++ -g protocolAnalyzer.o -o protocolAnalyzer -lrt

protocolAnalyzer.o: protocolAnalyzer.cpp
	g++ -g -c protocolAnalyzer.cpp

packetMirror: packetMirror.o
	g++ -g packetMirror.o -o packetMirror

packetMirror.o: packetMirror.cpp
	g++ -g -c packetMirror.cpp

clean:
	rm -rf *o 
