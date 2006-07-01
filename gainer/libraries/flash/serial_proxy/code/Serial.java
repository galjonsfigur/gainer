/**
 * Serial class
 *
 * @author PDP Project
 * @version 1.0
 */

import gnu.io.*;
import java.io.*;
import java.util.*;

public class Serial implements SerialPortEventListener {

	private SerialProxy parent;

	public SerialPort port;

	private final int rate = 38400;

	private final int parity = SerialPort.PARITY_NONE;

	private final int databits = 8;

	private final int stopbits = SerialPort.STOPBITS_1;

	public InputStream input;

	public OutputStream output;

	byte buffer[] = new byte[64];

	int bufferIndex;

	int bufferLast;

	int bufferSize = 64;

	public Serial(SerialProxy sp, String iname) {

		this.parent = sp;
		parent.printMsg("starting GAINER...");

		try {
			Enumeration portList = CommPortIdentifier.getPortIdentifiers();

			while (portList.hasMoreElements()) {
				CommPortIdentifier portId = (CommPortIdentifier) portList
						.nextElement();

				if (portId.getPortType() == CommPortIdentifier.PORT_SERIAL) {
					if (portId.getName().equals(iname)) {
						port = (SerialPort) portId.open("serial gainer", 2000);
						input = port.getInputStream();
						output = port.getOutputStream();
						port.setSerialPortParams(rate, databits, stopbits,
								parity);
						port.addEventListener(this);
						port.notifyOnDataAvailable(true);

						parent.printMsg("GAINER started on port " + iname);
					}
				}
			}
			if(port==null) printMsg("specified port was not found...");
			
		} catch (Exception e) {
			printMsg("connection error inside Serial. closing serialport...");
			e.printStackTrace();
			port = null;
			input = null;
			output = null;
		}

	}

	// 自動でシリアルポート名を得る
	static public String autoPortName() {
		String dname = null;

		try {
			Enumeration portList = CommPortIdentifier.getPortIdentifiers();
			while (portList.hasMoreElements()) {
				CommPortIdentifier portId = (CommPortIdentifier) portList
						.nextElement();
				if (portId.getPortType() == CommPortIdentifier.PORT_SERIAL) {

					String pname = portId.getName();
					if (pname.startsWith("/dev/cu.usbserial-")) {
						dname = pname;
					} else if (pname.startsWith("COM")) {
						dname = "COM3";
					}
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}

		return dname;
	}

	// メッセージをテキストエリアに出力
	public void printMsg(String msg) {
		parent.printMsg(msg);
	}

	// シリアルポートを停止する
	public void dispose() {
		port.removeEventListener();
		printMsg("dispose Gainer ..");
		try {
			if (input != null)
				input.close();
			if (output != null)
				output.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		input = null;
		output = null;

		try {
			if (port != null)
				port.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		port = null;
	}

	// シリアルから入力があったら
	synchronized public void serialEvent(SerialPortEvent serialEvent) {
		if (serialEvent.getEventType() == SerialPortEvent.DATA_AVAILABLE) {
			try {
				while (input.available() > 0) {
					synchronized (buffer) {
						if (bufferLast == buffer.length) {
							byte temp[] = new byte[bufferLast << 1];
							System.arraycopy(buffer, 0, temp, 0, bufferLast);
							buffer = temp;
						}
						buffer[bufferLast++] = (byte) input.read();

						if (buffer[bufferLast - 1] == '*') {
							parent.sendToFlash(readStringUntil('*'));
							clear();
						} else {
							continue;
						}
					}
				}
			} catch (IOException e) {

			}

		}
	}

	// バッファを空にする
	public void clear() {
		bufferLast = 0;
		bufferIndex = 0;
	}

	// 指定した文字までバッファを読み文字列で返す
	public String readStringUntil(int interesting) {
		byte b[] = readBytesUntil(interesting);
		if (b == null)
			return null;
		return new String(b);
	}

	// 指定した文字までバッファを読みバイト列で返す
	private byte[] readBytesUntil(int interesting) {
		if (bufferIndex == bufferLast)
			return null;
		byte what = (byte) interesting;

		synchronized (buffer) {
			int found = -1;
			for (int k = bufferIndex; k < bufferLast; k++) {
				if (buffer[k] == what) {
					found = k;
					break;
				}
			}
			if (found == -1)
				return null;

			int length = found - bufferIndex + 1;
			byte outgoing[] = new byte[length];
			System.arraycopy(buffer, bufferIndex, outgoing, 0, length);

			bufferIndex = 0; // rewind
			bufferLast = 0;
			return outgoing;
		}
	}

	// GAINERに文字列を送る
	public void write(String what) {
		try {
			output.write(what.getBytes());
			output.flush();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
