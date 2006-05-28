/**
 * Server class
 *
 * @author PDP Project
 * @version 1.0
 */

import java.net.*;
import java.io.*;
import java.util.*;

public class Server extends Thread {

	private ServerSocket srvsocket;

	private Vector clist;

	private int port;

	private SerialProxy parent;

	public Server(SerialProxy parent, int port) {
		this.parent = parent;
		clist = new Vector();
		this.port = port;
	}

	// 接続しているすべてのクライアントにメッセージを配る
	public void sendToAll(String msg) {
		if (clist != null) {
			Enumeration e = clist.elements();
			while (e.hasMoreElements()) {
				Client c = (Client) (e.nextElement());
				c.sendToClient(msg);
			}
		}
	}

	// 現在の接続数を返す
	public synchronized int getClientsCount() {
		return clist.size();
	}

	// メッセージをテキストエリアに出力
	public void printMsg(String msg) {
		parent.printMsg(msg);
	}

	// サーバを開始し、接続を受け付ける
	public void run() {

		printMsg("starting server...");

		try {
			srvsocket = new ServerSocket(port);
			printMsg("server started on port " + port);

			while (true) {
				Socket sock = srvsocket.accept();
				Client c = new Client(this, sock);
				clist.add(c);
				printMsg(c.getIP() + " connected to the server.");
				c.start();
			}
		} catch (IOException ioe) {
			printMsg("connection error inside Server. closing serversocket...");
			ioe.printStackTrace();
			stopServer();
		}
	}

	// サーバーを停止する
	private void stopServer() {
		try {
			srvsocket.close();
			printMsg("serversocket closed");

		} catch (IOException ioe) {
			ioe.printStackTrace();
		}
	}

	// クライアントを削除
	public void deleteClient(Client c) {
		printMsg(c.getIP() + " disconnected.");
		clist.remove(c);
	}

	// クライアントからのメッセージを受信したとき
	public void onReceiveMsg(String msg) {
		parent.sendToGainer(msg);
	}
	
	// クライアントすべてを終了する
	public void dispose(){
		if (clist != null) {
			Enumeration e = clist.elements();
			while (e.hasMoreElements()) {
				Client c = (Client) (e.nextElement());
				c.dispose();
			}
		}
	}
}
