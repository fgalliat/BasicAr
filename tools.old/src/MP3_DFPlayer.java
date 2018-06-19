import java.io.*;
import java.util.ArrayList;
import java.util.List;

import jssc.SerialPort;
import jssc.SerialPortException;
import jssc.SerialPortTimeoutException;

/**
 * Wingoneer DFPlayer Mini (mp3 module)
 * 
*/


public class MP3_DFPlayer {

	static SerialPort serialPort = null;
	static BufferedReader kbd = null;

	static void halt() {
		try {
		System.out.println("closing");
		serialPort.closePort();// Close serial port
		} catch(Exception ex) {}
		System.exit(0);
	}


	// public static void pumpThread() {
	// 	new Thread(  ) {
	// 		public void run() {
	// 			int ch;
	// 			try {
	// 				while((ch = serialPort.readBytes(1)[0]) != -1) {
	// 					System.out.print((char)ch);
	// 				}
	// 			}catch(Exception ex) {
	// 				ex.printStackTrace();
	// 				halt();
	// 			}
	// 			System.out.println();
	// 		}
	// 	}.start();
	// }


	static void dump() throws Exception {
		String str = readSerialLine(!true);
		_(str);
		while( ( str = readSerialLine() ) != null ) {
			_(str);
		}
	}

	public static void main(String[] args) throws Exception {
		serialPort = new SerialPort("/dev/ttyUSB0");
		try {
			System.out.println("opening " + serialPort.getPortName());
			serialPort.openPort();// Open serial port

			System.out.println("setting");
			serialPort.setParams(SerialPort.BAUDRATE_9600, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
			serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);

// 			String str = null;
// Zzz(2000);
// str = readSerialLine(!true);
// _(str);
// while( ( str = readSerialLine() ) != null ) {
// 	_(str);
// 	//Zzz(100);
// }


Zzz(3000); // can be up to 3sec to set-up..
_("ready");

final int BEGIN_FRAME = 0x7E;
final int END_FRAME   = 0xEF;

final int VERSION     = 0xFF;

final int FEEDBACK_NONE = 0x00;

final int CMD_PLAY_NUM = 0x03;

int PACKET_LENGTH = 6; // (VERSION, len, cmd, feedback, para1MSB, para2LSB)

int chk0 = -1;
int chk1 = -1;
// 0x00, 0x01 -> for track num 1 (0-based ??????) (0-2999)
int[] frame = { BEGIN_FRAME, VERSION, PACKET_LENGTH, CMD_PLAY_NUM, FEEDBACK_NONE, 0x00, 0x01, chk0, chk1, END_FRAME };

// not certified !!!!!
int chk_ = frame[1]+frame[2]+frame[3]+frame[4]+frame[5]+frame[6];
//chk0 = chk_ / 256;
chk0 = 0xFF; // TEMP
chk1 = chk_ % 256;

frame[ 1 + PACKET_LENGTH + 0 ] = chk0;
frame[ 1 + PACKET_LENGTH + 1 ] = chk1;

System.out.println(chk_);
System.out.println(chk0);
System.out.println(chk1);

for(int i=0; i < frame.length; i++) {
  System.out.print( Integer.toHexString( frame[i] )+" " );
  serialPort.writeByte( (byte)frame[i] );
}
System.out.println();

Zzz(100); // Cf specs



			System.out.println("closing");
			serialPort.closePort();// Close serial port
		} catch (SerialPortException ex) {
			System.out.println(ex);
		}

	}

	public static void _(Object o) {
		System.out.println(o);
	}

	public static void printSerialLine(String str) throws Exception {
		printSerialLine(str, true);
	}

	public static void printSerialLine(String str, boolean consumeEcho) throws Exception {
		str += "\r\n";
		serialPort.writeBytes(str.getBytes());
		if (consumeEcho) {
			serialPort.readBytes(str.length());
		}
	}

	public static String readSerialLine() throws Exception {
		return readSerialLine(false);
	}

	public static String readSerialLine(boolean wait1stLineInfinitly) throws Exception {
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

	public static void Zzz(long time) {
		try {
			Thread.sleep(time);
		} catch (Exception ex) {
		}
	}

}
