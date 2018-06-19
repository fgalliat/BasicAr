import java.io.*;
import java.util.*;
import java.net.*;

public class EspTelnetEmul {

    public static void main(String[] args) throws Exception {
        new EspTelnetEmul().doWork();
    }

    public void doWork() throws Exception {
        ServerSocket ssk = new ServerSocket(23);
        _("listening on :23");
        Socket sk = ssk.accept();
        _("connected on :23");

        BufferedReader in = new BufferedReader( new InputStreamReader( sk.getInputStream() ) );
        PrintStream out = new PrintStream( sk.getOutputStream() );

        out.println("Hi, press enter please");
        String line;
        while( (line = in.readLine()) != null ) {
            line = line.trim();
            if ( line.equals("/quit") ) {
                break;
            }
            System.out.println("> "+line);
            out.println("OK");
        }

        sk.close();
        ssk.close();
    }



    static void _(Object o) {
        System.out.println(o);
    }

}

