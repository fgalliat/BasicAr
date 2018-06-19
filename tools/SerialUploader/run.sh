rm *.class ; 

javac -cp .:../libs/jssc.jar SerialUploader.java ; 

java -cp .:../libs/jssc.jar SerialUploader ../../../../data/CARO.PCT