package de.m_marvin.nglink;

import java.io.FileNotFoundException;

public class NativeNGLink {

	// Complete vector info, description and values
	public static record NGComplex(double real, double imag) {};
	public static record VectorInfo(String name, int type, short flags, double[] realdata, NGComplex[] complexdata, int length) {};
	// Only vector values, no description of the vector (except for the name)
	public static record VectorValue(String name, double realdata, double complexdata, boolean isScale, boolean isComplex) {};
	public static record VectorValuesAll(int count, int index, VectorValue[] values) {};
	// Only vector description, no values
	public static record VectorDescription(int number, String name, boolean isReal) {};
	public static record PlotDescription(String name, String title, String date, String type, int vectorCount, VectorDescription[] vectorInfo) {};
	
	public static abstract class NGCallback {
		public abstract void log(String s);
		public abstract void detacheNGSpice();
		public abstract void reciveVecData(VectorValuesAll vecData, int vectorCount);
		public abstract void reciveInitData(PlotDescription plotInfo);
	}
	
	private static boolean libLoaded = false;
	
	static {
		try {
			if (NativeExtractor.extractNativeLibs()) {
				System.load(NativeExtractor.findNative("nglink"));
				NativeExtractor.findNative("ngspice"); // Check ngspice lib
				libLoaded = true;
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
	
	public static boolean loadedSuccessfully() {
		return libLoaded;
	}
	
	public NativeNGLink() {
		if (!loadedSuccessfully()) throw new IllegalStateException("Native nglink lib is not loaded, mybe a error in the extracting process!");	
	}
	
	public native int initNGLink(NGCallback callbacks);
	public native int initNGSpice(String libName);
	
	public native int detachNGSpice();
	public native int execCommand(String command);
	public native int loadCircuit(String commandList);
	public native boolean isRunning();
	public native String[] listPlots();
	public native String getCurrentPlot();
	public native String[] getVecs(String plotName);
	public native VectorInfo getVec(String vecName);
	
}

