old_IFS=$IFS
IFS=$'\n'
for i in `cat FS/test.bas`
do
        echo $i
	echo -ne "$i\r\n" > /dev/ttyACM0
        sleep .5

done
IFS=$old_IFS