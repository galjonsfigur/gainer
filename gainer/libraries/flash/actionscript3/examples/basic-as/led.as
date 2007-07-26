import gainer.*;

var gnr:Gainer = new Gainer("localhost", 2000, Gainer.MODE1, true);

gnr.onReady = function() {
	sqr.onPress = function() {
		gnr.turnOnLED();
	}
	sqr.onRelease = function() {
		gnr.turnOffLED();
	}
}
