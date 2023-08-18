package de.m_marvin.nglink;

public class NativeNGLink {

	private static String ngspiceNativeAutodetect;
	
	static {
		NativeLoader.setTempLibFolder(System.getProperty("java.io.tmpdir") + "/jnglink");
		NativeLoader.setLibLoadConfig("/libload_nglink.cfg");
		NativeLoader.loadNative("nglink");
		ngspiceNativeAutodetect = NativeLoader.getNative("ngspice");
	}
	
	// Complete vector info, description and values
	public static record NGComplex(double real, double imag) {};
	public static record VectorInfo(String name, int type, short flags, double[] realdata, NGComplex[] complexdata, int length) {};
	// Only vector values, no description of the vector (except for the name)
	public static record VectorValue(String name, double realdata, double complexdata, boolean isScale, boolean isComplex) {};
	public static record VectorValuesAll(int count, int index, VectorValue[] values) {};
	// Only vector description, no values
	public static record VectorDescription(int number, String name, boolean isReal) {};
	public static record PlotDescription(String name, String title, String date, String type, int vectorCount, VectorDescription[] vectorInfo) {};
	
	public static interface INGCallback {
		public abstract void log(String s);
		public abstract void detacheNGSpice();
		public abstract void reciveVecData(VectorValuesAll vecData, int vectorCount);
		public abstract void reciveInitData(PlotDescription plotInfo);
	}
	
	
	
	
	private final String spiceLib;
	private final long classid;
	private INGCallback callbacks;
	
	public NativeNGLink(String spiceLib) {
		this.spiceLib = spiceLib;
		this.classid = this.hashCode();
	}
	
	public NativeNGLink() {
		this(ngspiceNativeAutodetect);
	}
	
	protected void log(String msg) {
		if (callbacks != null) {
			callbacks.log("[Java] " + msg);
		} else {
			System.out.println("[Java] " + msg);
		}
	}
	
	protected boolean classidErrorCodes(int code) {
		switch (code) {
		case 1:
			return true;
		case 0:
			return false;
		case -1:
			log("Error-Code returned: nglink not initialized or bound!");
			return false;
		case -2:
			log("Error-Code returned: ngspice not initialized or bound!");
			return false;
		case -3:
			log("Error-Code returned: jni-error, c-class not bound!");
			return false;
		default:
			log("Error-Code returned: unknown error!");
			return false;
		}
	}
	
	public native int initNGLink(long classid, INGCallback callbacks);
	public boolean initNGLink(INGCallback callback) {
		this.callbacks = callback;
		return classidErrorCodes(initNGLink(classid, callback));
	}
	
	private native int detachNGLink(long classid);
	public boolean detachNGLink() {
		return classidErrorCodes(detachNGLink(classid));
	}
	
	private native boolean isInitialized(long classid);
	public boolean isInitialized() {
		return isInitialized(classid);
	}
	
	private native int initNGSpice(long classid, String libName);
	public int initNGSpice(long classid) {
		return initNGSpice(classid, this.spiceLib);
	}
	public boolean initNGSpice() {
		return classidErrorCodes(initNGSpice(classid));
	}
	public boolean initNGSpice(String libName) {
		return classidErrorCodes(initNGSpice(classid, libName));
	}
	
	private native int detachNGSpice(long classid);
	public boolean detachNGSpice() {
		return classidErrorCodes(detachNGLink(classid));
	}
	
	private native boolean isNGSpiceAttached(long classid);
	public boolean isNGSpiceAttached() {
		return isNGSpiceAttached(classid);
	}
		
	private native int execCommand(long classid, String command);
	public boolean execCommand(String command) {
		return classidErrorCodes(execCommand(classid, command));
	}
	
	private native int loadCircuit(long classid, String commandList);
	public boolean loadCircuit(String commandLiString) {
		return classidErrorCodes(loadCircuit(classid, commandLiString));
	}
	
	private native boolean isRunning(long classid);
	public boolean isRunning() {
		return isRunning(classid);
	}
	
	private native String[] listPlots(long classid);
	public String[] listPlots() {
		return listPlots(classid);
	}
	
	private native String getCurrentPlot(long classid);
	public String getCurrentPlot() {
		return getCurrentPlot(classid);
	}
	
	private native String[] getVecs(long classid, String plotName);
	public String[] getVecs(String plotName) {
		return getVecs(classid, plotName);
	}
	
	private native VectorInfo getVec(long classid, String vecName);
	public VectorInfo getVec(String vecName) {
		return getVec(classid, vecName);
	}
	
}

