CXX=g++
CXX_FLAGS=-I/usr/lib/jvm/java-6-openjdk-amd64/include
LDFLAGS=-lpthread

jvmAgent.o: jvmAgent.cpp
	g++ -Wall -fpermissive -fPIC -g -c jvmAgent.cpp $(CXX_FLAGS)
jvmAgent.so: jvmAgent.o
	g++ -Wall -shared -g -o jvmAgent.so jvmAgent.o $(LDFLAGS)