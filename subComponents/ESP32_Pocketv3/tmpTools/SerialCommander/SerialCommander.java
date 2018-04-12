import java.util.*;
import java.io.*;

import jssc.SerialPort;
import jssc.SerialPortException;
import jssc.SerialPortTimeoutException;

public class SerialCommander {

	static SerialPort serialPort = null;

    public static void main(String[] args) throws Exception {
        serialPort = new SerialPort("/dev/ttyUSB0");
        System.out.println("opening " + serialPort.getPortName());
        serialPort.openPort();// Open serial port

        System.out.println("setting");
        serialPort.setParams(SerialPort.BAUDRATE_115200, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
        serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);
        
        // if ( args == null || args.length < 1 ) {
        //     throw new Exception("missing file/dir name");
        // }

        // String entName = args[0];
        // File entry = new File(entName);

        // try {
        //     if ( entry.isDirectory() ) {
        //         File[] content = entry.listFiles();
        //         for(File ent : content) {
        //             if ( ent.isDirectory() ) { continue; }
        //             doUpload( ent.getAbsolutePath() );
        //             Zzz(500);
        //         }
        //     } else {
        //         doUpload( entry.getAbsolutePath() );
        //     }
        // } catch(Exception ex) {
        //     throw ex;
        // } finally {
        //     try { serialPort.closePort(); }
        //     catch(Exception ex2) {}
        // }

        serialPort.purgePort( serialPort.PURGE_RXCLEAR | serialPort.PURGE_TXCLEAR );

        if ( false ) {
            // reset the MCU
            serWrite( 0x01 );
            return;
        }

        if ( !false ) {
            // set ScreenMode :: 320x240
            serWrite( 0x21 ); serWrite( 2 );
            // clear screen
            serWrite( 0x22 ); 
        }

        if ( false ) {
            // set ScreenMode :: 160x128
            serWrite( 0x21 ); serWrite( 1 );
            // clear screen
            serWrite( 0x22 ); 
        }

        if ( !false ) {
            // draws a PCT file @ 0,0
            serWrite( 0x47 ); serPrintU16(0); serPrintU16(0); serPrintln("/TEST.PCT"); serWrite( 0x00 );
            // draws a PCT cache @ 160,0
            serWrite( 0x47 ); serPrintU16(160); serPrintU16(0);   serWrite( 0x00 );
            serWrite( 0x47 ); serPrintU16(0);   serPrintU16(128); serWrite( 0x00 );
            serWrite( 0x47 ); serPrintU16(160); serPrintU16(128); serWrite( 0x00 );
        }

        if ( !false ) {
            // draws a BPP file @ 0,0
            serWrite( 0x46 ); serPrintU16(160); serPrintU16(0); serPrintln("/LOTUS.BPP"); serWrite( 0x00 );
        }


        if ( !false ) {
            // print a string
            serWrite( 0x32 ); serPrintln("Hello World !"); serWrite( 0x00 );
            // print an int + '\n'
            serWrite( 0x33 ); serPrintFloat( 11 ); 
            serWrite( 0x31 ); serWrite( (int)' ' ); 
            // print a float + '\n'
            serWrite( 0x34 ); serPrintFloat( 3.141596f ); 
            serWrite( 0x31 ); serWrite( (int)'\n' ); 
        }

        try { serialPort.closePort(); }
        catch(Exception ex2) {}


    }

    static void doUpload(String file) throws Exception {
        serialPort.purgePort( serialPort.PURGE_RXCLEAR | serialPort.PURGE_TXCLEAR );

        //final int PACKET_LENGTH = 64;
        final int PACKET_LENGTH = 32;

        try {
            serialPort.readBytes(1, 1000);
        } catch(Exception ex) {}

        System.out.println("SENT SIG UPLOAD");
        // upload via bridge SIGNAL
        serWrite( 0x04 );
        Zzz(200);

        // wait for 0xFF -- ACK
        System.out.println("WAIT ACK");
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
            // for(int i=0; i< readed; i++) {
            //     serWrite(buffer[i]);
            //     Zzz(2);
            // }
            total += readed;
            
            System.out.println("Waiting HandShake ("+ total +" / "+ f.length() +") => "+f.getName());
            int ACK = serialPort.readBytes(1)[0]; // waits for 0xFE

            // volountary after HandShake read
            if ( total >= f.length() ) { break; }
            
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
        //System.out.println(">>> BIN CONTENT ("+ bteCount +" bytes)");

        if ( bteCount > arry.length ) {
            bteCount = arry.length;
        }

        byte[] used = arry;
        if ( arry.length != bteCount ) {
            used = new byte[ bteCount ];
            System.arraycopy(arry, 0, used, 0, bteCount);
        }

        //serialPort.writeBytes( used );
        for(int i=0; i < bteCount; i++) {
            serialPort.writeByte( used[i] );
            //Zzz(2);
        }
        Zzz(10);
        //serialPort.purgePort( serialPort.PURGE_RXCLEAR | serialPort.PURGE_TXCLEAR );
    }

    static void serWrite(int bte) throws Exception {
        //System.out.println(">>> BIN CONTENT ("+ 1 +" byte)");

        serialPort.writeByte( (byte)bte );
    }

    static void serPrintU16(int val) throws Exception {
        serialPort.writeByte( (byte)( val / 256 ) );
        serialPort.writeByte( (byte)( val % 256 ) );
    }

    static void serPrintFloat(float val) throws Exception {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutputStream data = new DataOutputStream(baos);
        data.writeFloat(val);

        byte[] bb = baos.toByteArray();

        // need to reverse endian .....
        //serialPort.writeBytes( bb );
        serialPort.writeByte( bb[3] );
        serialPort.writeByte( bb[2] );
        serialPort.writeByte( bb[1] );
        serialPort.writeByte( bb[0] );
    }


    static void Zzz(long millis) {
        try { Thread.sleep(millis); } catch(Exception ex) {}
    }
}

