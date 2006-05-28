/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

import pdp.*;

class pdp.Sleep extends GainerCommand{
	
	private var wait:Number = 100;
	
	function Sleep(wait:Number) {
		this.wait = wait;
	}
	
	public function sendMsg():Void {
		new Timer(this, function() {
			dispatchEvent({type:"onSuccess"});
		}, wait);
	}
	
}