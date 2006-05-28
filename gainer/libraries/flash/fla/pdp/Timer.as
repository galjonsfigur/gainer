/**
 * GAINER flash libray
 * @author PDP Project
 * @version 1.0
 */

import pdp.*;

class pdp.Timer
{     
		private var timeInt:Number;
		
        function Timer(obj, func, len)
        {
			var owner = this;
			timeInt = setInterval(function(){func.apply(obj);owner.clear();}, len);
        }
		
		public function clear() {
			clearInterval(timeInt);
		}
}