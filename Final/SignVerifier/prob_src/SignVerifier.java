class SignVerifier{
  byte [] sign;
  byte [] verify;
  byte [] key;

  static{
    System.loadLibrary("Sign");
  }

  SignVerifier(byte [] mSign, byte [] mVerify, byte [] mKey){
    sign = mSign;
    verify = mVerify;
    key = mKey;
  }

  public void verify(String encodedSign){
    decode(encodedSign, sign);
    verify(sign, verify, key);
  }

  public native void decode(String input, byte [] buf);
  public native void verify(byte [] sign, byte [] verify, byte [] key);

  public byte [] getSign(){
    return sign;
  }

  public byte [] getVerify(){
    return verify;
  }

  public void setKey(byte [] mKey){
    System.arraycopy(mKey, 0, key, 0, 16);
  }
}
