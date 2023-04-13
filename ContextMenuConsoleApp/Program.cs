using System;
using ContextMenuRegistration;

namespace ContextMenuConsoleApp
{
    class Program
    {
        static void Main(string[] args)
        {
            StartUp.Main(args);

            Console.WriteLine("Press any key to exit...");
            Console.ReadKey();
        }
    }
}
