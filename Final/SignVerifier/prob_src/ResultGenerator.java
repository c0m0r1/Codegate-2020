class ResultGenerator{

  public String exec(String data, boolean isVerified){
    if(!isVerified){
      return "[-] verify fail : " + data;
    }
    else{
      return "[+] verify success : " + data;
    }
  }
}
