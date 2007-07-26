/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

package gainer
{
//	import mx.events.EventDispatcher;

	import flash.events.EventDispatcher;
	import gainer.*;
	import flash.events.Event;

	public class CommandQueue {
		
		private var commands:Array;
		
		private var hasJob:Boolean;
		
		//private var dispatchEvent:Function;
		
		//public var addEventListener:Function;
		
		//public var removeEventListener:Function;
		
		public var eventDispatcher:EventDispatcher;
		
		public function onFinished():void {}
		
		public function onTimeout(command:Object):void {}
		
		public function set nextTask(cmd:Object):void {
			enqueue(cmd);
		}
		
		function CommandQueue() {
			//EventDispatcher.initialize(this);
			eventDispatcher = new EventDispatcher();
			commands = new Array();
			hasJob = false;
		}
		
		public function enqueue(cmd:Object):void {
			if(cmd is Function || cmd is GainerCommand || cmd is Sleep) {
				commands.push(cmd);
				if(!hasJob){
					hasJob = true;
					dequeue();
				}
			}
		}
		
		public function dequeue():void {
			if(commands.length > 0) {
				var cmd:Object = commands.shift();
				if(cmd is Function) {
					cmd.apply(this);
					dequeue();
				} else if(cmd is GainerCommand || cmd is Sleep) {
					//replace cmd.addEventListener("onSuccess", this);
					cmd.eventDispatcher.addEventListener("onSuccess", onSuccess);
					//replace cmd.addEventListener("onFailed", this);
					cmd.eventDispatcher.addEventListener("onFailed", onFailed);
					cmd.sendMsg();
				}
			} else {
				hasJob = false;
				onFinished();
				//dispatchEvent({type:"onFinished"});
			}
		}
		
		public function onSuccess(evtObj:Event):void {
			dequeue();
			//replace evtObj.target.removeEventListener("onSuccess", this);
			evtObj.target.removeEventListener("onSuccess", onSuccess);
			//replace evtObj.target.removeEventListener("onFailed", this);
			evtObj.target.removeEventListener("onFailed", onFailed);
		}
		
		public function onFailed(evtObj:Event):void {
			//replace evtObj.target.removeEventListener("onSuccess", this);
			evtObj.target.removeEventListener("onSuccess", onSuccess);
			//replace evtObj.target.removeEventListener("onFailed", this);
			evtObj.target.removeEventListener("onFailed", onFailed);
			trace("wait for  " + evtObj);
			trace("_gainer TIMEOUT return code error!!" );
			onTimeout(evtObj.target);
			//dispatchEvent({type:"onTimeout", command:evtObj.target});
		}
		
		public function getTaskCount():Number {
			return commands.length;
		}
	}
}