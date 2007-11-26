package processing.gainer;

import java.io.*;
import java.util.*;


import gnu.io.*;

//�V���A���ʐM���J�v�Z��
public class Client{
	
  private final int rate=38400;
  private final int parity=SerialPort.PARITY_NONE;
  private final int databits=8;
  private final int stopbits=SerialPort.STOPBITS_1;
  
	public SerialPort port;
	private OutputStream output;

	public Client(){
	}
	
	
	public void cleanSerialPort(){
	
		closeSerialPort();
		try{
			output.close();
		}catch(IOException e){}
		output = null;
	}
	
	
	//Gainer�ɖ��߂𑗂�A�߂��҂��ǂ���
	public String sendGainer(String code, boolean answer) throws TimeoutException,IOException{
		String returnCode = "";
		
		if(!answer){
			write(code);
			return returnCode;
		}
		
		
		write(code);
		returnCode = readGainer(200);
		
		return returnCode;
	}
	
	//��M�o�b�t�@��ǂ�
	public String readGainer(long timeout) throws TimeoutException{
		try{
			InputStream input = port.getInputStream();
			long start = System.currentTimeMillis();
			int n = input.available();
			StringBuffer returnBuffer= new StringBuffer();
			while(n==0 && returnBuffer.indexOf("*")==-1){

					long now = System.currentTimeMillis();
					long rest = timeout - (now-start);
					if(rest <= 0){
						System.out.println("timeout return_  " +  Thread.currentThread().getName() );
						throw new TimeoutException("TimeoutException!! " + (now - start));
					}

				n = input.available();
				byte buf[] = new byte[n];
				input.read(buf);
				returnBuffer.append(new String(buf));
				
			}

			return returnBuffer.toString();
		}catch(IOException e){
			e.printStackTrace();
		}

		return "";
	}
	
	//���O���w�肵��Gainer���J��
	public boolean openGainer(String pname){
		if(openSerialPort(pname)){

			String returnCode="";
			while(!returnCode.equals("Q*")){
				try{
				returnCode = sendGainer("Q*",true);
				
				
				}catch(TimeoutException e){
				}catch(IOException e){
				}
			}
			return true;
		}
		return false;
	}
	
	//���ԂɃ|�[�g���`�F�b�N����Gainer��������
	public boolean findGainer(){
		//Properties prop = System.getProperties();
		//String os = prop.getProperty("os.name");
		Enumeration portList = CommPortIdentifier.getPortIdentifiers();
		if(!portList.hasMoreElements()){
			System.out.println("no elements :there is no com port");
		}
		System.out.println("finding Gainer...");
		while(portList.hasMoreElements()){
			CommPortIdentifier portId = (CommPortIdentifier) portList.nextElement();
			if (portId.getPortType() == CommPortIdentifier.PORT_SERIAL) {
				System.out.println("found port .. " + portId.getName());
				String pname = portId.getName();
				//mac pname.startsWith("/dev/cu.usbserial-")
				//win pname.startsWith("COM")
				if(openSerialPort(pname)){

					try{
						//�����Ŗ߂�l�̊m�F������[?*]
						sendGainer("Q*",true);//�K��Q�͑���
						Thread.sleep(100);
						String version = sendGainer("?*",true);
						System.out.println("gainer firm version " + version);
						if(version.startsWith(Gainer.libraryVersion, 1) ){
							return true;
						}
						sendGainer("Q*",true);
						cleanSerialPort();
						
						
					}catch(TimeoutException e){
						cleanSerialPort();
						continue;
					}catch(IOException e){
						cleanSerialPort();
						continue;
					}catch(InterruptedException e){
						e.getStackTrace();
					}

				}
			}
		}
		
		return false;
	}
	
	private void write(String what) throws IOException{
		write(what.getBytes());
	}
	
	
	private void write(byte bytes[]) throws IOException{
		output.write(bytes);
		output.flush();
		
	}
	

	private boolean openSerialPort(String portName){
		try{
			CommPortIdentifier portId = CommPortIdentifier.getPortIdentifier(portName);
			
			port = (SerialPort)portId.open("GainerSerialPort", 2000);
	    port.setSerialPortParams(rate, databits, stopbits, parity);

	    output = port.getOutputStream();
	    
		}catch(NoSuchPortException e){
			//����ȃ|�[�g�͂Ȃ�
			System.out.println("naiyo!");
			return false;
		}catch(PortInUseException e){
			//�|�[�g�͎g�p����
			System.out.println("this comm port is used by " + e.currentOwner + " !");
			return false;
		}catch(UnsupportedCommOperationException e){
			return false;
		}catch(IOException e){
			//���͏o�̓X�g���[��������
			System.out.println("io stream error!");
			return false;
		}

		
		//System.out.println("hiraita!");
		return true;
	}
	
	private void closeSerialPort(){
		if(port!=null) port.close();
		
		System.out.println("closing " + port.getName());
	}
	

}
