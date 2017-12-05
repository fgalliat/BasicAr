alias java=jamvm
java -Djava.library.path=./lib -cp bin:../testSerial/lib/jssc.jar TestGUI $*