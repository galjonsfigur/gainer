/**
 * GAINER control libray
 * @author PDP Project
 * @version 1.0.1
 */

package processing.gainer;

import processing.core.*;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;

import java.lang.reflect.*;
import java.util.TooManyListenersException;

import java.io.*;

public final class Gainer {
	
	PApplet parent;
	Method gainerButtonEventMethod;
	Method gainerDigitalInputMethod;
	Method gainerAnalogInputMethod;
	
	//外部から参照します
	
	//ファームのバージョン[1.*]にマッチする
	public static final String libraryVersion = "1.";
	
	public boolean buttonPressed = false;
	public int[] analogInput;
	public boolean[] digitalInput;
  public int[] analogOutput;
  public boolean[] digitalOutput;
  
  public static final int MODE1 = 1;
  public static final int MODE2 = 2;
  public static final int MODE3 = 3;
  public static final int MODE4 = 4;
  public static final int MODE5 = 5;
  public static final int MODE6 = 6;
  public static final int MODE7 = 7;
  public static final int MODE8 = 8;
  
  
	private boolean currentVerbose;
	
  
	private final Client client;
	private SerialTokenizer serialtokenizer=null;
	

	public static boolean DEBUG = false;

	public Gainer(PApplet parent, int mode, boolean verb){
		this.parent = parent;
		
		client = new Client();
		if(client.findGainer()){
			serialtokenizer = new SerialTokenizer(this,client.port);
			initialize(mode,verb);
			

		}else{
			errorMessage("Gainer not found !!");
		}
	}
	
	public Gainer(PApplet parent, boolean verb){
		this(parent,MODE1,verb);
	}
	
	public Gainer(PApplet parent, int mode){
		this(parent,mode,false);
	}
	
	public Gainer(PApplet parent){
		this(parent,MODE1,false);
	}
	
	public Gainer(PApplet parent, String pname,int mode,boolean verb){
		this.parent = parent;
		
		client = new Client();
		if(client.openGainer(pname)){
			serialtokenizer = new SerialTokenizer(this,client.port);
			initialize(mode,verb);
			
		}else{
			errorMessage("Gainer not found !!");
			
		}

	}
	
	public Gainer(PApplet parent, String pname){
		this(parent,pname,MODE1,false);
	}
	
	public Gainer(PApplet parent, String pname, int mode){
		this(parent,pname,mode,false);
	}
	
	public Gainer(PApplet parent, String pname, boolean verb){
		this(parent,pname,MODE1,verb);
	}
	
	public void dispose(){
		reboot();
		client.cleanSerialPort();
	}
	
	//メッセージを表示して終了
	private void errorMessage(String message){
		System.out.println(message);
		System.exit(-1);
	}
	
	//
	private void interpretCode(String code){
		
		StringBuffer codeBuffer = new StringBuffer(code); 
		while(codeBuffer.indexOf("*")!=-1){
			String iCode = codeBuffer.substring(0, codeBuffer.indexOf("*"));
			codeBuffer.delete(0, codeBuffer.indexOf("*")+1);
		
			if(iCode.startsWith("i")||iCode.startsWith("I")){
				
				String value;
				for(int i=0;i<analogInput.length;i++){
					value = iCode.substring(2*i+1,2*(i+1)+1);
					analogInput[i] = Integer.parseInt(value,16);
				}
				
				if(gainerAnalogInputMethod != null){
					try{
						gainerAnalogInputMethod.invoke(parent,new Object[]{ analogInput });
					}catch(Exception e){
						e.printStackTrace();
						gainerAnalogInputMethod = null;
						errorMessage("GainerAnalogInputEvent error!!");
					}
				}
				
			}else if(iCode.startsWith("r")||iCode.startsWith("R")){

				int value = Integer.parseInt(iCode.substring(1,5),16);
				for(int i=0;i<digitalInput.length;i++){
					int c = 1&(value>>i);
					if(c==1){
						digitalInput[i] = true;
					}else{
						digitalInput[i] = false;
					}
				}
				if(gainerDigitalInputMethod != null){
					try{
						gainerDigitalInputMethod.invoke(parent,new Object[]{ digitalInput });
					}catch(Exception e){
						e.printStackTrace();
						gainerDigitalInputMethod = null;
						errorMessage("GainerDigitalInputEvent error!!");
					}
				}

			}else if(iCode.startsWith("N")){
				buttonPressed = true;
				createButtonEvent();
				
			}else if(iCode.startsWith("F")){
				buttonPressed = false;
				createButtonEvent();
			}
		
		
		}

	}
	
