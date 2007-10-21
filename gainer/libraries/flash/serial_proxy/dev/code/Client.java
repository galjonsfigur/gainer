/**
 * Client class
 *
 * @author PDP Project
 * @version 1.0
 */

import java.io.*;
import java.net.*;

public class Client extends Thread {

	private Server server;

	private Socket socket;

	protected BufferedReader in;

	protected PrintWriter out;

	private String ip;
	
	private boolean runFlag = true;

	public Client(Server server, Socket socket) {
		this.server = server;
		this.socket = socket;
		this.ip = socket.getInetAddress().getHostAddress();

		try {
			in = new BufferedReader(new InputStreamReader(
					socket.getInputStream(), "SJIS"));
			out = new PrintWriter(new OutputStreamWriter(
					socket.getOutputStream(), "SJIS"), true);
		} catch (IOException ioe) {
			server.printMsg("initilization error on Client: " + ip
					+ ". disconnected.");
			ioe.printStackTrace();
			stopClient();
		}
	}

	// クライアントからのメッセージをGainerに渡す
	public void run() {
		try {

			char c[] = new char[1];
			// クライアントからのストリームがある
			while (in.read(c, 0, 1) != -1 && runFlag) {

				StringBuffer sb = new StringBuffer(4096);
				// null文字がこなければ
				while (c[0] != '\0') {

					sb.append(c[0]);

					in.read(c, 0, 1);
				}
				server.onReceiveMsg(sb.toString());
			}
		} catch (IOException ioe) {
			server.printMsg("read error on Client: " + ip + ". disconnected.");
		} finally {
			stopClient();
		}
	}

	// クライアントのIPを返す
	public final String getIP() {
		return ip;
	}

	// クライアントにメッセージを送る
	public void sendToClient(String msg) {
		out.print(msg + "\0");
		if (out.checkError()) {
			server.printMsg("write error on Client: " + ip + ". disconnected.");
			stopClient();
		}
	}

	// クライアントを停止する
	private void stopClient() {
		server.deleteClient(this);
		try {
			in.close();
			out.close();
			socket.close();
		} catch (IOException ioe) {
			server.printMsg("connection error on Client: " + ip);
			ioe.printStackTrace();
		}
	}
	
	// クライアントを終了する
	public void dispose() {
		runFlag = false;
	}
}