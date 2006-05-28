/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

import mx.events.EventDispatcher;
import pdp.*;

class pdp.GainerCommand {
	
	private var gainer:Gainer;
	public var msg:String;
	public var dispatchEvent:Function;
	
	function GainerCommand(gainer:Gainer, msg:String) {
		EventDispatcher.initialize(this);
		this.gainer = gainer;
		this.msg = msg;
	}
	
	public function sendMsg():Void {
		gainer.write(msg);
		dispatchEvent({type:"onSuccess"});
	};
}