/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

import mx.events.EventDispatcher;
import gainer.*;

class gainer.GainerCommand {
	
	private var _gainer:Gainer;
	public var msg:String;
	public var dispatchEvent:Function;
	
	function GainerCommand(_gainer:Gainer, msg:String) {
		EventDispatcher.initialize(this);
		this._gainer = _gainer;
		this.msg = msg;
	}
	
	public function sendMsg():Void {
		_gainer.write(msg);
		dispatchEvent({type:"onSuccess"});
	};
}