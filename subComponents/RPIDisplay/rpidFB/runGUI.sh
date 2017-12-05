alias java=jamvm
java -Djava.library.path=./lib -cp bin:./lib/jssc.jar TestGUI $*