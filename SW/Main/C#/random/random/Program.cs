using System;
using System.IO;

namespace random
{
    class Program
    {
        private static void Main()
        {
            var rand = new Random();
            var fileName = "random.dat";
            var buffer = new byte[1024];
            BinaryWriter bw = new BinaryWriter(File.Open(fileName, FileMode.Create));
            //for (int i = 0; i < 1024; i++)
            //{              
            //    bw.Write(rand.Next());
            //}
            rand.NextBytes(buffer);
            bw.Write(buffer);
            bw.Close();
        }
    }
}
