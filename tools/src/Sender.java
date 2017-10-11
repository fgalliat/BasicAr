import java.io.*;
import java.util.ArrayList;
import java.util.List;

import jssc.SerialPort;
import jssc.SerialPortException;
import jssc.SerialPortTimeoutException;

public class Sender {

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
		String fileToSend = args != null && args.length > 0 ? args[0] : null;

		serialPort = new SerialPort("/dev/ttyACM0");
		try {
			System.out.println("opening " + serialPort.getPortName());
			serialPort.openPort();// Open serial port

			System.out.println("setting");
			serialPort.setParams(SerialPort.BAUDRATE_115200, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
			serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);

			String str = null;

Zzz(2000);
str = readSerialLine(!true);
_(str);
while( ( str = readSerialLine() ) != null ) {
	_(str);
	//Zzz(100);
}
_("ready");

			if ( fileToSend != null ) {
				File f = new File( new File("../FS"), fileToSend );
				if ( !f.exists() ) {
					_("File does not exists !");
				} else {
					printSerialLine("NEW", false); Zzz(300);
					dump();

					// printSerialLine("ECHO 0", false); Zzz(300);
					// dump();

					byte[] b = new byte[ (int)f.length() ];
					FileInputStream fis = new FileInputStream(f);
					fis.read(b);
					fis.close();
					boolean hadCR = false;
					for (int i=0; i < b.length; i++) {
						int ch = (int)b[i]; if ( ch < 0 ) { ch += 256; }
						if ( ch == '\r' ) { hadCR = true; }
						if ( ch == '\n' ) {
							if ( !hadCR ) { serialPort.writeByte( (byte) '\r'); }
							hadCR = false;

							//ch = '\r'; // seems to parse ONLY CR !!!!!
						} 

						serialPort.writeByte( (byte)ch );
						System.out.print( (char)ch );
						Zzz(50);

						if ( ch == '\r' || ch == '\n' ) { 
							Zzz(300); 
							//dump(); 
						}

					}
					Zzz(600);

					// printSerialLine("ECHO 1", false); Zzz(400);
					// dump();

					printSerialLine("*", false); Zzz(400);
					dump();

					_("-EOF-");
				}
			}




// 			// =========================

//             if ( args.length > 1 && args[1].equals("send") ) {
// 				String localFile = args[2];
// 				String remoteFile = new File(args[2]).getName();
// 				if ( args.length > 3) { remoteFile = args[3]; }

// 				while ((str = readSerialLine()) != null) {
// 					System.out.println("$> " + str);
// 				}

// 				pumpThread();


// //Zzz(5000);
// System.out.println("Copying "+ localFile +" to SD...");

// 				// TMP cmd
// 				//serialPort.writeBytes( new byte[]{'w'} );
// 				//serialPort.writeBytes((remoteFile+"\n").getBytes() );
// 				serialPort.writeBytes(("DUMPSTS \""+remoteFile+"\"\n").getBytes() );
// Zzz(500);
// 				File f = new File(localFile);
// 				FileInputStream fin = new FileInputStream( f );
// 				  byte[] buff = new byte[ (int)f.length()];
// 				  fin.read( buff );
// 				  serialPort.writeBytes(buff);
// 				fin.close();
// Zzz(3000+1000);
// System.out.println("done...");

// 				// 'ls'
// 				//serialPort.writeBytes(("d\n").getBytes() );
// 				serialPort.writeBytes(("FILES\n").getBytes() );
// Zzz(5000);	

// 				halt();
// 			} else if ( args.length > 1 && args[1].equals("ls") ) {
				
// 				while ((str = readSerialLine()) != null) {
// 					System.out.println("$> " + str);
// 				}

// 				pumpThread();
				
// 				//Zzz(5000);

// 				//serialPort.writeBytes(("d"+"\n").getBytes() );
// 				// serialPort.writeBytes(("FILES\n").getBytes() );
// 				Zzz(5000);
// 				halt();
// 			} else if ( args.length > 1 && args[1].equals("rm") ) {
// 				String localFile = args[2];
// 				while ((str = readSerialLine()) != null) {
// 					System.out.println("$> " + str);
// 				}
// 				//pumpThread();

// 				//Zzz(5000);
// 				//serialPort.writeBytes( new byte[]{'z'} );
// 				//serialPort.writeBytes((localFile+"\n").getBytes() );

// 				serialPort.writeBytes(("DELETE \""+ localFile +"\"\n").getBytes() );

// 				Zzz(500);
// 				//Zzz(2000);
// 				while ((str = readSerialLine()) != null) {
// 					System.out.println("$> " + str);
// 				}
// 				halt();
// 			} else if ( args.length > 1 && args[1].equals("pt5k") ) {
// 				String localFile = args[2];
// 				while ((str = readSerialLine()) != null) {
// 					System.out.println("$> " + str);
// 				}
// 				serialPort.writeBytes(("PLAYT5K \""+ localFile +"\"\n").getBytes() );
// 				Zzz(500);
// 				while ((str = readSerialLine()) != null) {
// 					System.out.println("$> " + str);
// 				}
// 				halt();
// 			} else if ( args.length > 1 && args[1].equals("pt53") ) {
// 				String localFile = args[2];
// 				while ((str = readSerialLine()) != null) {
// 					System.out.println("$> " + str);
// 				}
// 				serialPort.writeBytes(("PLAYT53 \""+ localFile +"\"\n").getBytes() );
// 				Zzz(500);
// 				while ((str = readSerialLine()) != null) {
// 					System.out.println("$> " + str);
// 				}
// 				halt();
// 			} else if ( args.length > 1 && args[1].equals("cat") ) {
// 				String localFile = args[2];
// 				while ((str = readSerialLine()) != null) {
// 					System.out.println("$> " + str);
// 				}
// 				//pumpThread();

// 				//Zzz(5000);
// 				//serialPort.writeBytes( new byte[]{'c'} );
// 				//serialPort.writeBytes((localFile+"\n").getBytes() );
// 				serialPort.writeBytes(("CAT \""+ localFile +"\"\n").getBytes() );
// 				Zzz(500);
// 				//Zzz(2000);
// 				while ((str = readSerialLine()) != null) {
// 					System.out.println("$> " + str);
// 				}
// 				halt();
// 			}

// 			// =========================

// 			// printSerialLine("mem");
// 			while ((str = readSerialLine()) != null) {
// 				System.out.println("$> " + str);
// 			}

// 			//_("1. CINV");
// 			//_("2. WRITETXT a file");
// 			//_("3. CAT a file");
// 			//_("4. FILES the SD");
// 			_("0. Exit");
// 			kbd = new BufferedReader(new InputStreamReader(System.in));
// 			str = kbd.readLine().trim();

// 			boolean writeFile = false;

// 			if (str.equals("1")) {
// 				printSerialLine("cinv", false);
// 				halt();
// 			} else if (str.equals("2")) {
// 				writeFile = true;
// 			} else if (str.equals("4")) {
// 				printSerialLine("files");
// 				while ((str = readSerialLine()) != null) {
// 					System.out.println("$> " + str);
// 				}
// 				halt();
// 			} else if (str.equals("0")) {
// 				halt();
// 			}

// 			String filename = null;
// 			while (true) {
// 				_("Filename ? ");
// 				filename = kbd.readLine();
// 				if (filename == null || filename.length() == 0) {
// 					_("Invalid filname");
// 				} else {
// 					break;
// 				}
// 			}

// 			String line = null;
// 			if (writeFile) {
// 				printSerialLine("delete " + filename);
// 				while ((str = readSerialLine()) != null) {
// 					System.out.println("$> " + str);
// 				}

// 				System.out.println("Paste your text, till '+++'");
// 				List<String> buffer = new ArrayList<String>();
// 				while ((line = kbd.readLine()) != null && !line.equals("+++")) {
// 					buffer.add(line);
// 				}

// 				printSerialLine("writetxt " + filename);
// 				for (String line0 : buffer) {
// 					printSerialLine(line0, false);
// 				}
// 				_("-EOF-");

// 				_("Press Enter when ready...");
// 				kbd.readLine();
// 			}

// 			printSerialLine("cat " + filename);
// 			while ((line = readSerialLine()) != null) {
// 				_(">>" + line);
// 			}
// 			_("-EOF-");

			// System.out.println("Press Enter...");
			// new BufferedReader(new InputStreamReader(System.in)).readLine();

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
