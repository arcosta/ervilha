CXX=g++
CXX_FLAGS=-I/usr/lib/jvm/java-6-openjdk-amd64/include
LDFLAGS=-lpthread -lmicrohttpd

debug: jvmAgent.so Hello.class
	java -agentpath:./jvmAgent.so Hello
run: Hello.class
	java Hello
clean:
	rm jvmAgent.so jvmAgent.o
Hello.class: Hello.java
	javac Hello.java
jvmAgent.o: jvmAgent.cpp
	g++ -Wall -fpermissive -fPIC -g -c jvmAgent.cpp $(CXX_FLAGS)
jvmAgent.so: jvmAgent.o
	g++ -Wall -shared -g -o jvmAgent.so jvmAgent.o $(LDFLAGS)
package:
	zip jvmAgent-$(date '+%F_%H-%M').zip Makefile *.cpp *.h Hello.* *.kdev4
