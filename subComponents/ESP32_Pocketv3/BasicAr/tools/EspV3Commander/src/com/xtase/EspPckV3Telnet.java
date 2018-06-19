package com.xtase;

import java.io.*;
import java.net.Socket;

/**
 * Created by fgalliat on 14/05/18.
 * Telnet control implementation
 */
public class EspPckV3Telnet {

    protected Socket sk = null;
    protected PrintStream skOut = null;
    protected BufferedReader skIn = null;
    protected IPrinter dbugStream = null;

    public EspPckV3Telnet(String ip) throws Exception {
        this(new SystemPrinter(), ip);
    }

    public EspPckV3Telnet(IPrinter dbugStream, String ip) throws Exception {
        this.dbugStream = dbugStream;

        this.sk = new Socket(ip, 23);
        this.skOut = new PrintStream(this.sk.getOutputStream());
        this.skIn = new BufferedReader(new InputStreamReader(this.sk.getInputStream()));

        skOut.println("Hello !");
        Zzz(500);
    }


    public void close() {
        try {
            skOut.println("/quit");
        } catch (Exception ex) {
        }
        Zzz(500);
        try {
            skIn.close();
        } catch (Exception ex) {
        }
        try {
            skOut.close();
        } catch (Exception ex) {
        }
        try {
            sk.close();
        } catch (Exception ex) {
        }
    }

    public void print(String str) throws Exception {
        if (str == null) {
            return;
        }
        skOut.write(str.getBytes());
    }

    public void println(String str) throws Exception {
        print(str + "\n");
    }

    public String readLine() throws Exception {
        return skIn.readLine();
    }

    protected void Zzz(long time) {
        try {
            Thread.sleep(time);
        } catch (Exception ex) {
        }
    }

    public void sendFile(String filename) throws Exception {
        File f = new File(filename);
        int size = (int) f.length();
        String name = f.getName();

        byte[] buff = new byte[size];
        FileInputStream fis = new FileInputStream(f);
        fis.read(buff);
        fis.close();

        skOut.println("EXEC \"WIFI\",\"UPLOAD\" ");
        Zzz(100);
        skOut.println(name);
        skOut.println("" + size);
        Zzz(300);

        skOut.write(buff, 0, size);
        skOut.flush();

        String line;
        while ((line = skIn.readLine()) != null) {
            if (line.equalsIgnoreCase("-EOF-")) {
                break;
            }
        }
        dbugStream.println("Client received the file");

        skOut.println();
        Zzz(300);
    }

    public void sendDir(String dirName) throws Exception {
        File dir = new File(dirName);
        File[] files = dir.listFiles();
        dbugStream.println("-Start sending dir : " + dirName + " -");
        for (File f : files) {
            if (f.isDirectory()) {
                continue;
            }
            dbugStream.println("Sending " + f.getName());
            sendFile(f.getAbsolutePath());
        }
        dbugStream.println("-EOT-");
    }


    public void sendSomething(String entryName, boolean autoClose) throws Exception {
        if (new File(entryName).isDirectory()) {
            sendDir(entryName);
        } else {
            sendFile(entryName);
        }
        if (autoClose) skOut.println("/quit");
    }


}
