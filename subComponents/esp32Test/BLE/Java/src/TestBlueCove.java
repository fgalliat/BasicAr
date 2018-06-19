import java.io.*;
import javax.microedition.io.*;
import javax.bluetooth.*;
public class TestBlueCove {
 
 public void startclient() {
   try {   
//   String url = "btspp://01576e1a82a6:1";
     String url = "btspp://30AEA41B9036:1";
        StreamConnection con = 
            (StreamConnection) Connector.open(url);
        OutputStream os = con.openOutputStream();
        InputStream is = con.openInputStream();
        InputStreamReader isr = new InputStreamReader(System.in);
        BufferedReader bufReader = new BufferedReader(isr);
        RemoteDevice dev = RemoteDevice.getRemoteDevice(con);

     /**   if (dev !=null) {
         File f = new File("test.xml");
         InputStream sis = new FileInputStream("test.xml");
         OutputStream oo = new FileOutputStream(f);
         byte buf[] = new byte[1024];
         int len;
         while ((len=sis.read(buf))>0)
          oo.write(buf,0,len);
         sis.close();
        }  **/
        
     if (con !=null) {
      while (true) {   
         //sender string
     System.out.println("Server Found:" 
         +dev.getBluetoothAddress()+"\r\n"+"Put your string"+"\r\n");
        String str = bufReader.readLine();
        os.write( str.getBytes());
       //reciever string
        byte buffer[] = new byte[1024];
        int bytes_read = is.read( buffer );
        String received = new String(buffer, 0, bytes_read);
        System.out.println("client: " + received
         + "from:"+dev.getBluetoothAddress()); 
      } 
        } 
        }
  catch(Exception e){}  
 }
 
 
    public static void main( String args[] ) {
       try{
      LocalDevice local = LocalDevice.getLocalDevice();
      System.out.println("Address:"+local.getBluetoothAddress()
        +"+n"+local.getFriendlyName());
       }
       catch (Exception e) {System.err.print(e.toString());}
     try {        
     TestBlueCove ss = new TestBlueCove();
        while(true){
     ss.startclient();
        }
        } catch ( Exception e ) {
            System.err.print(e.toString());
        }
    }
}//main
