package de.m_marvin.nglink;

import java.net.URL;
import java.util.Scanner;

public class NativeExtractor {
	
	public static boolean extractNativeLibs() {
		
		String osPrefix = getOS() + getArch();
		
		try {
			
			System.out.println("Try to load files for " + osPrefix + " os ...");
			Scanner scanner = new Scanner(NativeExtractor.class.getResourceAsStream("/" + osPrefix));
			while (scanner.hasNextLine()) {
				String fileName = scanner.nextLine();
				System.out.println("File " + fileName + " ...");
				URL nativeLib = ClassLoader.getSystemResource(osPrefix + "/" + fileName);
				System.load(nativeLib.getPath());	
			}
			scanner.close();
			System.out.println("Done");
			
			return true;
		} catch (UnsatisfiedLinkError e) {
			System.err.println("Failed to load native libs for nglink!");
			e.printStackTrace();
		}
		
		return false;
		
	}
	
	public static String getArch() {
		String archName = System.getProperty("os.arch");
		return archName.contains("64") ? "64" : "32";
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
