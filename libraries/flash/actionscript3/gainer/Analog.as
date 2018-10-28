/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

package gainer
{

	import gainer.*;
	
	public class Analog {
		private var _gainer:Gainer;
		public var numInCh:Number = 4;
		public var numOutCh:Number = 4;
		
		function Analog(_gainer:Gainer) {
			this._gainer = _gainer;
		}
		
		public function configuration(nIn:Number,nOut:Number):void {
	  		numInCh = nIn;
	  		numOutCh = nOut;
	  	}
		
	  	//アナログ受信開始
	  	public function begin():void {
			_gainer.enqueue(new GainerCommand(_gainer, "i*"));
	  	}
	 
	  	//アナログ受信終了
	  	public function end():void {
			_gainer.enqueue(new SynchronizedGC(_gainer, "E*", "E*"));
	  	}
		
		public function peek():void {
			_gainer.enqueue(new SynchronizedGC(_gainer, "I*", "I"));
			//_gainer.enqueue(new GainerCommand(_gainer, "I*"));
	  	}
		
		//指定したチャンネルへ送信
	  	public function out(ch,value:Number=0):void {
			var s:String;
			var sv:String;
			
			if(typeof(ch) == "number") {
				if(numOutCh>ch){
					s = "a" + ch.toString(16).toUpperCase();
					value = Math.floor(value);
					value = value<  0 ?   0: value;
					value = value>255 ? 255: value;
						
					sv = value<16 ? "0": "";
					sv+= value.toString(16).toUpperCase();
					s+=sv;
					s+="*";
					_gainer.enqueue(new SynchronizedGC(_gainer, s, "a"));
				}else{
					trace("Gainer error!! out of bounds analog out");
				}
			} else if(ch is Array) {
				var values:Array = ch;
			
				s = "A";
				sv = "";
				if(numOutCh==values.length){
					for(var i:Number = 0;i<values.length;i++){
						values[i] = Math.floor(values[i]);
						values[i] = values[i]<  0 ?   0: values[i];
						values[i] = values[i]>255 ? 255: values[i];
						sv = values[i]<16 ? "0": "";
						sv+= values[i].toString(16).toUpperCase();
						s+=sv;
					}
				  s+= "*";
				}else{
					trace("Gainer error!! - number of analog outputs are wrong");
				}
				_gainer.enqueue(new SynchronizedGC(_gainer, s, "A"));
			}
	 	 }
	}
}