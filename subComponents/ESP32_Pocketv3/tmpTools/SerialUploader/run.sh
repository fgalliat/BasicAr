rm *.class ; 

javac -cp .:../../../../tools/libs/jssc.jar SerialUploader.java ; 

java -cp .:../../../../tools/libs/jssc.jar SerialUploader ../../../../data/TEST.PCT