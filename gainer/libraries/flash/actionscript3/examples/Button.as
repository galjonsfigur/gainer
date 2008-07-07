﻿package {	import flash.display.Shape;	import flash.display.Sprite;	import gainer.*;	public class Button extends Sprite {		private var _gnr:Gainer;	// Gainerのインスタンス		private var _sqr:Shape;		// ボタンの状態を表示するオブジェクト		public function Button() {			// Gainerのインスタンスを生成する			_gnr = new Gainer("localhost", 2000, Gainer.MODE1, true);			// I/Oモジュールのボタンの状態を表示するオブジェクトを生成する			_sqr = new Shape();			_sqr.graphics.beginFill(0x000000);			_sqr.graphics.drawRect(100, 60, 120, 120);			_sqr.graphics.endFill();			this.addChild(_sqr);			// I/Oモジュールの準備が完了した時に呼ばれるイベントハンドラをセット			_gnr.onReady = function() {			  _start();			};		}		private function _start():void {			// I/Oモジュールのボタンが押された時に呼ばれるイベントハンドラをセット							_gnr.onPressed = _onPress;						// I/Oモジュールのボタンが離された時に呼ばれるイベントハンドラをセット							_gnr.onReleased = _onRelease;		}		private function _onPress():void {			_sqr.visible = false;		}		private function _onRelease():void {			_sqr.visible = true;		}	}}