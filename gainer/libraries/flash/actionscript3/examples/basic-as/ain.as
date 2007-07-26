import gainer.*;

var gnr:Gainer = new Gainer("localhost", 2000, Gainer.MODE1, true);

gnr.onReady = function() {
	gnr.beginAnalogInput();
	_root.onEnterFrame = function() {
		for (i=0; i<gnr.analogInput.length; i++) {
			_root["a"+i].text = gnr.analogInput[i];
		}
	}
}
