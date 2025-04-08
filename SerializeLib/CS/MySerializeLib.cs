using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CS_MySerializeLib
{
    internal class MySerializeLib
    {
        public const int MAX_STRING_SIZE = 1000;

        public class SerializeLib
        {
            public SerializeLib()
            {
                data = null;
                size = 0;
                capacity = 0;
            }

            public bool Init(int cap_)
            {
                try
                {
                    data = new byte[cap_];
                }
                catch (Exception e)
                {
                    Console.WriteLine($"SerializeLib::Init : {e.Message}");
                    return false;
                }

                capacity = cap_;
                return true;
            }

            public bool Resize(int cap_)
            {
                if (size > cap_)
                {
                    return false;
                }

                if (capacity == cap_)
                {
                    return true;
                }

                byte[] newData;

                try
                {
                    newData = new byte[cap_];
                }
                catch (Exception e)
                {
                    Console.WriteLine($"SerializeLib::Resize : {e.Message}");
                    return false;
                }

                if (data != null)
                {
                    Buffer.BlockCopy(data, 0, newData, 0, (int)size);
                }

                data = newData;
                capacity = cap_;

                return true;
            }

            unsafe public bool Push<T>(T rhs_) where T : struct
            {
                if (typeof(T) == typeof(int) || typeof(T) == typeof(uint) ||
                    typeof(T) == typeof(float) || typeof(T) == typeof(double) ||
                    typeof(T) == typeof(ulong) || typeof(T) == typeof(long) ||
                    typeof(T) == typeof(short) || typeof(T) == typeof(ushort))
                {
                    int tSize = System.Runtime.InteropServices.Marshal.SizeOf<T>();

                    if (size + tSize > capacity)
                    {
                        return false;
                    }

                    fixed (byte* pBuffer = data)
                    {
                        T* pT = (T*)(pBuffer + size);
                        *pT = rhs_;
                    }

                    size += tSize;

                    return true;
                }
                else
                {
                    throw new ArgumentException("Unsupported type");
                }
            }

            public bool Push(string rhs_)
            {
                byte[] bytes = Encoding.GetEncoding("euc-kr").GetBytes(rhs_);

                uint len = (uint)bytes.Length;

                if (size + len + sizeof(uint) > capacity)
                {
                    return false;
                }

                data[size] = (byte)(len & 0xFF);
                data[size + 1] = (byte)(len >> 8 & 0xFF);
                data[size + 2] = (byte)(len >> 16 & 0xFF);
                data[size + 3] = (byte)(len >> 24 & 0xFF);

                size += 4;

                Buffer.BlockCopy(bytes, 0, data, size, bytes.Length);

                size += (int)len;

                return true;
            }

            public int GetSize() { return size; }
            public int GetCap() { return capacity; }
            public byte[] GetData() { return data; }

            private byte[] data;
            private int size;
            private int capacity;
        }

        public class DeserializeLib
        {
            public DeserializeLib(byte[] bytes, int size_)
            {
                if (bytes != null)
                {
                    try
                    {
                        data = new byte[size_];
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine($"DeserializeLib() : {e.Message}");
                    }

                    Buffer.BlockCopy(bytes, 0, data, 0, size_);
                    size = size_;
                }
            }

            unsafe public bool Get<T>(ref T rhs_) where T : struct
            {
                if (typeof(T) == typeof(int) || typeof(T) == typeof(uint) ||
                    typeof(T) == typeof(float) || typeof(T) == typeof(double) ||
                    typeof(T) == typeof(ulong) || typeof(T) == typeof(long) ||
                    typeof(T) == typeof(short) || typeof(T) == typeof(ushort))
                {
                    int tSize = System.Runtime.InteropServices.Marshal.SizeOf<T>();

                    if (size - idx < tSize)
                    {
                        return false;
                    }

                    fixed (byte* pbyte = data)
                    {
                        T* pT = (T*)(pbyte + idx);
                        rhs_ = *pT;
                    }
                    idx += tSize;

                    return true;
                }
                else
                {
                    throw new ArgumentException("Unsupported type");
                }
            }

            public bool Get(ref string rhs_)
            {
                if (size - idx < sizeof(uint))
                {
                    return false;
                }
                uint len = (uint)(data[idx] | data[idx + 1] << 8 | data[idx + 2] << 16 | data[idx + 3] << 24);

                if (len == 0 || len > MAX_STRING_SIZE || size - idx - sizeof(uint) < (int)len)
                {
                    return false;
                }

                idx += sizeof(uint);

                byte[] SubBytes = new byte[len];
                Buffer.BlockCopy(data, idx, SubBytes, 0, (int)len);

                rhs_ = Encoding.GetEncoding("euc-kr").GetString(SubBytes);

                return true;
            }

            public int GetRemainSize() { return size - idx; }

            private byte[] data;
            private int size;
            private int idx;
        }
    }
}
