/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

package gainer
{

	import gainer.*;
	import flash.utils.clearInterval;
	import flash.utils.setInterval;
		
	public class Timer
	{     
			private var timeInt:uint;
			
	        function Timer(obj:Object, func:Function, wait:Number)
	        {
				var owner:Timer = this;
				timeInt = setInterval(
					function():void{
						func.apply(obj);
						owner.clear();
					},
					wait
				);
	        }
			
			public function clear():void {
				clearInterval(timeInt);
			}
	}
}