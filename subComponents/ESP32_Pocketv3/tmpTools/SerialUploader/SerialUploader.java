import java.util.*;
import java.io.*;

import jssc.SerialPort;
import jssc.SerialPortException;
import jssc.SerialPortTimeoutException;

public class SerialUploader {

	static SerialPort serialPort = null;

    public static void main(String[] args) throws Exception {
        serialPort = new SerialPort("/dev/ttyUSB0");
        System.out.println("opening " + serialPort.getPortName());
        serialPort.openPort();// Open serial port

        System.out.println("setting");
        serialPort.setParams(SerialPort.BAUDRATE_115200, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
        serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);
        
        try {
            doUpload( args[0] );
        } catch(Exception ex) {
            throw ex;
        } finally {
            try { serialPort.closePort(); }
            catch(Exception ex2) {}
        }
    }

    static void doUpload(String file) throws Exception {
        final int PACKET_LENGTH = 64;

        // upload via bridge SIGNAL
        serWrite( 0x04 );
        Zzz(200);

        // wait for 0xFF -- ACK
        int ok = serialPort.readBytes(1)[0];

        File f = new File(file);
        String newFentry = "/"+f.getName().toUpperCase();

        serPrintln( newFentry );
        Zzz(200);
        serPrintln( ""+f.length() );
        Zzz(500);

        FileInputStream fis = new FileInputStream(f);
        byte[] buffer = new byte[ PACKET_LENGTH ];

        // for(int i=0; i < f.length(); i+= PACKET_LENGTH) {
        int total = 0;
        while(true) { 
            int readed = fis.read(buffer);
            serWrite(buffer, readed);
            total += readed;
            if ( total >= f.length() ) { break; }

            System.out.println("Waiting HandShake");
            int ACK = serialPort.readBytes(1)[0]; // waits for 0xFE

            // Zzz(15);
        }
        fis.close();
        // Zzz(200);
        System.out.println("Waiting -EOT-");
        int ACK = serialPort.readBytes(1)[0]; // waits for 0xFF

        System.out.println("-EOF-");
    } 


    static void serPrintln(String str) throws Exception {
        System.out.println(">>> "+str);

        String str2 = str+"\n";
        serWrite( str2.getBytes(), str2.length() );
    }

    static void serWrite(byte[] arry, int bteCount) throws Exception {
        System.out.println(">>> BIN CONTENT ("+ bteCount +" bytes)");

        if ( bteCount > arry.length ) {
            bteCount = arry.length;
        }

        byte[] used = arry;
        if ( arry.length != bteCount ) {
            used = new byte[ bteCount ];
            System.arraycopy(arry, 0, used, 0, bteCount);
        }

        serialPort.writeBytes( used );
    }

    static void serWrite(int bte) throws Exception {
        System.out.println(">>> BIN CONTENT ("+ 1 +" byte)");

        serialPort.writeByte( (byte)bte );
    }


    static void Zzz(long millis) {
        try { Thread.sleep(millis); } catch(Exception ex) {}
    }
}

