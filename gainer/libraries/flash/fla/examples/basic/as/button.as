import gainer.*;

var gnr:Gainer = new Gainer("localhost", 2000, Gainer.MODE1, true);

gnr.onReady = function() {
	this.onPressed = function(){
		sqr._visible = false;
	}
	this.onReleased = function(){
		sqr._visible = true;
	}
}
