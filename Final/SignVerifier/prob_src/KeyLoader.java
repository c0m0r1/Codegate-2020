import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

class KeyLoader{
  public String exec(String path, boolean isDebug){
    if(isDebug){
      return "thisissamplekey!";
    }
    else{
      byte [] res = new byte[16];
      int c = 0;
      try{
        File f = new File(path);

        if(res.length < f.length())
          res = new byte[(int)f.length()];

        FileReader fr = new FileReader(f);
        for(int i = 0; (c = fr.read()) != -1; i++){
          res[i] = (byte) c;
        }
      
        fr.close();
      }
      catch (FileNotFoundException e){
         System.out.println(e);
         System.exit(1);
      }
      catch (IOException e){
        System.out.println(e);
        System.exit(1);
      }
      
      return new String(res);
    }

  }
}
