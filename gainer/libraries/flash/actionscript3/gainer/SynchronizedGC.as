/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

package gainer
{
	import flash.events.EventDispatcher;
	import gainer.*;
	import flash.events.Event;
	
	public class SynchronizedGC extends GainerCommand {
		
		public var returnCode:String;
		private var timer:Timer;
		
		function SynchronizedGC(_gainer:Gainer, msg:String, returnCode:String) {
			super(_gainer, msg);
			this.returnCode = returnCode;
		}
		
		override public function sendMsg():void {
			_gainer.write(msg);
			timer = new Timer(this, onTimeout, _gainer.timeout);
			//replace _gainer.addEventListener("onReceived", this);
			_gainer.eventDispatcher.addEventListener("onReceived", onReceived);
		}
		
		private function onReceived(evtObj:ReturnEvent):void {
			var sReturn:String = evtObj.sReturn;
			if(sReturn.indexOf(returnCode) == 0) {
				timer.clear();
				//replace _gainer.removeEventListener("onReceived", this);
				_gainer.eventDispatcher.removeEventListener("onReceived", onReceived);
				//replace dispatchEvent({type:"onSuccess"});
				eventDispatcher.dispatchEvent(new Event("onSuccess"));
			}
		}
		
		private function onTimeout():void {
			//replace _gainer.removeEventListener("onReceived", this);
			_gainer.eventDispatcher.removeEventListener("onReceived", onReceived);
			//replace dispatchEvent({type:"onFailed"});
			eventDispatcher.dispatchEvent(new Event("onFailed"));
		}
		
	}
}