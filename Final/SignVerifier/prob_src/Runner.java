import java.nio.charset.StandardCharsets;
import java.util.Scanner;

class Runner{
  public static void main(String args[]){
    KeyLoader kl = new KeyLoader();
    SignVerifier sv = new SignVerifier(new byte[16], new byte[16], new byte[16]);
    ResultGenerator rg = new ResultGenerator();
    sv.setKey(kl.exec("./signature_verify_key", false).getBytes(StandardCharsets.US_ASCII));
    String input;
    byte [] chal = {0x64,0x6f,0x5f,0x79,0x6f,0x75,0x5f,0x6c,0x6f,0x76,0x65,0x5f,0x6a,0x61,0x76,0x61};

    Scanner s = new Scanner(System.in); 
    System.out.println("[*] give me sign in base64 form");

    input = s.nextLine();

    sv.verify(input);

    System.out.println(rg.exec(new String(sv.getSign()), chal.equals(sv.getVerify())));
    System.out.println(new String(sv.getVerify()));
  }
}
