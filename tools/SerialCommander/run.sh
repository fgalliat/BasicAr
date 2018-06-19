rm *.class ; 

javac -cp .:../../../../tools/libs/jssc.jar SerialCommander.java

java -cp .:../../../../tools/libs/jssc.jar SerialCommander $*
