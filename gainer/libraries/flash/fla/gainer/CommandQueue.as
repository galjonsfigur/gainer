/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

import mx.events.EventDispatcher;
import gainer.*;

class gainer.CommandQueue {
	
	private var commands:Array;
	
	private var hasJob:Boolean;
	
	private var dispatchEvent:Function;
	
	public var addEventListener:Function;
	
	public var removeEventListener:Function;
	
	public function onFinished() {}
	
	public function onTimeout(command:Object) {}
	
	function set nextTask(cmd:Object):Void {
		enqueue(cmd);
	}
	
	function CommandQueue() {
		EventDispatcher.initialize(this);
		commands = new Array();
		hasJob = false;
	}
	
	public function enqueue(cmd:Object):Void {
		if(cmd instanceof Function || cmd instanceof GainerCommand) {
			commands.push(cmd);
			if(!hasJob){
				hasJob = true;
				dequeue();
			}
		}
	}
	
	public function dequeue():Void {
		if(commands.length > 0) {
			var cmd = commands.shift();
			if(cmd instanceof Function) {
				cmd.apply(this);
				dequeue();
			} else if(cmd instanceof GainerCommand) {
				cmd.addEventListener("onSuccess", this);
				cmd.addEventListener("onFailed", this);
				cmd.sendMsg();
			}
		} else {
			hasJob = false;
			onFinished();
			//dispatchEvent({type:"onFinished"});
		}
	}
	
	private function onSuccess(evtObj):Void {
		dequeue();
		evtObj.target.removeEventListener("onSuccess", this);
		evtObj.target.removeEventListener("onFailed", this);
	}
	
	private function onFailed(evtObj):Void {
		evtObj.target.removeEventListener("onSuccess", this);
		evtObj.target.removeEventListener("onFailed", this);
		trace("wait for  " + evtObj.target.returnCode);
		trace("_gainer TIMEOUT return code error!!" );
		onTimeout(evtObj.target);
		//dispatchEvent({type:"onTimeout", command:evtObj.target});
	}
	
	public function getTaskCount():Number {
		return commands.length;
	}
}