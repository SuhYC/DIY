using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using CS_MySerializeLib;

namespace CsMemcpyIOTest
{
    internal class FileName
    {
        static void Main()
        {
            MySerializeLib.SerializeLib slib = new MySerializeLib.SerializeLib();

            slib.Init(30);

            bool ret = slib.Push(4);
            if (!ret)
            {
                Console.WriteLine($"iWhat");
            }

            ret = slib.Push(1.5);
            if (!ret)
            {
                Console.WriteLine($"dWhat");
            }

            ret = slib.Push("Hello");
            if (!ret)
            {
                Console.WriteLine($"sWhat");
            }

            Console.WriteLine($"Size : {slib.GetSize()}, Cap : {slib.GetCap()}");

            MySerializeLib.DeserializeLib dlib = new MySerializeLib.DeserializeLib(slib.GetData(), slib.GetSize());

            int ival = 0;
            double dval = 0.0;
            string str = null;

            ret = dlib.Get(ref ival);
            if (ret)
            {
                Console.WriteLine($"int : {ival}, remainSize : {dlib.GetRemainSize()}");
            }
            else
            {
                Console.WriteLine($"Failed");
            }

            ret = dlib.Get(ref dval);
            if (ret)
            {
                Console.WriteLine($"double : {dval}, remainSize : {dlib.GetRemainSize()}");
            }
            else
            {
                Console.WriteLine($"Failed");
            }

            ret = dlib.Get(ref str);
            if (ret)
            {
                Console.WriteLine($"string : {str}, remainSize : {dlib.GetRemainSize()}");
            }
            else
            {
                Console.WriteLine($"Failed");
            }

            Console.ReadKey();
        }
    }
}
