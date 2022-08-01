using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LCS_Konsole
{
    class LCSConsole
    {

        public static NGLinkHandler ngHandler;

        static void Main(string[] args)
        {
            Console.WriteLine("################################################");
            Console.WriteLine("# LCS Console                                  #");
            Console.WriteLine("# using ngspice shared libary 36               #");
            Console.WriteLine("################################################\n");

            ngHandler = new NGLinkHandler("nglink.dll", "ngspice36.dll", Console.WriteLine);
            
            if (!ngHandler.InitNGLink())
            {
                Console.WriteLine("Failed to initialise nglinker!");
                ngHandler = null;
            }
            
            while (true)
            {

                Console.Write("\n> ");
                string line = Console.ReadLine();
                string[] arguments = line.Split(' ');

                switch (arguments[0])
                {
                    case "help":
                        Console.WriteLine("Aviable Commands:");
                        Console.WriteLine("exit - close the LCS console");
                        Console.WriteLine("initspice - Init a new spice instance");
                        Console.WriteLine("destspiec - Destroy the current spice instance");
                        Console.WriteLine("spice [command] - send a ngspice command");
                        
                        break;
                    case "exit":
                        Environment.Exit(0);
                        break;
                    case "initspice":
                        if (ngHandler.InitNGSpice())
                        {
                            Console.WriteLine("New spice instance initialised");
                        }
                        else
                        {
                            Console.WriteLine("Failed to initialise new Spice engine!");
                        }
                        break;
                    case "destspice":
                        if (ngHandler.StopNGSpice())
                        {
                            Console.WriteLine("Spice instance destroyed");
                        }
                        else
                        {
                            Console.WriteLine("Failed to destroy spice instance!");
                        }
                        break;
                    case "spice":
                        if (arguments.Length > 1)
                        {
                            string command = "";
                            for (int i = 1; i < arguments.Length; i++)
                            {
                                command = command + arguments[i] + " ";
                            }
                            ngHandler.SpiceCommand(command);
                        }
                        else
                        {
                            Console.WriteLine("Not enough arguments!");
                        }
                        break;
                    default:
                        Console.WriteLine("Unknown command " + arguments[0] + "!");
                        Console.WriteLine("Use help for a list of aviable commands.");
                        break;
                }

            }

        }

    }
}
