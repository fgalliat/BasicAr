import java.util.*;
import java.io.*;
import java.net.*;

public class EspFileSender {

    protected PrintStream out = null;
    protected BufferedReader in = null;

    public void sayHello() throws Exception {
        out.println("Hi bob !");
    }

    public void sayBye() throws Exception {
        out.println("/quit");
    }

    public void sendFile(String filename) throws Exception {
        File f = new File(filename);
        int size = (int)f.length();
        String name = f.getName();

        byte[] buff = new byte[ size ];
        FileInputStream fis = new FileInputStream(f);
        fis.read(buff);
        fis.close();

        out.println("EXEC \"WIFI\",\"UPLOAD\" ");
        Zzz(100);
        out.println(name);
        out.println(""+size);
        Zzz(300);

        out.write( buff, 0, size );
        out.flush();

        String line;
        while( ( line = in.readLine() ) != null ) {
            if ( line.equalsIgnoreCase("-EOF-") ) {
                break;
            }
        }
        System.out.println("Client received the file");

        out.println();
        Zzz(300);
    }
    

    public void sendDir(String dirName) throws Exception {
        File dir = new File(dirName);
        File[] files = dir.listFiles();
        System.out.println("-Start sending dir : "+ dirName +" -");
        for(File f : files) {
            if ( f.isDirectory() ) { continue; }
            System.out.println("Sending "+f.getName());
            sendFile(f.getAbsolutePath());
        }
        System.out.println("-EOT-");
    }

    public void sendSomething(String entryName, String ip) throws Exception {
        Socket sk = new Socket(ip, 23);
        out = new PrintStream( sk.getOutputStream() );
        in = new BufferedReader(new InputStreamReader(sk.getInputStream()));
        sayHello();
        Zzz(500);
        if ( new File(entryName).isDirectory() ) {
            sendDir(entryName);
        } else {
            sendFile(entryName);
        }
        sayBye();
        Zzz(500);
        sk.close();
    }

    static void Zzz(long time) {
        try { Thread.sleep(time); } catch(Exception ex) {}
    }

    public static void main(String[] args) throws Exception {
        // "192.168.4.1" -> if APmode
        String ip = "192.168.4.1";
        String whatToSend = "./data/";
        if ( args != null && args.length >= 1 ) { whatToSend = args[0]; }
        if ( args != null && args.length >= 2 ) { ip = args[1]; }

        if ( ip.equals("192.168.4.1") ) {
            System.out.println("Be sure to be on right ESP-AP");
        }

        new EspFileSender().sendSomething(whatToSend, ip);
    }


}