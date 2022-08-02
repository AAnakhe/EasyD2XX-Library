package com.example.easyd2xx;

import java.nio.ByteBuffer;
import java.util.List;

public class EasyD2XX {
	
	public String name;
	public String chipName;
	public String serialNumber;
	public int location;
	private Long handle;


	private EasyD2XX(String name, String chipName, String serialNumber, int location) {
		this.name = name;
		this.chipName = chipName;
		this.serialNumber = serialNumber;
		this.location = location;
	}



	static {
		try {
			System.loadLibrary("EasyD2XX");
		}
		catch (UnsatisfiedLinkError e) {
			
			// do nothing to allow graceful degradation if the FTDI device driver or EasyD2XX.dll could not be found.
			// native function calls will now  throw an UnsatisfiedLinkError when they are called.

		}
	}

	public static native List<EasyD2XX> getDevices();

	public native void openAsFifo(int readTimeoutMilliseconds, int writeTimeoutMilliseconds) throws Exception;

	public native byte[] read(int byteCount) throws Exception;

	public native void read(ByteBuffer buffer, int byteCount) throws Exception;

	public native void close() throws Exception;

}
