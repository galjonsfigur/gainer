import gainer.Gainer;
import flash.utils.Timer;
import flash.events.TimerEvent;
		public var gnr:Gainer = new Gainer("localhost", 2000, Gainer.MODE1, true);public var inputTimer:Timer;

public function init():void
{
	//same button.as
	gnr.onPressed = on_board_button_pressed;
	gnr.onReleased = on_board_button_released;
	
	switchBeginInput();
	var delay_msec:Number = 1000;
	inputTimer = new Timer(delay_msec, 0);
	inputTimer.addEventListener(TimerEvent.TIMER, switchInput);
	inputTimer.start();
}
				public function led_on():void{	//same led.as
	gnr.turnOnLED();}		public function led_off():void{	//same led.as
	gnr.turnOffLED();}public function on_board_button_pressed():void{	//same button.as
	onBoardStatusText.text = "Pressed";}public function on_board_button_released():void
{
	//same button.as
	onBoardStatusText.text = "Released";
}
public function dOut():void
{
	//same aout.as
	var dArray:Array = [false, false, false, false];
	
	dArray[0] = do0.selected;
	dArray[1] = do1.selected;
	dArray[2] = do2.selected;
	dArray[3] = do3.selected;
	
	gnr.digitalOutput(dArray);
}

public function aOut():void
{
	//same dout.as
	var aoutValues:Array = new Array(4);
	aoutValues[0] = ao0.value;
	aoutValues[1] = ao1.value;
	aoutValues[2] = ao2.value;
	aoutValues[3] = ao3.value;
	gnr.analogOutput(aoutValues);
}

public function aIn():void
{
	//same ain.as
	var aString:String = "";
	for (var i:Number=0; i<gnr.digitalInput.length; i++) {
		aString += "aIn" + i.toString() + ":" + gnr.analogInput[i] + "  ";
	}
	aInText.text = aString;}

public function dIn():void
{
	//same din.as
	var dString:String = "";
	for (var i:Number=0; i<gnr.digitalInput.length; i++) {
		dString += "dIn" + i.toString() + ":" + gnr.digitalInput[i] + "  ";
	}
	dInText.text = dString;}
private function switchBeginInput():void
{
	if("digital" == inputType.selectedValue)
	{
		gnr.endAnalogInput();
		gnr.beginDigitalInput();
		aInText.text = "Disable";
	}
	else
	{
		gnr.endDigitalInput();
		gnr.beginAnalogInput();
		dInText.text = "Disable";
	}}
private function switchInput(dummy:TimerEvent):void
{
	if("digital" == inputType.selectedValue)
	{
		dIn();
	}
	else
	{
		aIn();
	}
}