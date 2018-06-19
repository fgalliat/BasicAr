rm -rf bin/*
javac -d bin -cp src:libs/jssc.jar src/MP3_DFPlayer.java
java -cp bin:libs/jssc.jar MP3_DFPlayer $1