using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsMemcpyIOTest
{
    internal class ResHandler
    {
        private static ResHandler _instance;
        private string _remainMsg;

        private RingBuffer _buffer;
        private byte[] _processBuffer;
        private byte[] _req;

        private MySerializeLib.DeserializeLib dlib;
        private MySerializeLib.SerializeLib slib;

        private uint[] header;
        private const int MAX_SIZE_OF_PACKET = ClientTcp.MAX_SOCKBUF;

        private int count = 0;
        private const int GOAL = 1000;

        private int lastpercent = 0;

        public static ResHandler Instance
        {
            get { return _instance; }
        }

        public static void Set(ResHandler instance)
        {
            _instance = instance;
        }

        public void Init()
        {
            _remainMsg = string.Empty;
            header = new uint[1];
            _buffer = new RingBuffer();
            _processBuffer = new byte[RingBuffer.BUFSIZE];
            _req = new byte[MAX_SIZE_OF_PACKET];

            dlib = new MySerializeLib.DeserializeLib();
            slib = new MySerializeLib.SerializeLib();
            slib.Init(60);
        }

        public void Restart()
        {
            count = 0;
            lastpercent = 0;
            _buffer.Init();
        }

        public async Task HandlePacketByte(byte[] msg, int size_)
        {
            _buffer.Enqueue(msg, size_);

            try
            {
                int len = _buffer.Dequeue(_processBuffer);
                int idx = 0;

                while (true)
                {
                    // 페이로드가 아예 없음
                    if (len - idx < 5)
                    {
                        _buffer.Enqueue(_processBuffer, len - idx, idx);

                        return;
                    }

                    uint length = (uint)(_processBuffer[idx] | _processBuffer[idx + 1] << 8 | _processBuffer[idx + 2] << 16 | _processBuffer[idx + 3] << 24);

                    // 유효하지 않은 헤더
                    if (length == 0 || length > MAX_SIZE_OF_PACKET)
                    {
                        Console.WriteLine($"ResHandler::HandlePacket : InValid Size : {length}");
                        //await _buffer.EnqueueWithLock(_processBuffer, len - idx, idx);
                        return;
                    }

                    if (len - idx >= length + sizeof(uint))
                    {
                        try
                        {
                            Buffer.BlockCopy(_processBuffer, idx + sizeof(uint), _req, 0, (int)length);
                        }
                        catch(Exception e)
                        {
                            Console.WriteLine($"ResHandler::HandlePacket : Buffer.BlockCopy : {e.Message}");
                        }

                        idx += sizeof(uint) + (int)length;

                        if (count < GOAL)
                        {
                            count++;

                            if (count * 100 / GOAL > lastpercent)
                            {
                                lastpercent++;
                                //Console.WriteLine($"{lastpercent}% Done.");
                            }

                            dlib.Init(_req, (int)length);

                            TestObject to = new TestObject();

                            dlib.Get(ref to.sdata);
                            dlib.Get(ref to.idata);
                            dlib.Get(ref to.ldata);
                            dlib.Get(ref to.usdata);
                            dlib.Get(ref to.uidata);
                            dlib.Get(ref to.uldata);

                            dlib.Get(ref to.fdata);
                            dlib.Get(ref to.ddata);

                            dlib.Get(ref to.strData);

                            Console.WriteLine($"s: {to.sdata}, i:{to.idata}, l:{to.ldata}, " +
                                $"us: {to.usdata}, ui: {to.uidata}, ul: {to.uldata}, " +
                                $"f: {to.fdata}, d: {to.ddata}, str: {to.strData}");

                            slib.Flush();

                            slib.Push(to.sdata);
                            slib.Push(to.idata);
                            slib.Push(to.ldata);
                            slib.Push(to.usdata);
                            slib.Push(to.uidata);
                            slib.Push(to.uldata);

                            slib.Push(to.fdata);
                            slib.Push(to.ddata);

                            slib.Push(to.strData);

                            // echo
                            await ClientTcp.Instance.SendMsg(slib.GetData(), (uint)slib.GetSize());
                        }
                        else
                        {
                            ClientTcp.Instance.End();
                        }

                    }
                    else
                    {
                        _buffer.Enqueue(_processBuffer, len - idx, idx);
                        return;
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine($"ResHandler::HandlePacket : {e.Message}, {e.StackTrace}");
            }
        }
    }
}
