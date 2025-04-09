using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace CsMemcpyIOTest
{
    internal class FileName
    {
        static void Main()
        {
            const int port = 12345;
            const string serverIp = "127.0.0.1";

            const int PACKETNUM = 100;

            ClientTcp test = new ClientTcp();
            ResHandler handler = new ResHandler();

            ClientTcp.Set(test);
            ResHandler.Set(handler);

            ResHandler.Instance.Init();

            test.Restart();

            for(int i = 0; i < PACKETNUM; i++)
            {
                MySerializeLib.SerializeLib slib = new MySerializeLib.SerializeLib();

                slib.Init(60);

                TestObject to = new TestObject();

                to.sdata = (short)(1 * i);
                to.idata = 2 * i;
                to.ldata = 3 * i;
                to.usdata = (ushort)(4 * i);
                to.uidata = (uint)(5 * i);
                to.uldata = (ulong)(6 * i);

                to.fdata = 1.5f * i;
                to.ddata = 2.5 * i;

                to.strData = "hello" + i.ToString();

                slib.Push(to.sdata);
                slib.Push(to.idata);
                slib.Push(to.ldata);
                slib.Push(to.usdata);
                slib.Push(to.uidata);
                slib.Push(to.uldata);

                slib.Push(to.fdata);
                slib.Push(to.ddata);

                slib.Push(to.strData);


                test.EnqueueSendData(slib.GetData(), (uint)slib.GetSize());
            }

            Stopwatch sw = new Stopwatch();

            sw.Start();

            Task task = test.ConnectToTcpServer(serverIp, port);
            task.Wait();

            sw.Stop();

            Console.WriteLine($"Elapsed Time : {sw.ElapsedMilliseconds}ms.");
            Console.ReadKey();
        }
    }
}
