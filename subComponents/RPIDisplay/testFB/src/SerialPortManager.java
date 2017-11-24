import java.io.*;
import java.util.ArrayList;
import java.util.List;

import jssc.SerialPort;
import jssc.SerialPortException;
import jssc.SerialPortTimeoutException;

public class SerialPortManager {
 
  protected SerialPort serialPort = null;

  protected void halt() {
    try {
      System.out.println("closing");
      serialPort.closePort();// Close serial port
    } catch(Exception ex) {}
    //System.exit(0);
  }
  
  protected String devFile = "/dev/ttyS0";
  protected int speed = 115200;
  
  public SerialPortManager(String devFile, int speed){
    serialPort = new SerialPort(devFile);
    this.devFile = devFile;
    this.speed = speed;
  }
  
  public void open() throws Exception {
    System.out.println("opening " + serialPort.getPortName());
    serialPort.openPort();// Open serial port

    System.out.println("setting");
    if ( speed != 9600 && speed != 115200 ) {
      throw new Exception("This speed : "+ speed +" is not yet supported");
    }
    serialPort.setParams( (speed == 115200 ? SerialPort.BAUDRATE_115200 : SerialPort.BAUDRATE_9600), 
                         SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
    serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);
  }
  
  public void close() {
    try {
      System.out.println("closing");
      serialPort.closePort();// Close serial port
    } catch(Exception ex) {}
  }
  
  // =======================================================================================
  
  public static void _(Object o) {
    System.out.println(o);
  }

  public void printSerialLine(String str) throws Exception {
    printSerialLine(str, !true);
  }

  public void printSerialLine(String str, boolean consumeEcho) throws Exception {
    str += "\r\n";
    serialPort.writeBytes(str.getBytes());
    if (consumeEcho) {
      serialPort.readBytes(str.length());
    }
  }

  public String readSerialLine() throws Exception {
    return readSerialLine(false);
  }

  public int readChar() throws Exception {
    return readChar( 10 ); // 10ms
  }
  
  public int readChar(int timeout) throws Exception {
    try {
      return b2u( serialPort.readBytes(1, timeout)[0] );
    } catch (SerialPortTimeoutException ex) {
      return -1;
    }
  }
  
  public String readSerialLine(boolean wait1stLineInfinitly) throws Exception {
    String ret = "";
    try {
      // char lastCh = 0x00;
      while (true) {
        char ch = 0x00;
        if (wait1stLineInfinitly) {
          ch = (char) serialPort.readBytes(1)[0];
        } else {
          ch = (char) serialPort.readBytes(1, 1000)[0];
        }
        if (ch < 0) {
          ch += 256;
        }
        // may miss the 2nd return char

        // System.out.print(ch);

        // if (ch == 13 || ch == 10) {
        if (ch == 13) {
          // break;
          continue;
        }
        if (ch == 10) {
          break;
        }
        ret += ch;
      }
    } catch (SerialPortTimeoutException ex) {
      return null;
    }
    return ret;
  }

  static int b2u(byte b) {
    int ret = (int)b;
    if ( ret < 0 ) { ret += 256; }
    return ret;
  }
  
  public static void Zzz(long time) {
    try {
      Thread.sleep(time);
    } catch (Exception ex) {
    }
  }
  
  
}