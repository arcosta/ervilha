DIRS = src tests
debug: jvmAgent.so Hello.class
	java -agentpath:./jvmAgent.so Hello
run: Hello.class
	$(MAKE) -C tests
clean:
	rm jvmAgent.so jvmAgent.o
Hello.class: Hello.java
	javac Hello.java

package:
	zip jvmAgent-$(date '+%F_%H-%M').zip Makefile *.cpp *.h Hello.* *.kdev4
