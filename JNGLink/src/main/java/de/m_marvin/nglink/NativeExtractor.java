package de.m_marvin.nglink;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.Enumeration;
import java.util.Scanner;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

public class NativeExtractor {
	
	public static final File NATIVE_TEMP_FOLDER = new File(System.getProperty("java.io.tmpdir"), "jnglink");
	
	public static boolean extractNativeLibs() {
		
		if (!NATIVE_TEMP_FOLDER.isDirectory()) NATIVE_TEMP_FOLDER.mkdir();
		
		String osPrefix = getOS() + getArch();
		
		try {
			System.out.println("Try to load files for " + osPrefix + " os ...");

			boolean failed = false;
			
			try {

				String jarLocation = NativeExtractor.class.getProtectionDomain().getCodeSource().getLocation().toURI().getPath();
				if (jarLocation.endsWith("#0!/")) jarLocation = jarLocation.substring(0, jarLocation.length() - 4);
				if (jarLocation.startsWith("/")) jarLocation = jarLocation.substring(1, jarLocation.length());
				JarFile libJar = new JarFile(jarLocation);
				Enumeration<JarEntry> jarEntrys = libJar.entries();
				while (jarEntrys.hasMoreElements())  {
					JarEntry entry = jarEntrys.nextElement();
					if (entry.getName().startsWith(osPrefix + "/") && !entry.isDirectory()) {
						String[] s = entry.getName().split("/");
						String libName = s[s.length - 1];
						System.out.println("File " + libName + " ...");
						InputStream nativeInputStream = libJar.getInputStream(entry);
						if (!extractNative(nativeInputStream, libName)) {
							System.err.println("Failed to extract lib!");
							failed = true;
						}
					}
				}
				libJar.close();
				
			} catch (FileNotFoundException e) {
				
				System.out.println("Could not open jar file, asuming running in ide!");

				Scanner scanner = new Scanner(NativeExtractor.class.getResourceAsStream("/" + osPrefix));
				while (scanner.hasNextLine()) {
					String fileName = scanner.nextLine();
					System.out.println("File " + fileName + " ...");
					URL nativeLib = ClassLoader.getSystemResource(osPrefix + "/" + fileName);
					InputStream nativeInputStream = new FileInputStream(new File(nativeLib.getPath()));
					if (!extractNative(nativeInputStream, fileName)) {
						System.err.println("Failed to extract lib!");
						failed = true;
					}
				}
				scanner.close();
				
			}

			System.out.println(failed ? "Not all natives could be extracted!" : "Done");
			NATIVE_TEMP_FOLDER.deleteOnExit();
			
			return !failed;
		} catch (UnsatisfiedLinkError e) {
			System.err.println("Failed to load native libs for nglink!");
			e.printStackTrace();
		} catch (URISyntaxException e) {
			System.err.println("Failed to load native libs for nglink!");
			e.printStackTrace();
		} catch (IOException e) {
			System.err.println("Failed to load native libs for nglink!");
			e.printStackTrace();
		}
		
		return false;
		
	}
	
	public static boolean extractNative(InputStream inputStream, String libName) {
		try {
			File tempLibFile = new File(NATIVE_TEMP_FOLDER, libName);
			FileOutputStream outputStream;
			try {
				outputStream = new FileOutputStream(tempLibFile);
			} catch (FileNotFoundException e) {
				return true; // File exists already and is in use
			}
			outputStream.write(inputStream.readAllBytes());
			outputStream.close();
			inputStream.close();
			return true;
		} catch (IOException e) {
			System.err.println("Failed to transfer file!");
			e.printStackTrace();
		}
		return false;
	}
	
	public static String findNative(String libName) throws FileNotFoundException {
		for (File file : NATIVE_TEMP_FOLDER.listFiles()) {
			if (file.getName().contains(libName)) return file.getAbsolutePath();
		}
		throw new FileNotFoundException("Could not find extracted native " + libName);
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
