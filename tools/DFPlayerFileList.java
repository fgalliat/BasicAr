import java.util.Comparator;
import java.util.Iterator;
import java.util.Set;
import java.util.TreeSet;
import java.io.File;
import java.io.FileOutputStream;
import java.io.PrintStream;

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

        FileOutputStream outFileOs = new FileOutputStream( outFile );
        PrintStream outF = new PrintStream(outFileOs);

        System.out.println(result.size());
        outF.println(result.size());
        System.out.println("# idx / name");
        outF.println("# idx / name");
        Iterator<String> itFile = result.iterator();
        for(int i=0; itFile.hasNext(); i++) {
            String name = itFile.next();
            String num = (i < 99 ? "0" : "")+ (i < 9 ? "0" : "")+ (i+1);
            System.out.println(num+";"+name);
            outF.println(num+";"+name);
        }

        outFileOs.flush();
        outFileOs.close();
    }

    public static void reindexDir(String dir) throws Exception {
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

        Iterator<String> itFile = result.iterator();
        for(int i=0; itFile.hasNext(); i++) {
            String name = itFile.next();
            String num = (i < 99 ? "0" : "")+ (i < 9 ? "0" : "")+ (i+1);
            // if ( name.indexOf("_") != 2 ) { continue; }
            String newName = num +'_'+ name.substring( name.indexOf('_')+1 );
            System.out.println(num+"| "+name+"   =>   "+newName);

            if ( true ) { new File(f, name).renameTo(new File(f, newName)); }
        }
    } // end of reindexDir()


    public static void main(String[] args) throws Exception {
        //reindexDir("./MP3");
        //treatDir("./MP3", "./data/JUKE.BAD");
        if ( args == null || args.length < 1 ) {
            System.out.println("dir name required");
            return;
        }
        //reindexDir( args[0] );
        treatDir(args[0], "../data/JUKE.BAD");
    }



}