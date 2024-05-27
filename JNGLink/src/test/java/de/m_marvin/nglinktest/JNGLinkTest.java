package de.m_marvin.nglinktest;

import java.io.File;

import de.m_marvin.nglink.NativeNGLink;
import de.m_marvin.nglink.NativeNGLink.INGCallback;
import de.m_marvin.nglink.NativeNGLink.NGComplex;
import de.m_marvin.nglink.NativeNGLink.PlotDescription;
import de.m_marvin.nglink.NativeNGLink.VectorDescription;
import de.m_marvin.nglink.NativeNGLink.VectorInfo;
import de.m_marvin.nglink.NativeNGLink.VectorValue;
import de.m_marvin.nglink.NativeNGLink.VectorValuesAll;

// Is not part of the lib.

public class JNGLinkTest {
	
	public static void main(String... args) throws InterruptedException {
		
		File runDir = new File(new File("").getAbsolutePath());
		File initFile = new File(runDir, "..\\share\\ngspice\\scripts\\spinit");
		
		initFile.getParentFile().mkdirs();
		
		System.out.println(initFile);
		
		//System.exit(0);
		
//			try {
//				test();
//			} catch (InterruptedException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
//		});
//		Thread t2 = new Thread(() -> {
//			try {
//				test();
//			} catch (InterruptedException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
//		});
//		
//		t1.start();
//		//Thread.sleep(100);
//		t2.start();
		
		NativeNGLink nglik = new NativeNGLink();
		
		INGCallback callback = new INGCallback() {
			
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
				System.out.println("NGSpice Detached");
				//nglink.detachNGSpice();
			}
		};

		nglik.initNGLink(callback);
		
