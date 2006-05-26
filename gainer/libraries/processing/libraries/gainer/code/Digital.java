package processing.gainer;
import processing.core.*;

import java.util.*;

public class Digital {

	Gainer gainer;
	
  int numInCh = 4;
  int numOutCh = 4; 
  
  public Digital(Gainer gainer){
  	this.gainer = gainer;

  //	System.out.println("Digital init");
  }
  
  public void configuration(int nIn,int nOut){
  	numInCh = nIn;
  	numOutCh = nOut;
  }
  

	public void peek()
	{
		gainer.write("R*");
		gainer.waitForString("R");
	}
	
	public void begin()
	{
		gainer.write("r*");
	}
	
	public void end()
	{
		gainer.write("E*");
		gainer.waitForString("E*");
	}
  
  //全チャンネルに一度に送信
  public void out(int chs){
  	if(chs<=0xFFFF){
			String val = Integer.toHexString(chs).toUpperCase();
			String sv="";
			//必ず4桁
			for(int i=0;i<4-val.length();i++){
				sv += "0";
			}
			sv += val;
			
			String s = "D"+ sv+"*";
			gainer.write(s);
			if(gainer.getVerbose()){
				gainer.waitForString("D");
			}else{
			try{
				Thread.sleep(1);
			}catch(Exception e){
				System.out.println(e);
			}	
		}
		}else{
			throw new IndexOutOfBoundsException("Gainer error!! out of bounds digital port");
		}
  }
  
  public void out(boolean[] values){
  	int chs = 0;
  	if(numOutCh==values.length){
	  	for(int i=0;i<values.length;i++){
	  		if(values[i]){
	  			chs |= (1<<i);
	  		}
	  	}
  	}else{
  		throw new IndexOutOfBoundsException("Gainer error!! out of bounds digital out");
  	}
		String val = Integer.toHexString(chs).toUpperCase();
		String sv="";
		for(int i=0;i<numOutCh-val.length();i++){
			sv += "0";
		}
		sv += val;
			
		String s = "D"+ sv+"*";
		gainer.write(s);
		if(gainer.getVerbose()){
			gainer.waitForString("D");
		}else{
			try{
				Thread.sleep(1);
			}catch(Exception e){
				System.out.println(e);
			}	
		}
  }  
  //指定したチャンネルをHigh
  public void high(int ch){
		if(numOutCh>ch){
			String s = "H" + Integer.toHexString(ch).toUpperCase() + "*";
			gainer.write(s);
			if(gainer.getVerbose()){
				gainer.waitForString("H");
			}else{
			try{
				Thread.sleep(1);
			}catch(Exception e){
				System.out.println(e);
			}	
		}
		}else{
			throw new IndexOutOfBoundsException("Gainer error!! out of bounds digital out");
		}
  }
  //指定したチャンネルをLOW
  public void low(int ch){
		if(numOutCh>ch){
			String s = "L" + Integer.toHexString(ch).toUpperCase() + "*";
			gainer.write(s);
			if(gainer.getVerbose()){
				gainer.waitForString("L");
			}else{
			try{
				Thread.sleep(1);
			}catch(Exception e){
				System.out.println(e);
			}	
		}
		}else{
			throw new IndexOutOfBoundsException("Gainer error!! out of bounds digital out");
		}
  }
  //配列のチャンネルをHigh それ以外はLow
  public void high(int[] chs){
  	byte vch = 0;
  	for(int i=0;i<chs.length;i++){
  		if(numOutCh>chs[i]){
  		  vch |= (1<<chs[i]);
  		}else{
	  		throw new IndexOutOfBoundsException("Gainer error!! out of bounds digital out");
  		}
  	}
  	
 		String val = Integer.toHexString(vch).toUpperCase();
		String sv="";
		for(int i=0;i<numOutCh-val.length();i++){
			sv += "0";
		}
		sv += val;
		 	
		//String sv = val<16 ? "0": "";
		//sv+= Integer.toHexString(val).toUpperCase();
		String s = "D"+ sv+"*";
		gainer.write(s);
		if(gainer.getVerbose()){
			gainer.waitForString("D");
		}else{
			try{
				Thread.sleep(1);
			}catch(Exception e){
				System.out.println(e);
			}	
		}
		
  }
  //配列のチャンネルをlow それ以外はHigh
  public void low(int[] chs){
  	byte vch = 0;
  	byte xor = 0;
  	for(int i=0;i<numOutCh;i++){
  		xor |= (1<<i);
  	}
  	for(int i=0;i<chs.length;i++){
  		if(numOutCh>chs[i]){
  		  vch |= (1<<chs[i]);
  		  
  		}else{
	  		throw new IndexOutOfBoundsException("Gainer error!! out of bounds digital out");
  		}
  	}
  	vch ^= xor;
		String val = Integer.toHexString(vch).toUpperCase();
		String sv="";
		for(int i=0;i<numOutCh-val.length();i++){
			sv += "0";
		}
			sv += val;  	
		//String sv = val<16 ? "0": "";
		//sv+= Integer.toHexString(val).toUpperCase();
		String s = "D"+ sv+"*";
		gainer.write(s);
		if(gainer.getVerbose()){
			gainer.waitForString("D");
		}else{
			try{
				Thread.sleep(1);
			}catch(Exception e){
				System.out.println(e);
			}	
		}
		
  }
  

}
