/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

import pdp.*;

class pdp.SynchronizedGC extends GainerCommand {
	
	public var returnCode:String;
	private var timer:Timer;
	
	function SynchronizedGC(gainer:Gainer, msg:String, returnCode:String) {
		super(gainer, msg);
		this.returnCode = returnCode;
	}
	
	public function sendMsg():Void {
		gainer.write(msg);
		timer = new Timer(this, onTimeout, gainer.timeout);
		gainer.addEventListener("onReceived", this);
	}
	
	private function onReceived(evtObj):Void {
		var sReturn = evtObj.sReturn;
		if(sReturn.indexOf(returnCode) == 0) {
			timer.clear();
			gainer.removeEventListener("onReceived", this);
			dispatchEvent({type:"onSuccess"});
		}
	}
	
	private function onTimeout() {
		gainer.removeEventListener("onReceived", this);
		dispatchEvent({type:"onFailed"});
	}
	
}