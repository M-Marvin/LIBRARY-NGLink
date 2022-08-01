package de.m_marvin.nglinktest;

import de.m_marvin.nglink.NativeNGLink;

public class JNGLinkTest {
	
	public static void main(String... args) {
		
		if (!NativeNGLink.loadedSuccessfully()) System.out.println("NOT LOADED");
		
		//NativeNGLink nglink = new NativeNGLink();
		
		//nglink.detachNGSpice();
		
	}
	
}
