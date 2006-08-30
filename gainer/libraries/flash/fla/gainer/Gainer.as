/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

import mx.utils.Delegate;
import gainer.*;

class gainer.Gainer extends CommandQueue{

	public var buttonPressed:Boolean;
	
	public var digitalInput:Array;
	
	public var analogInput:Array;
	
	public var currentMode:Number;
	
	public static var MODE1:Number = 1;
	
	public static var MODE2:Number = 2;
	
	public static var MODE3:Number = 3;
	
	public static var MODE4:Number = 4;
	
	public static var MODE5:Number = 5;
	
	public static var MODE6:Number = 6;
	
	public static var MODE7:Number = 7;
	
	public var timeout:Number = 1000;
	
	private var serial_net:SerialPort;
	
	private var digital:Digital;
	
	private var analog:Analog;
	
	private var verbose:Boolean = true;
	
	private var mode:Number = 1;
	
	private var sReturn:String;
	
	public function onReady() {}
	
	public function onReceived(msg:String) {}
	
	public function onPressed(){}
	
	public function onReleased(){}
	
	function Gainer(ip:String, port:Number, mode:Number, verbose:Boolean) {
		super();
		
		digital = new Digital(this);
		analog = new Analog(this);
		this.verbose = verbose;
		this.mode = mode;
		
		serial_net = new SerialPort(ip, port);
		serial_net.onReceiveStr = Delegate.create(this, serialEvent);
		serial_net.onConnected = Delegate.create(this, initialize);
	}
	
	private function initialize() {
		reboot();
		setVerbose(verbose);
		configuration(mode);
		enqueue(Delegate.create(this, onReady));
	}
	
	private function setVerbose(verbose:Boolean):Void {
		this.verbose = verbose;
		var c:String;
		
		if (verbose) {
			enqueue(new GainerCommand(this, "V1*"));
		} else {
			enqueue(new GainerCommand(this, "V0*"));
		}
		enqueue(new Sleep(100));
	}
	
	public function getVerbose():Boolean {
		return verbose;
	}
	
	public function reboot() {
		trace("reboot now...");
		enqueue(new GainerCommand(this, "Q*"));
		enqueue(new Sleep(1000));
		var checkReboot = function(){
			if(sReturn.indexOf("Q*") == -1 || sReturn == undefined){
				trace("reboot error !!!");
			}else{
				trace("GAINER ready to Roll. ");
			}
		}
		enqueue(Delegate.create(this, checkReboot));
	}
	
	private function configuration(mode:Number):Void {
		//     ain :din :aout:dout
		//     ----:----:----:----
		// C0:    0:   0:   0:   0
		// C1:    4:   4:   4:   4
		// C2:    8:   0:   4:   4
		// C3:    4:   4:   8:   0
		// C4:    8:   0:   8:   0		←追加
		// C5:    0:  16:   0:   0	←以降一個ずつ後ろに
		// C6:    0:   0:   0:  16
		analogInput = null;
		digitalInput = null;
		switch (mode) {
		case MODE1 :
			analogInput = new Array(4);
			digitalInput = new Array(4);
			digital.configuration(4, 4);
			analog.configuration(4, 4);
			enqueue(new SynchronizedGC(this, "KONFIGURATION_1*", "KONFIGURATION_1"));
			break;
		case MODE2 :
			analogInput = new Array(8);
			digitalInput = new Array(0);
			digital.configuration(0, 4);
			analog.configuration(8, 4);
			enqueue(new SynchronizedGC(this, "KONFIGURATION_2*", "KONFIGURATION_2"));
			break;
		case MODE3 :
			analogInput = new Array(4);
			digitalInput = new Array(4);
			digital.configuration(4, 0);
			analog.configuration(4, 8);
			enqueue(new SynchronizedGC(this, "KONFIGURATION_3*", "KONFIGURATION_3"));
			break;
		case MODE4 :
			analogInput = new Array(8);
			digitalInput = new Array(0);
			digital.configuration(0, 0);
			analog.configuration(8, 8);
			enqueue(new SynchronizedGC(this, "KONFIGURATION_4*", "KONFIGURATION_4"));
			break;
		case MODE5 :
			analogInput = new Array(0);
			digitalInput = new Array(16);
			digital.configuration(16, 0);
			analog.configuration(0, 0);
			enqueue(new SynchronizedGC(this, "KONFIGURATION_5*", "KONFIGURATION_5"));
			break;
		case MODE6 :
			analogInput = new Array(0);
			digitalInput = new Array(0);
			digital.configuration(0, 16);
			analog.configuration(0, 0);
			enqueue(new SynchronizedGC(this, "KONFIGURATION_6*", "KONFIGURATION_6"));
			break;
		case MODE7:
			analogInput = new Array(0);
			digitalInput = new Array(0);
			digital.configuration(0,0);
			analog.configuration(0,0);
			enqueue(new SynchronizedGC(this, "KONFIGURATION_7*", "KONFIGURATION_7"));
			break;
		}
		enqueue(new Sleep(1000));
	}
	
