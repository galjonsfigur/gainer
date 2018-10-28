/**
 * SerialProxy
 *
 * @author PDP Project
 * @version 1.0
 */

import java.io.*;
import java.awt.*;
import java.awt.event.*;

public class SerialProxy extends Frame {
	
	private Server server;

	private Serial gainer;

	private TextArea log;

	private boolean logEnable = false;

	public SerialProxy() {
		super();

		// Windowが閉じられたら
		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent evt) {
				System.exit(0);
			}
		});
		
		// GUI初期化
		setTitle("GAINER serial proxy");
		setSize(300, 200);
		show();
		this.
		setLayout(null);
		setResizable(false);
		log = new TextArea(
				"GAINER serial proxy\n@author PDP Project\n@version 1.1\n\n",
				5, 10, TextArea.SCROLLBARS_VERTICAL_ONLY);
		Insets insets = this.getInsets();
		log.setBounds(insets.left, insets.top,
				300 - (insets.left + insets.right),
				200 - (insets.top + insets.bottom));
		log.setEditable(false);
		this.add(log);

		// テキストファイルからポート番号を読む
		String[] setting = loadStrings("setting.txt");
		if(setting[0] == null) setting[0] = "2000";

		// バイパス開始
		gainer = new Serial(this);
		if (gainer.start(setting[1])) {
			server = new Server(this, Integer.parseInt(setting[0]));
			server.start();
		}
	}

	// GAINERにメッセージ送信
	public void sendToGainer(String msg) {
		if (msg.startsWith("startLog")) {
			logEnable = true;
		} else if (msg.startsWith("stopLog")) {
			logEnable = false;
		} else if (msg.startsWith("clearLog")) {
			log.setText("");
		} else {
			try {
				gainer.write(msg);
			} catch (Exception e) {}
			if (logEnable)
				printMsg("from client: " + msg);
		}
	}

	// Flashにメッセージ送信
	public void sendToFlash(String msg) {
		server.sendToAll(msg);
		if (logEnable)
			printMsg("from GAINER: " + msg);
	}
	
	// クライアントが0になったらリブートする
	public void onDisconnected() {
		gainer.write("Q*");
	}

	// メッセージをテキストエリアに出力
	public void printMsg(String msg) {
		log.append(msg + "\n");
	}

	// テキストファイルからはじめの2行を読む
	public String[] loadStrings(String filename) {
		String[] setting = new String[2];
		FileReader fileReader = null;
		BufferedReader bufferedReader = null;
		try {
			fileReader = new FileReader(filename);
			bufferedReader = new BufferedReader(fileReader);

			for (int i = 0; i < 2; i++) {
				String line = bufferedReader.readLine();
				if (line == null) {
					break;
				}
				setting[i] = line;
			}
		} catch (IOException e) {
		} finally {
			try {
				if (bufferedReader != null) {
					bufferedReader.close();
				}
			} catch (IOException e) {
			}
			try {
				if (fileReader != null) {
					fileReader.close();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		return setting;
	}

	// スタートポイント
	public static void main(String[] args) {
		new SerialProxy();
	}
}
