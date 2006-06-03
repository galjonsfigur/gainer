/**
 * GAINER control libray
 * @author PDP Project
 * @version 1.0
 */

/*
 firmware updateの時
1 firmVersionの文字を書き換える
*/

package processing.gainer;
import processing.core.*;

import gnu.io.*;

import java.io.*;
import java.util.*;
import java.lang.reflect.*;


public class Gainer implements SerialPortEventListener {

  PApplet parent;
  Method serialEventMethod;
  
  Method gainerButtonEventMethod;
  //Method gainerAnalogInEventMethod;
  

  public SerialPort port;
  
  private Digital digital;
  private Analog analog;
  
  private boolean WINDOWS = true;
  
  public final String libVersion = "1.0.0";
  public String deviceVersion;

  private int currentMode = 0;
  public final static int MODE1 = 1;
  public final static int MODE2 = 2;
  public final static int MODE3 = 3;
  public final static int MODE4 = 4;
  public final static int MODE5 = 5;
  public final static int MODE6 = 6;
  public final static int MODE7 = 7;
  public final static int MODE8 = 8;

  public final static boolean AGND = true;
  //public final static boolean V2_5 = true;
  public final static boolean GND = false;
  public final static boolean DGND = false;
  //public final static boolean V0_0 = false;

  private final int rate=38400;
  private final int parity=SerialPort.PARITY_NONE;
  private final int databits=8;
  private final int stopbits=SerialPort.STOPBITS_1;

  public boolean buttonPressed=false;
  public int[] analogInput;
  public boolean[] digitalInput;

	private static boolean verbose;

  private boolean analogSeq = false;
  private boolean digitalSeq = false;
	
  private String sReturn;
  private final int TIMEOUT = 100;

  public InputStream input;
  public OutputStream output;

  //byte buffer[] = new byte[32768];
  byte buffer[] = new byte[64];
  int bufferIndex;
  int bufferLast;

  int bufferSize = 1;  // how big before reset or event firing

  static private boolean DEBUG = false;
  private static String dname = "COM3";
  private static int dmode = MODE1;
  


	//名前を指定しない初期化
  public Gainer(PApplet parent) {
    this(parent,dmode,false);
  }
  
  public Gainer(PApplet parent,boolean verb){
  	this(parent,dmode,verb);
  }
  
  public Gainer(PApplet parent,int mode){
  	this(parent,mode,false);
  }
  
  //名前を指定しない初期化 mode指定
  public Gainer(PApplet parent,int mode,boolean verb){
    try {
      Enumeration portList = CommPortIdentifier.getPortIdentifiers();
      while (portList.hasMoreElements()){
        CommPortIdentifier portId =(CommPortIdentifier) portList.nextElement();
        if (portId.getPortType() == CommPortIdentifier.PORT_SERIAL) {
          //System.out.println("found name  " + portId.getName());
          String pname = portId.getName();
          if(pname.startsWith("/dev/cu.usbserial-")){
            WINDOWS = false;
            dname = pname;
            
          }else if(pname.startsWith("COM")){
            WINDOWS = true;
          }
        }
      }
    }catch (Exception e) {
      errorMessage("<get serial name>", e);
      e.printStackTrace();
      
      port = null;
      input = null;
      output = null;
    }
    
    initialize(parent,dname,mode,verb);
  }

  public Gainer(PApplet parent,String iname){
    this(parent, iname ,dmode,false);
  }
	

  public Gainer(PApplet parent,String iname,int mode) {
    initialize(parent,iname,mode,false);
  }
  
  public Gainer(PApplet parent,String iname,boolean verb){
  	initialize(parent,iname,dmode,verb);
  }
  
  public Gainer(PApplet parent,String iname,int mode,boolean verb) {
    initialize(parent,iname,mode,verb);
  }
	
