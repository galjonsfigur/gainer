import gainer.*;

var gnr:Gainer = new Gainer("localhost", 2000, Gainer.MODE1, true);

gnr.onReady = function() {
	gnr.beginDigitalInput();
	_root.onEnterFrame = function() {
		for (i=0; i<gnr.digitalInput.length; i++) {
			_root["d"+i].text = gnr.digitalInput[i];
		}
	}
}
