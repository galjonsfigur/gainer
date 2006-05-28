﻿/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

import gainer.*;

class gainer.SynchronizedGC extends GainerCommand {
	
	public var returnCode:String;
	private var timer:Timer;
	
	function SynchronizedGC(_gainer:Gainer, msg:String, returnCode:String) {
		super(_gainer, msg);
		this.returnCode = returnCode;
	}
	
	public function sendMsg():Void {
		_gainer.write(msg);
		timer = new Timer(this, onTimeout, _gainer.timeout);
		_gainer.addEventListener("onReceived", this);
	}
	
	private function onReceived(evtObj):Void {
		var sReturn = evtObj.sReturn;
		if(sReturn.indexOf(returnCode) == 0) {
			timer.clear();
			_gainer.removeEventListener("onReceived", this);
			dispatchEvent({type:"onSuccess"});
		}
	}
	
	private function onTimeout() {
		_gainer.removeEventListener("onReceived", this);
		dispatchEvent({type:"onFailed"});
	}
	
}