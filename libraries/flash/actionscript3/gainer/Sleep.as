/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

package gainer
{

	import gainer.*;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	
	public class Sleep {
		
		private var wait:Number = 100;
		//replace public var dispatchEvent:Function;
		public var eventDispatcher:EventDispatcher;
		
		public var timer:Timer;
		
		function Sleep(wait:Number) {
			this.wait = wait;
			eventDispatcher = new EventDispatcher();
		}
		
		public function sendMsg():void {
			timer = new Timer(this, function():void {
				eventDispatcher.dispatchEvent(new Event("onSuccess"));
			}, wait);
		}
		
	}
}