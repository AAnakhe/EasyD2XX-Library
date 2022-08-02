package com.example.easyd2xx;

import java.nio.ByteBuffer;
import java.util.List;
import java.util.Scanner;

public class Test {

	public static void main(String[] args) {
		
		try {
			List<EasyD2XX> devices = EasyD2XX.getDevices();

			if(devices.isEmpty()) {
				System.out.println("No devices were detected. Exiting.");
				return;
			}

			System.out.println("Select a device to read from");
			System.out.println();

			for(int i = 0; i < devices.size(); i++) {

				System.out.println("Devices " + i + ":");
				EasyD2XX device = devices.get(i);
				System.out.println("Name: " + device.name);
				System.out.println("Chip: " + device.chipName);
				System.out.println("SN: " + device.serialNumber);
				System.out.println("Location: " + device.location);
				System.out.println();
			}

			Scanner stdin = new Scanner(System.in);
			int deviceIndex = stdin.nextInt();
			stdin.close();

			EasyD2XX device = devices.get(deviceIndex);
			device.openAsFifo(1000, 1000);

			long start = System.currentTimeMillis();
			@SuppressWarnings("unused")  byte[] oneGbArray = device.read(1073741824);
			long stop = System.currentTimeMillis();
			System.out.println("Read 1 GB into a Byte[] in " + (stop - start) + "ms.");

			start = System.currentTimeMillis();
			ByteBuffer buffer = ByteBuffer.allocateDirect(1073741824);
			device.read(buffer, 1073741824);
			stop = System.currentTimeMillis();
			System.out.println("Read 1 GB into a ByteBuffer in " + (stop - start) + "ms.");

			device.close();
			System.out.println("Done. Exiting");

	} catch(Exception | UnsatisfiedLinkError e) {

		System.out.println(e.getMessage() + "Exiting.");
		e.printStackTrace();
	}

	}

}
