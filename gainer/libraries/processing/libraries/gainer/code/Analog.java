package processing.gainer;
import processing.core.*;

import java.util.*;

public class Analog{

	Gainer gainer;

  int numInCh = 4;
  int numOutCh = 4; 
  
  public Analog(Gainer gainer){
		this.gainer = gainer;
  }
  
  public void configuration(int nIn,int nOut){
  	numInCh = nIn;
  	numOutCh = nOut;
  }
  //アナログ受信開始
  public void begin(){
  	gainer.write("i*");
  }
 
  //アナログ受信終了
  public void end(){
  	gainer.write("E*");
  	gainer.waitForString("E*");
  }
  
  public void peek(){
  	gainer.write("I*");
  	gainer.waitForString("I");
  }
  
  //指定したチャンネルへ送信
  public void out(int ch,int value){
  	if(numOutCh>ch){
  		String s = "a" + Integer.toHexString(ch).toUpperCase();
  		value = value<  0 ?   0: value;
			value = value>255 ? 255: value;
				
  		String sv = value<16 ? "0": "";
			sv+= Integer.toHexString(value).toUpperCase();
			s+=sv;
			s+="*";
			gainer.write(s);
			if(gainer.getVerbose()){
				gainer.waitForString("a");//*はこないよ！
			}else{
				try{
					Thread.sleep(1);
				}catch(Exception e){
					System.out.println(e);
				}	
			}
  	}else{
  		throw new IndexOutOfBoundsException("Gainer error!! out of bounds analog out");
  	}
  }
  
  public void out(int[] values){
  	String s = "A";
  	String sv="";
		if(numOutCh==values.length){
			for(int i=0;i<values.length;i++){
				values[i] = values[i]<  0 ?   0: values[i];
				values[i] = values[i]>255 ? 255: values[i];
				sv = values[i]<16 ? "0": "";
				sv+= Integer.toHexString(values[i]).toUpperCase();
				s+=sv;
			}
		  s+= "*";
		}else{
  		throw new IndexOutOfBoundsException("Gainer error!! - number of analog outputs are wrong");
		}
		gainer.write(s);
		if(gainer.getVerbose()){
			gainer.waitForString("A");
		}else{
			try{
				Thread.sleep(1);
			}catch(Exception e){
				System.out.println(e);
			}	
		}
		
  }

}