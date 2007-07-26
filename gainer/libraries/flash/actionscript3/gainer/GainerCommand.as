/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

package gainer
{
	//replace import mx.events.EventDispatcher;
	import flash.events.EventDispatcher;
	import gainer.*;
	import flash.events.Event;
	
	public class GainerCommand {
		
		protected var _gainer:Gainer;
		public var msg:String;
		public var eventDispatcher:EventDispatcher;
		
		public var onSuccess:Function;
		public var onFailed:Function;
		
		function GainerCommand(_gainerParam:Gainer, msgParam:String) {
			//replace EventDispatcher.initialize(this);
			//replace this._gainer = _gainer;
			eventDispatcher = new EventDispatcher();
			_gainer = _gainerParam;
			msg = msgParam;
		}
		
		public function sendMsg():void {
			_gainer.write(msg);
			//replace dispatchEvent({type:"onSuccess"});
			eventDispatcher.dispatchEvent(new Event("onSuccess"));
		};
	}
}