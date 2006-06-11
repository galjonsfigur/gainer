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
				if(gainer.port != null)gainer.write("E*");
				System.exit(0);
			}
		});

		// GUI初期化
		setTitle("GAINER serial proxy");
		setSize(300, 200);
		show();
		setLayout(null);
		setResizable(false);
		log = new TextArea(
				"GAINER serial proxy\n@author PDP Project\n@version 1.0\n\n",
				5, 10, TextArea.SCROLLBARS_VERTICAL_ONLY);
		Insets insets = this.getInsets();
		log.setBounds(insets.left, insets.top,
				300 - (insets.left + insets.right),
				200 - (insets.top + insets.bottom));
		log.setEditable(false);
		this.add(log);

		// テキストファイルからポート番号を読む
		String[] setting = loadStrings("setting.txt");
		if(setting[0] == null) {
			setting[0] = "2000";
			printMsg("undefined server port. use default setting: "+setting[0]);
		}
		if(setting[1] == null) {
			setting[1] = Serial.autoPortName();
			printMsg("undefined serial port. use default setting: "+setting[1]);
		}
		// バイパス開始
		server = new Server(this, Integer.parseInt(setting[0]));
		server.start();
		gainer = new Serial(this, setting[1]);

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
			gainer.write(msg);
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
			e.printStackTrace();
		} finally {
			try {
				if (bufferedReader != null) {
					bufferedReader.close();
				}
			} catch (IOException e) {
				e.printStackTrace();
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
