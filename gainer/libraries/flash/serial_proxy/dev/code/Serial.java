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

	static private final String version = "?1.0.";
	
	private SerialProxy parent;

	public SerialPort port;

	private final int rate = 38400;

	private final int parity = SerialPort.PARITY_NONE;

	private final int databits = 8;

	private final int stopbits = SerialPort.STOPBITS_1;

	public InputStream input;

	public OutputStream output;
	
	private String inputValue;

	byte buffer[] = new byte[64];

	int bufferIndex;

	int bufferLast;

	int bufferSize = 64;

	public Serial(SerialProxy sp) {
		this.parent = sp;
	}
	
	public boolean start(String specifiedName) {
		parent.printMsg("starting GAINER...");
		try {
			Enumeration portList = CommPortIdentifier.getPortIdentifiers();
			while (portList.hasMoreElements()) {
				CommPortIdentifier portId = (CommPortIdentifier) portList.nextElement();
				if (portId.getPortType() == CommPortIdentifier.PORT_SERIAL) {
					if (openSerialPort(portId) && foundGainer(specifiedName)) {
						printMsg("GAINER started on port " + port.getName());
						return true;
					} else {
						closeSerialPort();
					}
				}
			}
			printMsg("specified port was not found...");
			return false;
		} catch (Exception e) {
			printMsg("connection error inside Serial...");
			closeSerialPort();
			return false;
		}
	}
	
	private boolean openSerialPort(CommPortIdentifier portId) {
		try{
			port = (SerialPort) portId.open("GainerSerialPort", 2000);
			input = port.getInputStream();
			output = port.getOutputStream();
			port.setSerialPortParams(rate, databits, stopbits, parity);
			port.addEventListener(this);
			port.notifyOnDataAvailable(true);
		} catch (Exception e) {
			return false;
		}
		return true;
	}

	private void closeSerialPort() {
		try {
			if (input != null) input.close();
			if (output != null) output.close();
		} catch (Exception e) {}
		input = null;
		output = null;

		try {
			if (port != null) {
				port.removeEventListener();
				port.close();
			}
		} catch (Exception e) {}
		port = null;
	}
	
	private boolean foundGainer(String specifiedName) {
		String portName = port.getName();
		if (specifiedName != null) {
			return portName.equals(specifiedName);
		} else {
			try{
				/*
				write("Q*");
				Thread.sleep(100);
				*/
				write("?*");
				Thread.sleep(100);
				boolean isGainer = inputValue.startsWith(version);
				return isGainer;
			} catch(Exception e) {
				return false;
			}
		}
	}
	
	// メッセージをテキストエリアに出力
	public void printMsg(String msg) {
		parent.printMsg(msg);
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
		if (b == null) return null;
		inputValue = new String(b);
		return inputValue;
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
