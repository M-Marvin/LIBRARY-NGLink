package de.m_marvin.nglink;

public class NativeNGLink {
	
	private static boolean libLoaded = false;
	
	static {
		libLoaded = NativeExtractor.extractNativeLibs();
	}
	
	public static boolean loadedSuccessfully() {
		return libLoaded;
	}
	
	public NativeNGLink() {
		if (!loadedSuccessfully()) throw new IllegalStateException("Native nglink lib is not loaded, mybe a error in the extracting process!");	
	}
	
	// Complete vector info, description and values
	public static record NGComplex(double[] real, double[] imag) {};
	public static record VectorInfo(String name, int type, short flags, double[] realdata, double[]  complexdata, int length) {};
	// Only vector values, no description of the vector (except for the name)
	public static record VectorValues(String name, double realdata, double complexdata, boolean isScale, boolean isComplex) {};
	public static record VectorValuesAll(int count, int index, VectorValues[] values) {};
	// Only vector description, no values
	public static record VectorDescription(int number, String name, boolean isReal) {};
	public static record PlotDescription(String name, String title, String date, String type, int vectorCount, VectorDescription[] vectorInfo) {};
	
	public static abstract class NGCallback {
		public abstract void log(String s);
		public abstract void detacheNGSpice();
		public abstract void reciveVecData(VectorValuesAll vecData, int vectorCount);
		public abstract void reciveInitData(PlotDescription plotInfo);
	}
	
	public native int initNGLink(NGCallback callbacks);
	public native int initNGSPice(String libName);
	
	public native int detachNGSpice();
	public native int execCommand(String command);
	public native int execList(String commandList);
	public native boolean isRunning();
	public native String[] listPlots();
	public native String getCurrentPlot();
	public native String[] getVecs(String plotName);
	public native VectorInfo getVec(String vecName);
	
}

