/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

import pdp.*;

class pdp.SerialPort {
	
	private var ip:String;
	private var port:Number;
	private var socket:XMLSocket;
	
	public function SerialPort(ip:String, port:Number) {
		socket = new XMLSocket();
		this.ip = ip;
		this.port = port;
		
		var scope = this;
		socket.onXML = function(src:XML) {
			scope.onReceiveStr(src.toString());
		};
		
		socket.onConnect = function(success:Boolean) {
			if (success) {
				trace("connected to the server");
				scope.onConnected();
			} else {
				trace("connection failed");
			}
		};
		socket.connect(ip, port);
	}
	
	public function onReceiveStr(str:String) {
	}
	
	public function onConnected() {
	}
	
	public function close() {
		socket.close();
	}
	
	public function writeString(param:String) {
		socket.send(param);
	}
	
}
