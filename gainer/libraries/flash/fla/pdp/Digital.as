/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

import pdp.*;

class pdp.Digital {
	
	public var gainer:Gainer;
	public var numInCh:Number = 4;
	public var numOutCh:Number = 4;
	
	function Digital(gainer:Gainer) {
		this.gainer = gainer;
	}
	
	public function configuration(nIn:Number, nOut:Number):Void {
		numInCh = nIn;
		numOutCh = nOut;
	}
	
	public function peek():Void {
		gainer.enqueue(new SynchronizedGC(gainer, "R*", "R"));
		//gainer.enqueue(new GainerCommand(gainer, "R*"));
	}
	
	public function begin():Void {
		gainer.enqueue(new GainerCommand(gainer, "r*"));
	}
	
	public function end():Void {
		gainer.enqueue(new SynchronizedGC(gainer, "E*", "E*"));
	}
	
	//全チャンネルに一度に送信
	public function out(values):Void {
		var chs:Number = 0;
		if(typeof(values) == "number") {
			chs = values;
		} else if (values instanceof Array) {
			if (numOutCh == values.length) {
				for (var i = 0; i<values.length; i++) {
					if (values[i]) {
						chs |= (1 << i);
					}
				}
			} else {
				trace("Gainer error!! out of bounds digital out");
			}
		}
		if (chs<=0xFFFF) {
			var val:String = chs.toString(16);
			var sv:String = "";
			//必ず4桁
			for (var i = 0; i<4-val.length; i++) {
				sv += "0";
			}
			sv += val;
			var s:String = "D"+sv+"*";
			
			if (gainer.getVerbose()) gainer.enqueue(new SynchronizedGC(gainer, s, "D"));
			else gainer.enqueue(new GainerCommand(gainer, s));
				
		} else {
			trace("Gainer error!! out of bounds digital out");
		}
	}
	//指定したチャンネルをHigh
	public function high(ch):Void {
		if(typeof(ch) == "number") {
			if (numOutCh>ch) {
				var s:String = "H"+ch.toString(16)+"*";
				trace(s);
				gainer.enqueue(new SynchronizedGC(gainer, s, "H"));
			} else {
				trace("Gainer error!! out of bounds digital out");
			}
		} else if (ch instanceof Array) {
			var vch:Number = 0;
			for (var i = 0; i<ch.length; i++) {
				if (numOutCh>ch[i]) {
					vch |= (1 << ch[i]);
				} else {
					trace("Gainer error!! out of bounds digital out");
				}
			}
			var val:String = vch.toString(16);
			var sv:String = "";
			for (var i = 0; i<numOutCh-val.length; i++) {
				sv += "0";
			}
			sv += val;
			var s:String = "D"+sv+"*";
			gainer.enqueue(new SynchronizedGC(gainer, s, "D"));
		}
	}
	//指定したチャンネルをLOW
	public function low(ch):Void {
		if(typeof(ch) == "number") {
			if (numOutCh>ch) {
				var s:String = "L"+ch.toString(16)+"*";
				gainer.enqueue(new SynchronizedGC(gainer, s, "L"));
			} else {
				trace("Gainer error!! out of bounds digital out");
			}
		} else if (ch instanceof Array) {
			var vch:Number = 0;
			var xor:Number = 0;
			for (var i = 0; i<numOutCh; i++) {
				xor |= (1 << i);
			}
			for (var i = 0; i<ch.length; i++) {
				if (numOutCh>ch[i]) {
					vch |= (1 << ch[i]);
				} else {
					trace("Gainer error!! out of bounds digital out");
				}
			}
			vch ^= xor;
			var val:String = vch.toString(16);
			var sv:String = "";
			for (var i = 0; i<numOutCh-val.length; i++) {
				sv += "0";
			}
			sv += val;
			var s:String = "D"+sv+"*";
			gainer.enqueue(new SynchronizedGC(gainer, s, "D"));
		}
	}
}