	private void initialize(PApplet parent, String iname,int mode,boolean verb){

    
    this.parent = parent;
    //parent.attach(this);

    digital = new Digital(this);
    analog = new Analog(this);

    
    try {
      Enumeration portList = CommPortIdentifier.getPortIdentifiers();
      System.out.println(" ");
      System.out.println("selected port name "+iname);
      
      while (portList.hasMoreElements()) {
        CommPortIdentifier portId =
          (CommPortIdentifier) portList.nextElement();

        if (portId.getPortType() == CommPortIdentifier.PORT_SERIAL) {
          //System.out.println("found name" + portId.getName());
          if (portId.getName().equals(iname)) {
            System.out.println("serial port opening ...");
            
            port = (SerialPort)portId.open("serial gainer", 2000);
            input = port.getInputStream();
            output = port.getOutputStream();
            port.setSerialPortParams(rate, databits, stopbits, parity);
            port.addEventListener(this);
            port.notifyOnDataAvailable(true);
            
            
            
          }
        }
      }

    } catch (Exception e) {
    	System.out.println("initialize error "+e);
      errorMessage("<init>", e);
      e.printStackTrace();
      port = null;
      input = null;
      output = null;
    }

    parent.registerDispose(this);

     try {

      gainerButtonEventMethod = 
        parent.getClass().getMethod("gainerButtonEvent",
                                    new Class[] { Boolean.TYPE });
    } catch (Exception e) {
      // no such method, or an error.. which is fine, just ignore
    }
    
    buffer(64);
    reboot();
    
    getDeviceVersion();
    verbose = verb;
    setVerbose(verbose);
    configuration(mode);
    currentMode = mode;
    
    System.out.println("Gainer ready to Roll. ");
	}

	private void getDeviceVersion(){
		this.write("?*");
		try{
		Thread.sleep(100);
		}catch(Exception e){
			System.out.println(e);
		}
		deviceVersion = sReturn.substring(1,9);
		System.out.println("Gainer firmware ver. " + deviceVersion);
		deviceVersion = deviceVersion.substring(0,5);
		System.out.println("dev:"+ deviceVersion + " lib:" + libVersion);
		//if(!libVersion.startsWith(deviceVersion) ){
		//	throw new RuntimeException("Gainer error!! please check firmware version");
		//}	
	}

	private void setVerbose(boolean verb){
		String c;
		if(verb){
			c = "V1*";
			System.out.println("verbose mode");
		}else{
			c = "V0*";
			System.out.println("silent mode");
		}
		this.write(c);

		try{
		Thread.sleep(100);
		}catch(Exception e){
			System.out.println(e);
		}
	}
	
	public boolean getVerbose(){
		return verbose;
	}
	
	private void configuration(int mode){
		System.out.println("configuration: mode"+mode);
//     ain :din :aout:dout
//     ----:----:----:----
// C0:    0:   0:   0:   0
// C1:    4:   4:   4:   4
// C2:    8:   0:   4:   4
// C3:    4:   4:   8:   0
// C4:    8:   0:   8:   0	←追加
// C5:    0:  16:   0:   0	←以降一個ずつ後ろに
// C6:    0:   0:   0:  16
// C7: LED matrix control???	←予定は未定（ぼちぼちと）
// C8: CapSense???		←予定は未定（ぼちぼちと）
// C9: servo control???		←予定は未定（ぼちぼちと）
		
		analogInput = null;
		digitalInput = null;
		switch(mode){
		case MODE1:
			analogInput = new int[4];
			digitalInput = new boolean[4];
			digital.configuration(4,4);
			analog.configuration(4,4);
			
			this.write("KONFIGURATION_1*");
			if(!waitForString("KONFIGURATION_1")){
				throw new RuntimeException("Gainer error!! please check coneecting to gainer");
			}
			
			break;
		case MODE2:
			analogInput = new int[8];
			digitalInput = new boolean[0];
			digital.configuration(0,4);
			analog.configuration(8,4);
			
			this.write("KONFIGURATION_2*");
			if(!waitForString("KONFIGURATION_2")){
				throw new RuntimeException("Gainer error!! please check coneecting to gainer");
			}
			break;
		case MODE3:
			analogInput = new int[4];
			digitalInput = new boolean[4];
			digital.configuration(4,0);
			analog.configuration(4,8);
			
			this.write("KONFIGURATION_3*");
			if(!waitForString("KONFIGURATION_3")){
				throw new RuntimeException("Gainer error!! please check coneecting to gainer");
			}
			break;
		case MODE4:
			analogInput = new int[8];
			digitalInput = new boolean[0];
			digital.configuration(0,0);
			analog.configuration(8,8);
			
			this.write("KONFIGURATION_4*");
			if(!waitForString("KONFIGURATION_4")){
				throw new RuntimeException("Gainer error!! please check coneecting to gainer");
			}
			break;
		case MODE5:
			analogInput = new int[0];
			digitalInput = new boolean[16];
			digital.configuration(16,0);
			analog.configuration(0,0);
			
			this.write("KONFIGURATION_5*");
			if(!waitForString("KONFIGURATION_5")){
				throw new RuntimeException("Gainer error!! please check coneecting to gainer");
			}
			break;
		case MODE6:
			analogInput = new int[0];
			digitalInput = new boolean[0];
			digital.configuration(0,16);
			analog.configuration(0,0);
			
			this.write("KONFIGURATION_6*");
			if(!waitForString("KONFIGURATION_6")){
				throw new RuntimeException("Gainer error!! please check coneecting to gainer");
			}		
			break;

		case MODE7:
			analogInput = new int[0];
			digitalInput = new boolean[0];
			digital.configuration(0,0);
			analog.configuration(0,0);
			
			this.write("KONFIGURATION_7*");
			if(!waitForString("KONFIGURATION_7")){
				throw new RuntimeException("Gainer error!! please check coneecting to gainer");
			}	
			break;
			
		}
			try{
			Thread.sleep(1000);
			}catch(Exception e){
				System.out.println(e);
			}

	}