		test1(nglik);
		
//		try {
//			//CompletableFuture.runAsync(() -> test1(nglik)).get();
//			CompletableFuture.runAsync(() -> test1(nglik)).get();
//		} catch (InterruptedException | ExecutionException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
		
	}
	
	public static Object ngLinkLock = new Object();
	
	public static void test1(NativeNGLink nglink) {
		synchronized (ngLinkLock) {
			
			
			//nglink.initNGSpice("E:\\GitHub\\LIBRARY-NGLink\\JNGLink\\src\\main\\resources\\natives\\libngspice-0.dll");
			//nglink.initNGSpice("E:\\GitHub\\LIBRARY-NGLink\\JNGLink\\src\\main\\resources\\natives\\ngspice42_x64.dll");
			nglink.initNGSpice();
			
//			nglink.loadCircuit("test circuit\r\n"
//					+ "V1 in gnd 10\n"
//					+ "R1 in out 1k\n"
//					+ "R2 out gnd 1k\n"
//					+ ".end\n"
//					+ ".tran 1 10\n"
//					+ "");
			
			nglink.execCommand("version -f");
			String c = "ingame-level-circuit\r\n"
					+ "* Component Block{industriacore:power_source} BlockPos{x=4, y=-60, z=0}\r\n"
					+ "* Current limited power source [ i_nominal=1.7391304347826086, v_nominal=230 ]\r\n"
					+ "R1_0 node[pos4_-60_0_id0_lid2_lnmpower_shunt] node[pos4_-60_0_id0_lid0_lnml] 0.001\r\n"
					+ "I1_0 N003_0 N002_0 1.7391304347826086\r\n"
					+ "V1_0 N001_0 N002_0 230\r\n"
					+ "I2_0 N003_0 node[pos4_-60_0_id0_lid1_lnmn] 1.7391304347826086\r\n"
					+ "*S4_0 node[pos4_-60_0_id0_lid2_lnmpower_shunt] N001_0 N001_0 node[pos4_-60_0_id0_lid2_lnmpower_shunt] source_diosw\r\n"
					+ "*S5_0 N003_0 N002_0 N002_0 N003_0 source_diosw\r\n"
					+ "*S6_0 N003_0 node[pos4_-60_0_id0_lid1_lnmn] node[pos4_-60_0_id0_lid1_lnmn] N003_0 source_diosw\r\n"
					+ "\r\n"
					+ "D1_0 N001_0 node[pos4_-60_0_id0_lid2_lnmpower_shunt] D_ideal\r\n"
					+ "D2_0 N002_0 N003_0 D_ideal\r\n"
					+ "D3_0 node[pos4_-60_0_id0_lid1_lnmn] N003_0 D_ideal\r\n"
					+ "\r\n"
					+ ".model D_ideal D(Ron=0 Roff=1G Vfwd=0 Vrev=1G)\r\n"
					+ "*.model source_diosw sw vt=0 vh=0.001 ron=0.001 roff=10e9\r\n"
					+ "\r\n"
					+ "R0GND node[pos4_-60_0_id0_lid0_lnml] 0 1\r\n"
					+ ".end\r\n"
					+ "";
//			String c = "ingame-level-circuit\r\n"
//					+ "A1 N001 N002 DS1\r\n"
//					+ ".model DS1 sidiode(Roff=1000 Ron=0.7 Rrev=0.2 Vfwd=1\r\n"
//					+ "+ Vrev=10 Revepsilon=0.2 Epsilon=0.2 Ilimit=7 Revilimit=7)\n\r"
//					+ ".end\r\n"
//					+ "";
			
			nglink.loadCircuit(c);
			
			//nglink.execCommand("tran 1 1");
			nglink.execCommand("op");
			
			// Native FIXME
			for (String plot : nglink.listPlots()) {
				System.out.println("PLOTS: " + plot);
			}
			
			System.out.println(nglink.getCurrentPlot());
			
			VectorInfo info = nglink.getVec("out");
			System.out.println("OUT_VEC: " + info.name()+ " " + info.length() + " " + info.type() + " " + info.flags());
			for (double d : info.realdata()) {
				System.out.println(d);
			}
			if (info.complexdata() != null)  {
				for (NGComplex d : info.complexdata()) {
					System.out.println(d.real() + " " + d.imag());
				}
			}
			
			for (String vec : nglink.getVecs(nglink.getCurrentPlot())) {
				System.out.println("VECS: " + vec);
			}
			
			System.out.println(nglink.isRunning());
			
			nglink.execCommand("quit");
//			nglink.detachNGSpice();
			
		}
	}
	
	public static void test() throws InterruptedException {
		synchronized (ngLinkLock) {

			NativeNGLink nglink = new NativeNGLink();
			
			INGCallback callback = new INGCallback() {
				
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
			
			// Do wired calls to check if protection is working
			
			System.out.println("TEST1 " + (!nglink.isInitialized() ? "FALSE" : "TRUE"));
			
			nglink.initNGLink(callback);
			
			Thread.sleep(1000);
			
			nglink.initNGLink(callback);
			
			System.out.println("TEST1 " + (!nglink.isInitialized() ? "FALSE" : "TRUE"));

			System.out.println("TEST2 " + (!nglink.isNGSpiceAttached() ? "FALSE" : "TRUE"));
			
			nglink.initNGSpice();

			Thread.sleep(1000);
			
			nglink.initNGSpice();

			System.out.println("TEST2 " + (!nglink.isNGSpiceAttached() ? "FALSE" : "TRUE"));
			
			nglink.detachNGLink();

			Thread.sleep(1000);
			
			nglink.initNGSpice();
			

			Thread.sleep(1000);
			
			nglink.initNGLink(callback);
			nglink.initNGSpice();
			
			// Test api
			
			nglink.loadCircuit("test circuit\r\n"
					+ "V1 in gnd 10\n"
					+ "R1 in out 1k\n"
					+ "R2 out gnd 1k\n"
					+ ".end\n"
					+ ".tran 1 10\n"
					+ "");
			
			NativeNGLink nglink2 = nglink; // new NativeNGLink();
			
			nglink2.execCommand("tran 1 1");
			
			// Native FIXME
			for (String plot : nglink2.listPlots()) {
				System.out.println("PLOTS: " + plot);
			}
			
			System.out.println(nglink2.getCurrentPlot());
			
			VectorInfo info = nglink2.getVec("out");
			System.out.println("OUT_VEC: " + info.name()+ " " + info.length() + " " + info.type() + " " + info.flags());
			for (double d : info.realdata()) {
				System.out.println(d);
			}
			if (info.complexdata() != null)  {
				for (NGComplex d : info.complexdata()) {
					System.out.println(d.real() + " " + d.imag());
				}
			}
			
			for (String vec : nglink2.getVecs(nglink2.getCurrentPlot())) {
				System.out.println("VECS: " + vec);
			}
			
			System.out.println(nglink2.isRunning());

			
//			Scanner input = new Scanner(System.in);
//
//			while (true) {
//				
//				String command = input.nextLine();
//				if (command.equals("exit")) break;
//				
//				nglink2.execCommand(command);
//				
//			}
//			
//			input.close();
			
			nglink.detachNGSpice();
			
			System.out.println("All methods tested without any exception! :D");
			
		}
	}
	
}
