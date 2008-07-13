package {	import flash.display.Sprite;	import flash.events.Event;	import flash.text.*;	import gainer.*;	public class DigitalInput extends Sprite {		private var _gio:Gainer;		var _textBoxes:Array;		public function DigitalInput() {			// Gainerのインスタンスを生成する			_gio = new Gainer("localhost", 2000, Gainer.MODE1, true);			// アナログ入力の値を表示するために.fla側で配置したテキストフィールドを取得			_textBoxes = new Array(4);			for (var i:int = 0; i < 4; i++) {				_textBoxes[i] = this["d" + i];			}			// I/Oモジュールの準備が完了した時に呼ばれるイベントハンドラをセット			_gio.onReady = _start;		}		// I/Oモジュールの準備が完了したら以下を実行		private function _start():void {			// アナログ入力の連続取得を開始			_gio.beginDigitalInput();			// ENTER_FRAMEイベントに対するイベントリスナをセット			addEventListener(Event.ENTER_FRAME, _onEnterFrame);		}		// 毎フレームごとにテキストフィールドを更新する		private function _onEnterFrame(e:Event):void {			for (var i:int = 0; i < _gio.digitalInput.length; i++) {				_textBoxes[i].text = _gio.digitalInput[i];			}		}	}}