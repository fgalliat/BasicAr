rm -rf bin/*
javac -d bin -cp src:libs/jssc.jar src/Sender.java
java -cp bin:libs/jssc.jar Sender $1