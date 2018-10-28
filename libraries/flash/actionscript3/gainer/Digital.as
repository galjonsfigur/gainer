/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */
package gainer
{
	
	import gainer.*;
	
	public class Digital {
		
		public var _gainer:Gainer;
		public var numInCh:Number = 4;
		public var numOutCh:Number = 4;
		
		function Digital(_gainer:Gainer) {
			this._gainer = _gainer;
		}
		
		public function configuration(nIn:Number, nOut:Number):void {
			numInCh = nIn;
			numOutCh = nOut;
		}
		
		public function peek():void {
			_gainer.enqueue(new SynchronizedGC(_gainer, "R*", "R"));
			//_gainer.enqueue(new GainerCommand(_gainer, "R*"));
		}
		
		public function begin():void {
			_gainer.enqueue(new GainerCommand(_gainer, "r*"));
		}
		
		public function end():void {
			_gainer.enqueue(new SynchronizedGC(_gainer, "E*", "E*"));
		}
		
		//全チャンネルに一度に送信
		public function out(values):void {
			var i:Number;
			var chs:Number = 0;
			if(typeof(values) == "number") {
				chs = values;
			} else if (values is Array) {
				if (numOutCh == values.length) {
					for (i=0; i<values.length; i++) {
						if (values[i]) {
							chs |= (1 << i);
						}
					}
				} else {
					trace("Gainer error!! out of bounds digital out");
				}
			}
			if (chs<=0xFFFF) {
				var val:String = chs.toString(16).toUpperCase();
				var sv:String = "";
				//必ず4桁
				for (i=0; i<4-val.length; i++) {
					sv += "0";
				}
				sv += val;
				var s:String = "D"+sv+"*";
				
				if (_gainer.getVerbose()) _gainer.enqueue(new SynchronizedGC(_gainer, s, "D"));
				else _gainer.enqueue(new GainerCommand(_gainer, s));
					
			} else {
				trace("Gainer error!! out of bounds digital out");
			}
		}
		//指定したチャンネルをHigh
		public function high(ch:Object):void {
			var s:String;
			var i:Number;
			
			if(typeof(ch) == "number") {
				if (numOutCh>ch) {
					s = "H"+ch.toString(16).toUpperCase()+"*";
					trace(s);
					_gainer.enqueue(new SynchronizedGC(_gainer, s, "H"));
				} else {
					trace("Gainer error!! out of bounds digital out");
				}
			} else if (ch is Array) {
				var vch:Number = 0;
				for (i=0; i<ch.length; i++) {
					if (numOutCh>ch[i]) {
						vch |= (1 << ch[i]);
					} else {
						trace("Gainer error!! out of bounds digital out");
					}
				}
				var val:String = vch.toString(16).toUpperCase();
				var sv:String = "";
				for (i=0; i<numOutCh-val.length; i++) {
					sv += "0";
				}
				sv += val;
				s = "D"+sv+"*";
				_gainer.enqueue(new SynchronizedGC(_gainer, s, "D"));
			}
		}
		//指定したチャンネルをLOW
		public function low(ch:Object):void {
			var s:String;
			
			if(typeof(ch) == "number") {
				if (numOutCh>ch) {
					s = "L"+ch.toString(16).toUpperCase()+"*";
					_gainer.enqueue(new SynchronizedGC(_gainer, s, "L"));
				} else {
					trace("Gainer error!! out of bounds digital out");
				}
			} else if (ch is Array) {
				var vch:Number = 0;
				var xor:Number = 0;
				for (var i:Number = 0; i<numOutCh; i++) {
					xor |= (1 << i);
				}
				for (i = 0; i<ch.length; i++) {
					if (numOutCh>ch[i]) {
						vch |= (1 << ch[i]);
					} else {
						trace("Gainer error!! out of bounds digital out");
					}
				}
				vch ^= xor;
				var val:String = vch.toString(16).toUpperCase();
				var sv:String = "";
				for (i = 0; i<numOutCh-val.length; i++) {
					sv += "0";
				}
				sv += val;
				s = "D"+sv+"*";
				_gainer.enqueue(new SynchronizedGC(_gainer, s, "D"));
			}
		}
	}
}