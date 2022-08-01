package de.m_marvin.nglink;

import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.Scanner;

public class NativeExtractor {
	
	public static boolean extractNativeLibs() {
		
		String osPrefix = getOS();
		
		try {
			
			System.out.println("Try to extract files for " + osPrefix + " os ...");
			Scanner scanner = new Scanner(NativeExtractor.class.getResourceAsStream("/" + osPrefix));
			while (scanner.hasNextLine()) {
				String fileName = scanner.nextLine();
				System.out.println("File " + fileName + " ...");
				URL inputStream = ClassLoader.getSystemResource(osPrefix + "/" + fileName);
				
				System.load(inputStream.getPath());
				
//				inputStream.close(); 
//				System.out.println("Done");	
			}
			scanner.close();
			
		} catch (UnsatisfiedLinkError e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return false;
		
	}
	
	public static String getOS() {
		
		String osName = System.getProperty("os.name").toLowerCase();
		
		if (osName.contains("win")) {
			return "win";
		} else if (osName.contains("nix") || osName.contains("nux") || osName.contains("aix")) {
			return "uni";
		} else if (osName.contains("mac")) {
			return "osx";
		} else if (osName.contains("sunos")) {
			return "sol";
		} else {
			return "err";
		}
		
	}
	
}
