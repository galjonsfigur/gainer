package processing.gainer;



public class TimeoutException extends Exception{
		/**
	 * 
	 */
	private static final long serialVersionUID = -2579249283519796806L;

		public TimeoutException(){}
		
		public TimeoutException(String message){
			super(message);
		}
}
