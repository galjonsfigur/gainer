package gainer
{
	import flash.events.Event;

	public class ReturnEvent extends Event
	{
		public var sReturn:String;
		
		function ReturnEvent(eventType:String, sReturnParam:String):void{
			super(eventType);
			sReturn = sReturnParam;
		}
	}
}