	private String execCode(String code, boolean answer){
		String returnCode = "";
		
		try{
			 returnCode = client.sendGainer(code,answer);
			 if(DEBUG){
				 System.out.println("exec Code " + code);
			 }
		}catch(TimeoutException e){
			System.out.println("exec Code " + code + " TIMEOUT!");
		}catch(IOException e){
			errorMessage("I/O error!!");
		}

		return returnCode;
	}
	
	private boolean initialize(int mode, boolean verb){

		parent.registerDispose(this);
		try {
			gainerButtonEventMethod = 
				parent.getClass().getMethod("gainerButtonUpdated",new Class[] { Boolean.TYPE });
		} catch (Exception e) {

		}
		
		try {
			gainerDigitalInputMethod = 
				parent.getClass().getMethod("gainerDigitalInputUpdated",new Class[] { boolean[].class });
		} catch (Exception e) {

		}		

		try {
			gainerAnalogInputMethod = 
				parent.getClass().getMethod("gainerAnalogInputUpdated",new Class[] { int[].class });
		} catch (Exception e) {

		}	
		
		serialtokenizer.printDebugString = DEBUG;
		
		reboot();
		
		if(getDeviceVersion().startsWith(libraryVersion,1)){
			System.out.println("version check. OK");
			setVerbose(verb);
			if(!configuration(mode)){
				errorMessage("configuration error!!");
			}
			try{
				Thread.sleep(100);
			}catch(InterruptedException e){}
			
			System.out.println("get ready!");
			///正しく開いた

			return true;
		}else{
			errorMessage("not match device and library");
		}
		

		return false;
	}
	
	private String getDeviceVersion(){
		return execCode("?*",true);
	}
	
	private void reboot(){
		try{
			Thread.sleep(100);
		}catch(InterruptedException e){}
		
		//long t;
		//t = System.currentTimeMillis();
		//System.out.println("method reboot BEGIN " + t);
		

		execCode("Q*",true);

		
		//t = System.currentTimeMillis();
		//System.out.println("method reboot END " + t);
		
		try{
			Thread.sleep(100);
		}catch(InterruptedException e){}
	}
	
	private void setVerbose(boolean verb){
		currentVerbose = verb;
		String code="";
		if(verb){
			code = "V1*";
			System.out.println("verbose mode");
		}else{
			code = "V0*";
			System.out.println("silent mode");
		}
		execCode(code,true);
	}
	
