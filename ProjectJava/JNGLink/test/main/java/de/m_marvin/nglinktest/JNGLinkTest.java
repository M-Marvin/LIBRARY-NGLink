package de.m_marvin.nglinktest;

import de.m_marvin.nglink.NativeNGLink;
import de.m_marvin.nglink.NativeNGLink.NGCallback;
import de.m_marvin.nglink.NativeNGLink.PlotDescription;
import de.m_marvin.nglink.NativeNGLink.VectorValuesAll;

public class JNGLinkTest {
	
	public static void main(String... args) {
		
		if (!NativeNGLink.loadedSuccessfully()) System.out.println("NOT LOADED");
		
		NativeNGLink nglink = new NativeNGLink();
		
		NGCallback callback = new NGCallback() {
			
			@Override
			public void reciveVecData(VectorValuesAll vecData, int vectorCount) {
				// TODO Auto-generated method stub
				
			}
			
			@Override
			public void reciveInitData(PlotDescription plotInfo) {
				// TODO Auto-generated method stub
				
			}
			
			@Override
			public void log(String s) {
				System.out.println("LOG: " + s);
			}
			
			@Override
			public void detacheNGSpice() {
				System.out.println("Detache NGSpice");
				nglink.detachNGSpice();
			}
		};
		
		nglink.initNGLink(callback);
		
		nglink.initNGSPice("ngspice36_x64.dll");
		
	}
	
}
