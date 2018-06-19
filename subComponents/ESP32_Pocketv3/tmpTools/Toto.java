public class Toto {


public static void main(String[] args) {

 System.out.println("{");
 for (int i=0x00; i <= 0x45; i++) {
   String num = Integer.toHexString( i );
   if ( num.length() < 2 ) { num = '0'+num; }
   System.out.println(" -2, // 0x"+num);
 }
 System.out.println("}");


}



}