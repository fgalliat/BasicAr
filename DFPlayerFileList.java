import java.util.Comparator;
import java.util.Iterator;
import java.util.Set;
import java.util.TreeSet;
import java.io.File;

public class DFPlayerFileList {


    public static void treatDir(String dir, String outFile) throws Exception {
        File f = new File(dir);
        File[] content = f.listFiles();
        Set<String> result = new TreeSet<String>( new Comparator<String>() {
            public int compare(String e0, String e1) {
                return e0.compareTo(e1);
            }
        } );
        for(int i=0; i < content.length; i++) {
            if ( content[i].getName().toLowerCase().endsWith(".mp3") ) {
                result.add( content[i].getName() );
            }
        }
        System.out.println(result.size());
        System.out.println("# idx / name");
        Iterator<String> itFile = result.iterator();
        for(int i=0; itFile.hasNext(); i++) {
            String name = itFile.next();
            String num = (i < 9 ? "0" : "")+ (i+1);
            System.out.println(num+";"+name);
        }
    }


    public static void main(String[] args) throws Exception {
        treatDir("./MP3", "./data/JUKE.BAD");
    }



}