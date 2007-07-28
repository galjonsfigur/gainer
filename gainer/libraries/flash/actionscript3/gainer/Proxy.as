package gainer
{
	
	public class Proxy {
		public static function create(obj:Object, func:Function):Function
		{
			return function():Object {
				return func.apply(obj, arguments);
			};
		}
	}
}