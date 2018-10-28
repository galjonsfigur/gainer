import gainer.*;

var gnr:Gainer = new Gainer("localhost", 2000, Gainer.MODE1, true);

var aoutValues:Array;

gnr.onReady = function() {
	bar0.slider.onPress = function() {
		beginDrag(this);
	}
	bar0.slider.onRelease = function() {
		endDrag(this);
	}
	bar1.slider.onPress = function() {
		beginDrag(this);
	}
	bar1.slider.onRelease = function() {
		endDrag(this);
	}
	bar2.slider.onPress = function() {
		beginDrag(this);
	}
	bar2.slider.onRelease = function() {
		endDrag(this);
	}
	bar3.slider.onPress = function() {
		beginDrag(this);
	}
	bar3.slider.onRelease = function() {
		endDrag(this);
	}

	aoutValues = new Array(4);

	this.onFinished = function() {
		this.analogOutput(aoutValues);
	}
}

function beginDrag (obj) {
	obj.startDrag(false, 0, 0, obj._parent.gauge._width-obj._width, 0);
	obj.onEnterFrame = function () {
		aoutValues[0] = bar0.slider._x;
		aoutValues[1] = bar1.slider._x;
		aoutValues[2] = bar2.slider._x;
		aoutValues[3] = bar3.slider._x;
	}
}

function endDrag(obj) {
	obj.stopDrag();
	delete obj.onEnterFrame;
}

