package de.m_marvin.nglinktest;

import java.io.FileNotFoundException;
import java.util.Scanner;

import de.m_marvin.nglink.NativeExtractor;
import de.m_marvin.nglink.NativeNGLink;
import de.m_marvin.nglink.NativeNGLink.NGCallback;
import de.m_marvin.nglink.NativeNGLink.NGComplex;
import de.m_marvin.nglink.NativeNGLink.PlotDescription;
import de.m_marvin.nglink.NativeNGLink.VectorDescription;
import de.m_marvin.nglink.NativeNGLink.VectorInfo;
import de.m_marvin.nglink.NativeNGLink.VectorValue;
import de.m_marvin.nglink.NativeNGLink.VectorValuesAll;

public class JNGLinkTest {
	
	public static void main(String... args) {
		
		if (!NativeNGLink.loadedSuccessfully()) System.out.println("NOT LOADED");
		
		NativeNGLink nglink = new NativeNGLink();
		
		NGCallback callback = new NGCallback() {
			
			@Override
			public void reciveVecData(VectorValuesAll vecData, int vectorCount) {
				System.out.println("Recive Vector Data x " + vectorCount);
				
				System.out.println(vecData.count());
				System.out.println(vecData.index());
				
				for (VectorValue info : vecData.values()) {
					System.out.print(info.name());
					System.out.print(" " + info.realdata());
					System.out.print(" " + info.complexdata());
					System.out.print(" " + info.isComplex());
					System.out.println(" " + info.isScale());
				}
			}
			
			@Override
			public void reciveInitData(PlotDescription plotInfo) {
				System.out.println("Recive plot data");
				
				System.out.println(plotInfo.name());
				System.out.println(plotInfo.title());
				System.out.println(plotInfo.vectorCount());
				
				for (VectorDescription info : plotInfo.vectorInfo()) {
					System.out.print(info.name());
					System.out.print(" " + info.number());
					System.out.println(" " + info.isReal());
				}
				
			}
			
			@Override
			public void log(String s) {
				System.out.println("JNGLink: " + s);
			}
			
			@Override
			public void detacheNGSpice() {
				System.out.println("Detache NGSpice");
				nglink.detachNGSpice();
			}
		};
		
		nglink.initNGLink(callback);
		
		try {
			nglink.initNGSpice(NativeExtractor.findNative("ngspice"));
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		};
		
		Scanner input = new Scanner(System.in);

		nglink.loadCircuit("test circuit\r\n"
				+ "V1 in gnd 10\r\n"
				+ "R1 in out 1k\r\n"
				+ "R2 out gnd 1k\r\n"
				+ ".end\r\n"
				+ ".tran 1 10\r\n"
				+ "");
		
		nglink.execCommand("tran 1 1");
		
		// Native FIXME
		for (String plot : nglink.listPlots()) {
			System.out.println(plot);
		}
		
		System.out.println(nglink.getCurrentPlot());
		
		VectorInfo info = nglink.getVec("out");
		System.out.println(info.name()+ " " + info.length() + " " + info.type() + " " + info.flags());
		for (double d : info.realdata()) {
			System.out.println(d);
		}
		if (info.complexdata() != null)  {
			for (NGComplex d : info.complexdata()) {
				System.out.println(d.real() + " " + d.imag());
			}
		}
		
		for (String vec : nglink.getVecs(nglink.getCurrentPlot())) {
			System.out.println(vec);
		}
		
		System.out.println(nglink.isRunning());
		
		while (true) {
			
			String command = input.nextLine();
			if (command.equals("exit")) break;
			
			nglink.execCommand(command);
			
		}
		
		input.close();
		
		nglink.detachNGSpice();
		
	}
	
}
