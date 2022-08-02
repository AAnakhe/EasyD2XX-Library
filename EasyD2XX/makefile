#.PHONY : all


# "make header" to generate the .h file with JNi stubs.
header:
	mkdir -p jni
	javac -h jni src/com/example/easyd2xx/EasyD2XX.java
	rm src/com/example/easyd2xx/EasyD2XX.class
	
# "make sig" to ask the user for a class name, and print out the field and method signatures for the class.
sig:
	@bash -c 'read -p "fully-qualified class name (example: java.util.List) ? " CLASSNAME && javap -s $$CLASSNAME';
	
# "make signature" to print the field and method signatures for the EasyD2XX  class
signatures:
	javac src/com/example/easyd2xx/EasyD2XX.java -d bin
	javap -s -p bin/com/example/easyd2xx/EasyD2XX.class
	
# "make dll" to compile the source code into a .dll file.
dll:
	gcc jni/com_example_easyd2xx_EasyD2XX.c jni/ftd2xx.lib -I"C:\Program Files\Java\jdk-16.0.2\include" -I"C:\Program Files\Java\jdk-16.0.2\include\win32" -shared -o EasyD2XX.dll
	file EasyD2XX.dll
	mn EasyD2XX.dll | grep Java
	ldd EasyD2XX.dll

all: dll  