	private boolean configuration(int mode){
		System.out.println("configuration: MODE" + mode);
		
		analogInput = null;
		digitalInput = null;
		switch(mode){
		case MODE1:
			analogInput = new int[4];
			digitalInput = new boolean[4];
			
			analogOutput = new int[4];
			digitalOutput = new boolean[4];
			
			if(execCode("KONFIGURATION_1*",true).startsWith("KONFIGURATION_1")){
				break;
			}else{
				errorMessage("configuration error");
			}
			return false;
		case MODE2:
			analogInput = new int[8];
			digitalInput = new boolean[0];
			
			analogOutput = new int[4];
			digitalOutput = new boolean[4];
			
			if(execCode("KONFIGURATION_2*",true).startsWith("KONFIGURATION_2")){
				break;
			}else{
				errorMessage("configuration error");
			}
			return false;
		case MODE3:
			analogInput = new int[4];
			digitalInput = new boolean[4];
			
			analogOutput = new int[8];
			digitalOutput = new boolean[0];
			
			if(execCode("KONFIGURATION_3*",true).startsWith("KONFIGURATION_3")){
				break;
			}else{
				errorMessage("configuration error");
			}
			return false;
		case MODE4:
			analogInput = new int[8];
			digitalInput = new boolean[0];
			
			analogOutput = new int[8];
			digitalOutput = new boolean[0];
			
			if(execCode("KONFIGURATION_4*",true).startsWith("KONFIGURATION_4")){
				break;
			}else{
				errorMessage("configuration error");
			}
			return false;
		case MODE5:
			analogInput = new int[0];
			digitalInput = new boolean[16];
			
			analogOutput = new int[0];
			digitalOutput = new boolean[0];
			
			if(execCode("KONFIGURATION_5*",true).startsWith("KONFIGURATION_5")){
				break;
			}else{
				errorMessage("configuration error");
			}
			return false;
		case MODE6:
			analogInput = new int[0];
			digitalInput = new boolean[0];
			
			analogOutput = new int[0];
			digitalOutput = new boolean[16];
			
			if(execCode("KONFIGURATION_6*",true).startsWith("KONFIGURATION_6")){
				break;
			}else{
				errorMessage("configuration error");
			}
			return false;
		case MODE7:
			analogInput = new int[0];
			digitalInput = new boolean[8];
			
			analogOutput = new int[0];
			digitalOutput = new boolean[8];
			
			if(execCode("KONFIGURATION_7*",true).startsWith("KONFIGURATION_7")){
				break;
			}else{
				errorMessage("configuration error");
			}
			return false;
		case MODE8:
			analogInput = new int[0];
			digitalInput = new boolean[0];
			
			analogOutput = new int[8];
			digitalOutput = new boolean[8];
			
			if(execCode("KONFIGURATION_8*",true).startsWith("KONFIGURATION_8")){
				break;
			}else{
				errorMessage("configuration error");
			}
			return false;
		}
		
		return true;
	}
	
	private void createButtonEvent(){
		if(gainerButtonEventMethod != null){
			try{
				gainerButtonEventMethod.invoke(parent,new Object[]{new Boolean(buttonPressed)});
			}catch(Exception e){
				e.printStackTrace();
				gainerButtonEventMethod = null;
				errorMessage("GainerButton error!!");
			}
		}
	}
	
	
	
	
	public void turnOnLED(){
		execCode("h*",currentVerbose);
	}
	
	public void turnOffLED(){
		execCode("l*",currentVerbose);
	}
	
	public void peekDigitalInput(){
		interpretCode(execCode("R*",false));
	}
	
	public void beginDigitalInput(){
		execCode("r*",false);
	}
	
	public void endDigitalInput(){
		execCode("E*",true);
	}
	
	//全チャンネルに一度に送信
	//0x0000 - 0xFFFF
	public boolean digitalOutput(int channels){
		if(channels <=0xFFFF && channels >=0){
			String val = Integer.toHexString(channels).toUpperCase();
			String sv="";
			//必ず4桁
			for(int i=0;i<4-val.length();i++){
				sv += "0";
			}
			sv += val;
			
			String code = "D"+ sv +"*";
			execCode(code,currentVerbose);
		
//		冗長かな？
			for(int i=0;i<digitalOutput.length;i++){
				int a = (channels>>i) & 1;
				digitalOutput[i] = a==1 ? true:false;
			}
			
			return true;
		}
		return false;
	}
	
	public boolean digitalOutput(boolean[] values){
		int channels = 0;
  	if(digitalOutput.length==values.length){
	  	for(int i=0;i<values.length;i++){
	  		if(values[i]){
	  			channels |= (1<<i);
	  		}
//	  	冗長かな？
	  		digitalOutput[i] = values[i] ? true:false;
	  	}
  	}else{
  		return false;
  	}
		String val = Integer.toHexString(channels).toUpperCase();
		String sv="";
		for(int i=0;i<digitalOutput.length-val.length();i++){
			sv += "0";
		}
		sv += val;
			
		String code = "D"+ sv +"*";
		execCode(code,currentVerbose);
		return true;

	}
	
