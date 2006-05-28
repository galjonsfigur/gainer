/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

import pdp.*;

class pdp.Analog {
	private var gainer:Gainer;
	public var numInCh:Number = 4;
	public var numOutCh:Number = 4;
	
	function Analog(gainer:Gainer) {
		this.gainer = gainer;
	}
	
	public function configuration(nIn:Number,nOut:Number):Void {
  		numInCh = nIn;
  		numOutCh = nOut;
  	}
	
  	//アナログ受信開始
  	public function begin():Void {
		gainer.enqueue(new GainerCommand(gainer, "i*"));
  	}
 
  	//アナログ受信終了
  	public function end():Void {
		gainer.enqueue(new SynchronizedGC(gainer, "E*", "E*"));
  	}
	
	public function peek():Void {
		gainer.enqueue(new SynchronizedGC(gainer, "I*", "I"));
		//gainer.enqueue(new GainerCommand(gainer, "I*"));
  	}
	
	//指定したチャンネルへ送信
  	public function out(ch:Number,value:Number):Void {
		if(typeof(ch) == "number") {
			if(numOutCh>ch){
				var s:String = "a" + ch.toString(16);
				value = value<  0 ?   0: value;
				value = value>255 ? 255: value;
					
				var sv:String = value<16 ? "0": "";
				sv+= value.toString(16);
				s+=sv;
				s+="*";
				gainer.enqueue(new SynchronizedGC(gainer, s, "a"));
			}else{
				trace("Gainer error!! out of bounds analog out");
			}
		} else if(ch instanceof Array) {
			var values = ch;
		
			var s:String = "A";
			var sv:String = "";
			if(numOutCh==values.length){
				for(var i=0;i<values.length;i++){
					values[i] = values[i]<  0 ?   0: values[i];
					values[i] = values[i]>255 ? 255: values[i];
					sv = values[i]<16 ? "0": "";
					sv+= values[i].toString(16);
					s+=sv;
				}
			  s+= "*";
			}else{
				trace("Gainer error!! - number of analog outputs are wrong");
			}
			gainer.enqueue(new SynchronizedGC(gainer, s, "A"));
		}
 	 }
}