  /**
   * Used by PApplet to shut things down.
   */
  public void dispose() {
  	System.out.println("dispose Gainer ..");
  	this.write("E*");
  
    try {
      // do io streams need to be closed first?
      if (input != null) input.close();
      if (output != null) output.close();

    } catch (Exception e) {
      e.printStackTrace();
    }
    input = null;
    output = null;

    try {
      if (port != null) port.close();  // close the port

    } catch (Exception e) {
      e.printStackTrace();
    }
    port = null;
  }


  synchronized public void serialEvent(SerialPortEvent serialEvent) {
    if (serialEvent.getEventType() == SerialPortEvent.DATA_AVAILABLE) {
      try {
        while (input.available() > 0) {
          synchronized (buffer) {
            if (bufferLast == buffer.length) {
              byte temp[] = new byte[bufferLast << 1];
              System.arraycopy(buffer, 0, temp, 0, bufferLast);
              buffer = temp;
            }
            buffer[bufferLast++] = (byte) input.read();
            
            
            if(buffer[bufferLast-1]=='*'){
            	sReturn = readStringUntil('*');
            	clear();
            }else{
            	continue;
            }
            
            if(DEBUG){
            	System.out.println("gainer-> " + sReturn);
            }
            
            checkReturnCode(sReturn);
            
          }
        }

      } catch (IOException e) {
        errorMessage("serialEvent", e);
      }
    }
  }


  /**
   */
  private void buffer(int count) {
    bufferSize = count;
  }




  /**
   */
  private int available() {
    return (bufferLast - bufferIndex);
  }


  /**
   */
  private void clear() {
    bufferLast = 0;
    bufferIndex = 0;
  }






  /**
   * Return a byte array of anything that's in the serial buffer.
   * Not particularly memory/speed efficient, because it creates
   * a byte array on each read, but it's easier to use than
   * readBytes(byte b[]) (see below).
   */
  private byte[] readBytes() {
    if (bufferIndex == bufferLast) return null;

    synchronized (buffer) {
      int length = bufferLast - bufferIndex;
      byte outgoing[] = new byte[length];
      System.arraycopy(buffer, bufferIndex, outgoing, 0, length);

      bufferIndex = 0;  // rewind
      bufferLast = 0;
      return outgoing;
    }
  }


  /**
   * Grab whatever is in the serial buffer, and stuff it into a
   * byte buffer passed in by the user. This is more memory/time
   * efficient than readBytes() returning a byte[] array.
   *
   * Returns an int for how many bytes were read. If more bytes
   * are available than can fit into the byte array, only those
   * that will fit are read.
   */
  private int readBytes(byte outgoing[]) {
    if (bufferIndex == bufferLast) return 0;

    synchronized (buffer) {
      int length = bufferLast - bufferIndex;
      if (length > outgoing.length) length = outgoing.length;
      System.arraycopy(buffer, bufferIndex, outgoing, 0, length);

      bufferIndex += length;
      if (bufferIndex == bufferLast) {
        bufferIndex = 0;  // rewind
        bufferLast = 0;
      }
      return length;
    }
  }


