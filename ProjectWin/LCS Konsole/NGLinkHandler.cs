using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace LCS_Konsole
{

    public class NGLinkHandler
    {

        private LogPrint logPrinter;
        private string ngLinkLib;
        private string ngSpiceLib;
        private bool awaitingDetache;

        public delegate void LogPrint(string print);
        public delegate void DetacheFunc();

        [DllImport("nglink.dll", CallingConvention = CallingConvention.Cdecl)] private static extern int initNGLink(LogPrint printFunc, DetacheFunc detacheFunc);
        [DllImport("nglink.dll", CallingConvention = CallingConvention.Cdecl)] private static extern int initNGSpice(string libName);
        [DllImport("nglink.dll", CallingConvention = CallingConvention.Cdecl)] private static extern int detachNGSpice();
        [DllImport("nglink.dll", CallingConvention = CallingConvention.Cdecl)] private static extern int execCommand(string command);
        
        public NGLinkHandler(string ngLinkLib, string ngSpiceLib, LogPrint logPrinter)
        {
            this.logPrinter = logPrinter;
            this.ngLinkLib = ngLinkLib;
            this.ngSpiceLib = ngSpiceLib;
        }

        public bool InitNGLink()
        {
            try
            {
                return initNGLink(logPrinter, DetacheRequest) > 0;
            } catch (DllNotFoundException)
            {
                logPrinter("nglink dll not found!");
                return false;
            }
        }

        public bool InitNGSpice()
        {
            return initNGSpice(ngSpiceLib) > 0;
        }

        public bool StopNGSpice()
        {
            return detachNGSpice() > 0;
        }

        public void SpiceCommand(string command)
        {
            execCommand(command);
            if (awaitingDetache) ConfirmDetache();
        }

        private void DetacheRequest()
        {
            this.awaitingDetache = true;
        }
        private void ConfirmDetache() 
        {
            this.awaitingDetache = false;
            StopNGSpice();
        }

    }

}
