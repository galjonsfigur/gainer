README

Open a proper directory (win or mac) for your platform (Windows 
or Mac OS X), and open the settings.txt with your text editor.
Then edit the file if needed:
* If you want to specify a network port, write the number in the 
  first line of the settings.txt
* If you want to specify a serial port, write the port name in 
  the second line of the settings.txt

If you don't specify a network port, the default value (2000) 
will be used. If you don't specify a serial port, the gsp 
application will look for a serial port of your Gainer I/O module.

NOTE: If you want to specify a serial port on Windows, please add 
"//./" as a prefix for the actual serial port name:
e.g.: COM4 > //./COM4

We would like to fix this issue in a near future version.


win/macの各フォルダ内のsetting.txtをテキストエディタで開き、1行目にサー
バーのポート番号を、2行目にGainer I/Oモジュールが接続されているシリアル
ポート名を入力し、保存してください。

setting.txtがない場合、あるいは未記入の項目がある場合は、サーバーのポー
ト番号はデフォルトで2000に、シリアルポート名は、自動で選択されます。

なお、Windowsでシリアルポート名を指定する場合、ポート名の前に「//./」を
追加して指定してください。
例：COM4　→　//./COM4

※この問題は将来のバージョンで修正する予定です。
