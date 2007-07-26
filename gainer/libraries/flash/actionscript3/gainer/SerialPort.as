/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

package gainer
{
	import gainer.*;
	import flash.net.XMLSocket;
	import flash.events.DataEvent;
	import flash.events.Event;
	
	public class SerialPort {
		
		private var ip:String;
		private var port:Number;
		private var socket:XMLSocket;
		
		public function SerialPort(ip:String, port:Number) {
			socket = new XMLSocket();
			this.ip = ip;
			this.port = port;
			
			var scope:SerialPort = this;
			socket.addEventListener(DataEvent.DATA, function(src:DataEvent):void {
				//var xml:XML = new XML(src);
				scope.onReceiveStr(src.data);
			});
			
			socket.addEventListener(Event.CONNECT, function(success:Boolean):void {
				if (success) {
					trace("connected to the server");
					scope.onConnected();
				} else {
					trace("connection failed");
				}
			});
			socket.connect(ip, port);
		}
		
		/*
		public function onReceiveStr(str:String) {
		}
		*/
		public var onReceiveStr:Function;
		
		/*
		public function onConnected() {
		}
		*/
		public var onConnected:Function;
		
		public function close():void {
			socket.close();
		}
		
		public function writeString(param:String):void {
			socket.send(param);
		}
		
	}
}