  /**
   * Reads from the serial port into a buffer of bytes up to and
   * including a particular character. If the character isn't in
   * the serial buffer, then 'null' is returned.
   */
  private byte[] readBytesUntil(int interesting) {
    if (bufferIndex == bufferLast) return null;
    byte what = (byte)interesting;

    synchronized (buffer) {
      int found = -1;
      for (int k = bufferIndex; k < bufferLast; k++) {
        if (buffer[k] == what) {
          found = k;
          break;
        }
      }
      if (found == -1) return null;

      int length = found - bufferIndex + 1;
      byte outgoing[] = new byte[length];
      System.arraycopy(buffer, bufferIndex, outgoing, 0, length);

      bufferIndex = 0;  // rewind
      bufferLast = 0;
      return outgoing;
    }
  }


  /**
   * Reads from the serial port into a buffer of bytes until a
   * particular character. If the character isn't in the serial
   * buffer, then 'null' is returned.
   *
   * If outgoing[] is not big enough, then -1 is returned,
   *   and an error message is printed on the console.
   * If nothing is in the buffer, zero is returned.
   * If 'interesting' byte is not in the buffer, then 0 is returned.
   */
  private int readBytesUntil(int interesting, byte outgoing[]) {
    if (bufferIndex == bufferLast) return 0;
    byte what = (byte)interesting;

    synchronized (buffer) {
      int found = -1;
      for (int k = bufferIndex; k < bufferLast; k++) {
        if (buffer[k] == what) {
          found = k;
          break;
        }
      }
      if (found == -1) return 0;

      int length = found - bufferIndex + 1;
      if (length > outgoing.length) {
        System.err.println("readBytesUntil() byte buffer is" +
                           " too small for the " + length +
                           " bytes up to and including char " + interesting);
        return -1;
      }
      //byte outgoing[] = new byte[length];
      System.arraycopy(buffer, bufferIndex, outgoing, 0, length);

      bufferIndex += length;
      if (bufferIndex == bufferLast) {
        bufferIndex = 0;  // rewind
        bufferLast = 0;
      }
      return length;
    }
  }


  /**
   * Return whatever has been read from the serial port so far
   * as a String. It assumes that the incoming characters are ASCII.
   *
   * If you want to move Unicode data, you can first convert the
   * String to a byte stream in the representation of your choice
   * (i.e. UTF8 or two-byte Unicode data), and send it as a byte array.
   */
  private String readString() {
    if (bufferIndex == bufferLast) return null;
    return new String(readBytes());
  }


  /**
   * Combination of readBytesUntil and readString. See caveats in
   * each function. Returns null if it still hasn't found what
   * you're looking for.
   *
   * If you want to move Unicode data, you can first convert the
   * String to a byte stream in the representation of your choice
   * (i.e. UTF8 or two-byte Unicode data), and send it as a byte array.
   */
  private String readStringUntil(int interesting) {
    byte b[] = readBytesUntil(interesting);
    if (b == null) return null;
    return new String(b);
  }




  private void write(byte bytes[]) {
	  sReturn="";
    try {
      output.write(bytes);
      //output.flush();   // hmm, not sure if a good idea
    } catch (Exception e) { // null pointer or serial port dead
      System.out.println("- exception - please check connecting to gainer");
      errorMessage("write", e);
      e.printStackTrace();
      
    }
  }


  /**
   * Write a String to the output. Note that this doesn't account
   * for Unicode (two bytes per char), nor will it send UTF8
   * characters.. It assumes that you mean to send a byte buffer
   * (most often the case for networking and serial i/o) and
   * will only use the bottom 8 bits of each char in the string.
   * (Meaning that internally it uses String.getBytes)
   *
   * If you want to move Unicode data, you can first convert the
   * String to a byte stream in the representation of your choice
   * (i.e. UTF8 or two-byte Unicode data), and send it as a byte array.
   */
  public void write(String what) {
  	if(DEBUG){
  		System.out.println(what + "-> gainer");
  	}
    write(what.getBytes());
  }