	public function serialEvent(str:String):Void {
		sReturn = str;
		
		onReceived(str);
		
		if(sReturn.charAt(0) == "i" || sReturn.charAt(0) == "I"){
			var value:String;
            for(var i=0;i<analogInput.length;i++){
            	value = sReturn.substring(2*i+1,2*(i+1)+1);
            	analogInput[i] = parseInt(value,16);
            }
		} else if(sReturn.charAt(0) == "r" || sReturn.charAt(0) == "R"){
			var value:Number = parseInt(sReturn.substring(1,5),16);
            for(var i=0;i<digitalInput.length;i++){
            	var c:Number = 1&(value>>i);
            	if(c==1){
            		digitalInput[i] = true;
            	}else{
            		digitalInput[i] = false;
            	}
            }
		} else if(sReturn.charAt(0) == "N"){
              buttonPressed = true;
			  onPressed();
			  //dispatchEvent({type:"onPressed"});
        } else if(sReturn.charAt(0) == "F"){
              buttonPressed = false;
			  onReleased();
			  //dispatchEvent({type:"onReleased"});
        }
		
		dispatchEvent({type:"onReceived", sReturn:sReturn});
	}
	
	public function turnOnLED():Void {
		enqueue(new SynchronizedGC(this, "h*", "h"));
	}
	public function turnOffLED():Void {
		enqueue(new SynchronizedGC(this, "l*", "l"));
	}
	
	public function peekDigitalInput():Void {
		digital.peek();
	}
	public function beginDigitalInput():Void {
		digital.begin();
	}
	public function endDigitalInput():Void {
		digital.end();
	}
	public function digitalOutput(obj):Void {
		digital.out(obj);
	}
	public function setHigh(obj):Void {
		digital.high(obj);
	}
	public function setLow(obj):Void {
		digital.low(obj);
	}
	public function peekAnalogInput():Void {
		analog.peek();
	}
	public function beginAnalogInput():Void {
		analog.begin();
	}
	public function endAnalogInput():Void {
		analog.end();
	}
	public function analogOutput(ch, value):Void {
		analog.out(ch,value);
	}
	public function write(what:String):Void {
		serial_net.writeString(what);
	}
	//mode7のみ
	//line毎に処理
	public function scanLine(line:Number,values:Array):Void {
		if(currentMode==MODE7){
			var s:String = "a";
			var sv:String = "";
			if(values.length == 8){
				if(line<8){
					s += line.toString(16).toUpperCase();
				}else{
					trace("Gainer error!! out of bounds");
				}
				for(var i=0;i<8;i++){
					sv = values[i]<16 ? "0": "";
					s += sv;
					s += values[i].toString(16).toUpperCase();
				}
				s += "*";
				enqueue(new SynchronizedGC(this, s, "a"));
			}else{
				trace("Gainer error!! number of values");
			}
		}else{
			trace("Gainer error!! this method can use only MODE7");
		}
	}
	//mode7のみ
	//LEDmatrix全体を処理
	public function scanMatrix(values:Array):Void {
		if(currentMode==MODE7){
			if(values.length == 64){
				for(var col=0;col<8;col++){
					var v:Array = new Array(8);
					for(var i=0;i<8;i++){
						v[i] = values[col*8+i];
					}
					scanLine(col,v);
				}
			}
		}else{
			trace("Gainer error!! this method can use only MODE7");
		}
	}
	public function ampGainAGND(gain:Number):Void {
		var s:String ="G";
	 	if(gain>=0 && gain<16){
	  		s += gain.toString(16).toUpperCase();
  			s += "1";
	 	}else{
	 		trace("Gainer error!! gain");
	 	}
	 	s +="*";
	 	enqueue(new SynchronizedGC(this, s, "G"));
	}
	public function ampGainDGND(gain:Number):Void {
		var s:String ="G";
	  	if(gain>=0 && gain<16){
	   		s += gain.toString(16).toUpperCase();
	   		s += "0";
	  	}else{
	  		trace("Gainer error!! gain");
	 	}
	  	s +="*";
	   	enqueue(new SynchronizedGC(this, s, "G"));
	}
	public function startLog():Void {
		write("startLog");
	}
	public function stopLog():Void {
		write("stopLog");
	}
	public function clearLog():Void {
		write("clearLog");
	}
	public function sleep(wait:Number):Void {
		enqueue(new Sleep(wait));
	}
	public function remap(input:Number, inmin:Number, inmax:Number, outmin:Number, outmax:Number):Number {
		return ((input-inmin)*(outmax-outmin))/((inmax-inmin)+outmin);
	}
}