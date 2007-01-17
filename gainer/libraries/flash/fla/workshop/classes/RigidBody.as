﻿class RigidBody extends MovieClip {	public var mass:Number = 1;	public var elasticity:Number = 0.3;	public var friction:Number = 0.0;	public var force:Vector;	public var velocity:Vector;	public var simulator:Simulator;	private var _position:Vector;		public function init(x:Number, y:Number, mass:Number, elasticity:Number, friction:Number) {		force = new Vector(0,0);		velocity = new Vector(0,0);		position = new Vector(x,y);		this.mass = mass;		this.elasticity = elasticity;		this.friction = friction;	}		public function set position(p:Vector) {		_position = p;		_x = p.x;		_y = p.y;	}		public function get position():Vector {		return _position;	}		public function update():Void {		force.y += simulator.gravity;		velocity.plusEquals(force);		position.plusEquals(velocity);				_x = position.x;		_y = position.y;		force.x = 0;		force.y = 0;	}		public function resolve(mtd:Vector, vel:Vector, normal:Vector, depth:Number, order:Number):Void {		position.plusEquals(mtd);		_x = position.x;		_y = position.y;		velocity = vel;	}}