	//指定したチャンネルをHigh
	public boolean setHigh(int channel){
		if(digitalOutput.length>channel){
			String code = "H" + Integer.toHexString(channel).toUpperCase() + "*";
			execCode(code,currentVerbose);
//  	冗長かな？
  		digitalOutput[channel] = true;
  		
			return true;
		}		
		return false;
	}
	
	//配列のチャンネルをHigh それ以外はLow
	public boolean setHigh(int[] channels){
  	byte vch = 0;
  	for(int i=0;i<channels.length;i++){
  		if(digitalOutput.length>channels[i]){
  		  vch |= (1<<channels[i]);
  		}else{
	  		return false;
  		}
//  	冗長かな？
  		digitalOutput[channels[i]] = true;
  	}
  	
 		String val = Integer.toHexString(vch).toUpperCase();
		String sv="";
		for(int i=0;i<digitalOutput.length-val.length();i++){
			sv += "0";
		}
		sv += val;
		 	
		//String sv = val<16 ? "0": "";
		//sv+= Integer.toHexString(val).toUpperCase();
		String code = "D"+ sv +"*";
		execCode(code,currentVerbose);
		return true;
	}
	
	//指定したチャンネルをLOW
	public boolean setLow(int channel){
		if(digitalOutput.length>channel){
			String code = "L" + Integer.toHexString(channel).toUpperCase() + "*";
			execCode(code,currentVerbose);
			
//  	冗長かな？
			digitalOutput[channel] = false;
			return true;
		}
		return false;
	}
	
	//配列のチャンネルをlow それ以外はHigh
	public boolean setLow(int[] channels){
  	byte vch = 0;
  	for(int i=0;i<channels.length;i++){
  		if(digitalOutput.length>channels[i]){
  		  vch |= (1<<channels[i]);
  		}else{
	  		return false;
  		}
//  	冗長かな？
  		digitalOutput[channels[i]] = false; 		
  	}
  	
 		String val = Integer.toHexString(vch).toUpperCase();
		String sv="";
		for(int i=0;i<digitalOutput.length-val.length();i++){
			sv += "0";
		}
		sv += val;
		 	
		//String sv = val<16 ? "0": "";
		//sv+= Integer.toHexString(val).toUpperCase();
		String code = "D"+ sv +"*";
		execCode(code,currentVerbose);
		return true;
	}
	
	public void beginAnalogInput(){
		execCode("i*",false);
	}
	
	public void endAnalogInput(){
		execCode("E*",true);
	}
	
	public void peekAnalogInput(){
		interpretCode(execCode("I*",false));
	}
	
	//指定チャンネルに値を出力
	//値は0-255に丸める
	public boolean analogOutput(int ch,int value){
		if(ch>=analogInput.length ){
			return false;
		}
		String code = "a" + Integer.toHexString(ch).toUpperCase();
		value = value<  0 ?   0: value;
		value = value>255 ? 255: value;
//	冗長かな？
		analogOutput[ch] = value;
		
		
		String sv = value<16 ? "0": "";
		sv+= Integer.toHexString(value).toUpperCase();
		code+=sv;
		code+="*";
		
		execCode(code,currentVerbose);

		return true;

	}
	
	//配列を対応するチャンネルにそれぞれ出力
	public boolean analogOutput(int[] values){
  	String code = "A";
  	String sv="";
		if(analogOutput.length==values.length){
			for(int i=0;i<values.length;i++){
				values[i] = values[i]<  0 ?   0: values[i];
				values[i] = values[i]>255 ? 255: values[i];
				analogOutput[i] = values[i];
				
				sv = values[i]<16 ? "0": "";
				sv+= Integer.toHexString(values[i]).toUpperCase();
				code+=sv;
			}
		  code+= "*";
		}else{
  		return false;
		}
		
		execCode(code,currentVerbose);
		
		return true;
	}
	
	public void analogSamplingAllChannels(){
		execCode("M0*",true);
	}
	
	public void analogSamplingSingleChannel(){
		execCode("M1*",true);
	}
	
	public boolean ampGainAGND(int gain){
		String code = "G";
		if(gain>=0 && gain<16){
			code += Integer.toHexString(gain).toUpperCase();
			code += "1*";
			execCode(code,true);
			return true;
		}
		return false;
	}
	
