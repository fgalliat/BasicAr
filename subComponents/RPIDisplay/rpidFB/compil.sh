. /vm_mnt/env.sh

rm -rf bin/* 
mkdir bin
javac -d bin -classpath src:./lib/jssc.jar src/Test.java

# javac -d bin -classpath src src/VGATInterfaceFB.java
# javac -d bin -classpath ../testINPUT/src ../testINPUT/src/MappableKeyboard.java