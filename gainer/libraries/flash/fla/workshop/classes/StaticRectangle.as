﻿class StaticRectangle extends RigidBody {		public var extents:Array;	public var axes:Array;	private var oldPosition:Vector;	private var range:Object;		function StaticRectangle() {	}		public function set rotation(t:Number):Void {		_rotation = t * 180 / Math.PI;		setAxes(t);	}		public function get rotation():Number {		return _rotation;	}		public function init(x, y, width, height, rotation, elasticity, friction):Void {		super.init(x,y,100000,elasticity,friction);				oldPosition = new Vector(x,y);		_width = width;		_height = height;				extents = new Array(width/2, height/2);				axes = new Array(2);		axes[0] = new Vector(0,0);		axes[1] = new Vector(0,0);		this.rotation = rotation;				range = new Object();		range.min = 0;		range.max = 0;	}		public function update():Void {		velocity.x = position.x - oldPosition.x;		velocity.y = position.y - oldPosition.y;		oldPosition.x = position.x;		oldPosition.y = position.y;	}		public function resolve(mtd:Vector, vel:Vector, normal:Vector, depth:Number, order:Number):Void {	}		public function getProjection(v:Vector):Object {		var radius:Number = extents[0] * Math.abs(v.dot(axes[0])) + extents[1] * Math.abs(v.dot(axes[1]));		var c:Number = position.dot(v);		range.min = c - radius;		range.max = c + radius;		return range;	}		private function setAxes(t:Number):Void {		var s:Number = Math.sin(t);		var c:Number = Math.cos(t);					axes[0].x = c;		axes[0].y = s;		axes[1].x = -s;		axes[1].y = c;	}}