  /**
   * If this just hangs and never completes on Windows,
   * it may be because the DLL doesn't have its exec bit set.
   * Why the hell that'd be the case, who knows.
   */
  static public String[] list() {
    Vector list = new Vector();
    try {
      //System.err.println("trying");
      Enumeration portList = CommPortIdentifier.getPortIdentifiers();
      //System.err.println("got port list");
      while (portList.hasMoreElements()) {
        CommPortIdentifier portId =
          (CommPortIdentifier) portList.nextElement();
        if (portId.getPortType() == CommPortIdentifier.PORT_SERIAL) {
          String name = portId.getName();
          list.addElement(name);
          
        }
      }

    } catch (UnsatisfiedLinkError e) {
      System.err.println("1");
      errorMessage("ports", e);

    } catch (Exception e) {
      System.err.println("2");
      errorMessage("ports", e);
    }
    //System.err.println("move out");
    String outgoing[] = new String[list.size()];
    list.copyInto(outgoing);
    return outgoing;
  }


  /**
   * General error reporting, all corraled here just in case
   * I think of something slightly more intelligent to do.
   */
  static public void errorMessage(String where, Throwable e) {
    e.printStackTrace();
    throw new RuntimeException("Error inside Serial." + where + "()");
  }

	//戻り値のあるやつはそれぞれ判断する
	private void checkReturnCode(String s){
	
            if(s.startsWith("i") || s.startsWith("I")){
            
            	String value;
            	
            	for(int i=0;i<analogInput.length;i++){
            		value = s.substring(2*i+1,2*(i+1)+1);
            		analogInput[i] = Integer.parseInt(value,16);
            	}
            	
            }else if(s.startsWith("r") || s.startsWith("R")){
            	int value = Integer.parseInt(s.substring(1,5),16);
            	for(int i=0;i<digitalInput.length;i++){
            		int c = 1&(value>>i);
            		if(c==1){
            			digitalInput[i] = true;
            		}else{
            			digitalInput[i] = false;
            		}
            	}
            	
            }else if(s.startsWith("N")){
              buttonPressed = true;
            }else if(sReturn.startsWith("F")){
              buttonPressed = false;
            }
            //これら以外の戻りはおのおのの指定したwaitForで待つ
/*
            if(gainerAnalogInEventMethod != null){
            	if(s.startsWith("i")){
            		try{
            			Object[] args = 
            			gainerAnalogInEventMethod.invoke(parent,new Object[]{new Void()});
            		}catch(Exception e){
                  String msg = "error, disabling gainerAnalogInEvent() for " + port;
                  System.err.println(msg);
                  e.printStackTrace();
                  gainerAnalogInEventMethod = null;
                }
            	}
            }
*/
            if(gainerButtonEventMethod != null){
              if(s.startsWith("F") || s.startsWith("N") ){
                try{
                  gainerButtonEventMethod.invoke(parent,new Object[]{new Boolean(buttonPressed)});
                  //System.out.println("make ganerSWEvent "+ buttonPressed);
                }catch(Exception e){
                  String msg = "error, disabling gainerSWEvent() for " + port;
                  System.err.println(msg);
                  e.printStackTrace();
                  gainerButtonEventMethod = null;
                }
              }
            }
	}

	public boolean waitForString(String sRet){
		int count = TIMEOUT;
		//
		while(!sReturn.startsWith(sRet) && count>0){
			count--;
			if(sReturn.startsWith("!*")){
				//System.out.println("gainer wrong return code error!!" );
				return false;
			}else{
			//待ちの間に戻ってきた想定以外の値
				if(sReturn != ""){
					checkReturnCode(sReturn);
					return true;
				}
			}
			
			try{
			Thread.sleep(10);
			}catch(Exception e){
				System.out.println(e);
			}
		}
		
		if(!sReturn.startsWith(sRet)){
			System.out.print("wait for  " + sRet + " recv:" + sReturn);
			System.out.println("   gainer TIMEOUT return code error!!" );
			return false;
		}
		return true;
	}
	
  public void reboot(){
  	this.write("Q*");
			try{
			Thread.sleep(1000);
			}catch(Exception e){
				System.out.println(e);
			}  	
		if(sReturn.indexOf("Q*")==-1){
			throw new RuntimeException("Reboot Error !!!");
		}else{
			System.out.println("Gainer reboot.. ");
		}
		

  }
  