	public boolean ampGainDGND(int gain){
		String code = "G";
		if(gain>=0 && gain<16){
			code += Integer.toHexString(gain).toUpperCase();
			code += "0*";
			execCode(code,true);
			
			return true;
		}
		return false;	
	}
	
	
	
	
	
	//
	//
	//
	//
	public class SerialTokenizer
		implements SerialPortEventListener{
		
		private Gainer gainer;
		
		private SerialPort port;
		private InputStream input;

		
		private StringBuffer sReturnCodes;


		public boolean printDebugString = false;
		
		public SerialTokenizer(Gainer gainer,SerialPort port){
			sReturnCodes = new StringBuffer();
			this.port = port;
			
			this.gainer = gainer;
			
			try{
				input = this.port.getInputStream();
		
				this.port.addEventListener(this);
			}catch(IOException e){
				
			}catch(TooManyListenersException e){
				//Listeneつけすぎ　致命的
				System.out.println("TooManyListeners");
			}
			
	    this.port.notifyOnDataAvailable(true);

		}

		public synchronized void serialEvent(SerialPortEvent serialEvent){
			if(serialEvent.getEventType()==SerialPortEvent.DATA_AVAILABLE){
				try{
					while(input.available() > 0){
						putReturnCode();
					}
					
				}catch(IOException e){
					//よみこみエラー
					
					System.out.println("serialEvent read error ");
				}
				
			}
		}
		
		public synchronized void clean(){
			port.removeEventListener();
			try{
				input.close();
			}catch(IOException e){}
			
			input = null;
		}
		

		
		public synchronized String getReturnCode(){
//			System.out.println("getReturnCode BEGIN  " + Thread.currentThread().getName());
			
//			long start = System.currentTimeMillis();
//			while(sReturnCodes.indexOf("*") == -1){
//				//System.out.println("wait " + timeout + "  " + Thread.currentThread().getName() );
//				if(timeout==0){
//					wait(0);
//					return "";
//				}else{
//					long now = System.currentTimeMillis();
//					long rest = timeout - (now-start);
//					if(rest <= 0){
//						System.out.println("timeout return__buffer[" + sReturnCodes + "] " + sReturnCodes.indexOf("*")  + Thread.currentThread().getName() );
//						throw new TimeoutException("TimeoutException!! " + (now - start));
//					}
//					wait(rest);
//				}
//			}
//			
			
			String returnCode = sReturnCodes.substring(0, sReturnCodes.lastIndexOf("*")+1);
			sReturnCodes.delete(0, sReturnCodes.lastIndexOf("*")+1);
			
//			while(sReturnCodes.indexOf("*")!=-1){
//				returnCode = sReturnCodes.substring(0, sReturnCodes.indexOf("*")+1);
//				sReturnCodes.delete(0, sReturnCodes.indexOf("*")+1);
//			}
			
//			System.out.println("getReturnCode END");
			return returnCode;
		}
		
		public synchronized void putReturnCode(){
//			System.out.println("putReturnCode BEGIN  " + Thread.currentThread().getName());	
			try{
				byte temp[] = new byte[64];
				int read = input.read(temp);

				sReturnCodes.append(new String(temp,0,read));

				}catch(IOException e){
//			よみこみエラー
			}
			int indexStar = sReturnCodes.indexOf("*");
			if(indexStar != -1){
				//notifyAll();
				
				if(printDebugString){
					System.out.println("printDebugString returnbuffer [" + sReturnCodes+ "] " + Thread.currentThread().getName());
				}
				
				gainer.interpretCode(getReturnCode());
			}
			
//			System.out.println("putReturnCode END");
		}
		
		public synchronized void clearReturnCode(){
			int indexStar = sReturnCodes.indexOf("*");
		
			while(indexStar != -1){
				System.out.println("clearReturnCode : " + sReturnCodes);
				sReturnCodes.delete(0, indexStar+1);
				indexStar = sReturnCodes.indexOf("*");
			}
			
		}

		
	}


	
	
}
