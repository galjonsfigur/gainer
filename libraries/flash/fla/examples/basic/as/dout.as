import gainer.*;

var gnr:Gainer = new Gainer("localhost", 2000, Gainer.MODE1, true);

dArray = [false, false, false, false];

gnr.onReady = function() {
	d0.onRelease = function() {
		dOut(this);
	}
	d1.onRelease = function() {
		dOut(this);
	}
	d2.onRelease = function() {
		dOut(this);
	}
	d3.onRelease = function() {
		dOut(this);
	}
}

function dOut(obj) {
	if (obj._currentframe == 1) {
		obj.gotoAndStop(2);
	} else {
		obj.gotoAndStop(1);
	}
	dArray[obj._name.substr(1, 1)] = !dArray[obj._name.substr(1, 1)];
	gnr.digitalOutput(dArray);
}