	public void turnOnLED(){

		this.write("h*");
		if(getVerbose()){
			waitForString("h");
		}else{
			try{
				Thread.sleep(1);
			}catch(Exception e){
				System.out.println(e);
			}  	
		}
	}
	
	public void turnOffLED(){
		
		this.write("l*");
		if(getVerbose()){
			waitForString("l");
		}else{
			try{
				Thread.sleep(1);
			}catch(Exception e){
				System.out.println(e);
			}	
		}
	}
	
	public void peekDigitalInput(){
		digital.peek();
	}
	
	public void beginDigitalInput(){
		digital.begin();
	}
	public void endDigitalInput(){
		digital.end();
	}
	
	//全チャンネルに一度に送信
	public void digitalOutput(int chs){
		digital.out(chs);

	}
	
	public void digitalOutput(boolean[] values){
		digital.out(values);
		clear();
	}
	
	//指定したチャンネルをHigh
	public void setHigh(int ch){
		digital.high(ch);
	}
	
	//配列のチャンネルをHigh それ以外はLow
	public void setHigh(int[] chs){
		digital.high(chs);
	}
	
	//指定したチャンネルをLOW
	public void setLow(int ch){
		digital.low(ch);
	}
	
	//配列のチャンネルをlow それ以外はHigh
	public void setLow(int[] chs){
		digital.low(chs);
	}
	
	public void beginAnalogInput(){
		analog.begin();
		analogSeq = true;
	}
	
	public void endAnalogInput(){
		analogSeq = false;
		analog.end();
	}
	
	public void peekAnalogInput(){
		analog.peek();
	}
	
	public void analogOutput(int ch,int value){
		analog.out(ch,value);
	}
	
	
	public void analogOutput(int[] values){
		analog.out(values);
	}
	
	
	public void analogSamplingAllChannels(){
	
		this.write("M0*");
		waitForString("M0*");
	}
	public void analogSamplingSingleChannel(){
		this.write("M1*");
		waitForString("M1*");
		
	}
	
	//mode7のみ
	//line毎に処理
	public void scanLine(int line,int[] values){
		if(currentMode==MODE7){
			String s = "a";
			String sv = "";
			if(values.length == 8){
				if(line<8){
					s += Integer.toHexString(line).toUpperCase();
				}else{
					throw new RuntimeException("Gainer error!! out of bounds");
				}
				for(int i=0;i<8;i++){
					sv = values[i]<16 ? "0": "";
					s += sv;
					s += Integer.toHexString(values[i]).toUpperCase();
				}
				s += "*";
				write(s);
	 			waitForString("a");
			}else{
				throw new RuntimeException("Gainer error!! number of values");
			}
		}else{
			throw new RuntimeException("Gainer error!! this method can use only MODE7");
		}
	}
	//mode7のみ
	//LEDmatrix全体を処理
	public void scanMatrix(int[] values){
		if(currentMode==MODE7){
			if(values.length == 64){
				for(int col=0;col<8;col++){
					int[] v = new int[8];
					for(int i=0;i<8;i++){
						v[i] = values[col*8+i];
					}
					scanLine(col,v);
				}
			}
		}else{
			throw new RuntimeException("Gainer error!! this method can use only MODE7");
		}
	}
	
	
	
	
	public void ampGainAGND(int gain){
	 String s ="G";
	 
	 if(gain>=0 && gain<16){
	  s += Integer.toHexString(gain).toUpperCase();
  	s+="1";
	  
	 }else{
	 	throw new RuntimeException("Gainer error!! gain");
	 }
	 s +="*";
	 write(s);
	 waitForString("G");
	
	}
	
	


	public void ampGainDGND(int gain){
	 String s ="G";
	 
	  if(gain>=0 && gain<16){
	   s += Integer.toHexString(gain).toUpperCase();
	   s+="0";
	  }else{
	  	throw new RuntimeException("Gainer error!! gain");
	  }
	  s +="*";
	  write(s);
	  waitForString("G");
	
	}	
	
	static public void DEBUG(){
		DEBUG = true;
	}
}


