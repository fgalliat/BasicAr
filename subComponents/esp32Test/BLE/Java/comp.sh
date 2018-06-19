rm -rf bin
mkdir bin
echo "= Cleaned ="


javac -d bin -cp src:lib/bluecove-2.1.0.jar src/TestBlueCove.java
javac -d bin -cp src:lib/bluecove-2.1.0.jar src/BTMACFinder.java

echo "= Compiled ="

# java -cp bin:lib/bluecove-2.1.0.jar:lib/bluecove-gpl-2.1.0.jar TestBlueCove
java -cp bin:lib/bluecove-2.1.0.jar:lib/bluecove-gpl-2.1.0.jar BTMACFinder
