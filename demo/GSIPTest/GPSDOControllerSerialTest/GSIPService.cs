using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GPSDOControllerSerialTest
{
    public enum GSIP_OPERATION
    {
        ReadID = 0,
        ReadVersion,
        ReadFreq,
        WriteFreq,
        ReadCounter,
        WriteDAC,
        ReadLowestFreq,
        WriteLowestFreq,
        ReadHighestFreq,
        WriteHighestFreq,
        ReadCenterFreq,
        WrtieCenterFreq,
        ReadP,
        WriteP,
        ReadI,
        WriteI,
        ReadD,
        WriteD,
        ReadPIDSwitch,
        WritePIDSwitch,
        ReadFilterWindow,
        WriteFilterWindow,
        ReadFilterSwitch,
        WriteFilterSwitch,
        ID = 80,
        Version,
        Freq,
        Counter,
        LowestFreq,
        HighestFreq,
        CenterFreq,
        P,
        I,
        D,
        PIDSwitch,
        FilterWindow,
        FilterSwitch,
        DebugInfo
    }

    

    public class GSIP_Message
    {
        public Dictionary<GSIP_OPERATION, string> GSIP_Type_dict = new Dictionary<GSIP_OPERATION, string>();
        private void initTypeDict()
        {
            this.GSIP_Type_dict[GSIP_OPERATION.ReadID] = "";

            this.GSIP_Type_dict[GSIP_OPERATION.ID] = "string";
        }

        private string header;
        private GSIP_OPERATION operation;
        private byte size;
        private object payload;
        private byte crc7;

        public string Header
        {
            get
            {
                return header;
            }

            set
            {
                header = value;
            }
        }

        public GSIP_OPERATION Operation
        {
            get
            {
                return operation;
            }

            set
            {
                operation = value;
            }
        }

        public byte Size
        {
            get
            {
                return size;
            }

            set
            {
                size = value;
            }
        }

        public object Payload
        {
            get
            {
                return payload;
            }

            set
            {
                payload = value;
            }
        }

        public byte Crc7
        {
            get
            {
                return crc7;
            }

            set
            {
                crc7 = value;
            }
        }

        public GSIP_Message(string _header, GSIP_OPERATION _operation, byte _size, object _payload, byte _crc7)
        {
            this.Header = _header;
            this.Operation = _operation;
            this.Size = _size;
            this.Payload = _payload;
            this.Crc7 = _crc7;
        }

        public GSIP_Message(GSIP_OPERATION _operation, byte _size, object _payload)
        {
            this.Header = "#S";
            this.Operation = _operation;
            this.Size = _size;
            this.Payload = _payload;

            byte[] buffer = new byte[this.Size + 2];
            buffer[0] = (byte)this.Operation;
            buffer[1] = (byte)this.Size;

            if (this.Size > 0)
            {
                byte[] payloadBuffer = this.getPayloadBuffer();
                payloadBuffer.CopyTo(buffer, 2);
            }
            
            this.Crc7 = GSIPService.calcCrc7(buffer, this.Size + 2);
        }

        private byte[] getPayloadBuffer()
        {
            byte[] payloadBuffer = null;

            if (this.Size > 0)
            {
                payloadBuffer = new byte[this.Size];
                switch (this.GSIP_Type_dict[this.Operation])
                {
                    case "string":
                        string payloadStr = (string)this.Payload;
                        payloadBuffer = Encoding.ASCII.GetBytes(payloadStr);
                        break;

                    case "int":
                        int payloadInt = (int)this.Payload;
                        payloadBuffer = BitConverter.GetBytes(payloadInt);
                        break;
                }
            }

            return payloadBuffer;
        }

        public byte[] getMessageBuffer()
        {
            byte[] buffer = new byte[this.Size + 5];
            Encoding.ASCII.GetBytes(this.Header).CopyTo(buffer, 0);
            buffer[2] = (byte)this.Operation;
            buffer[3] = (byte)this.Size;
            getPayloadBuffer().CopyTo(buffer, 4);

            byte[] crcBuffer = new byte[this.Size + 2];
            for (int i = 0; i < this.Size + 2; i++)
            {
                crcBuffer[i] = buffer[i + 2];
            }

            buffer[this.Size + 4] = GSIPService.calcCrc7(crcBuffer, this.Size + 2);
 
            return buffer;
        }
    }

    class GSIPService
    {
        public static void writeMessage(SerialPort serialPort, GSIP_Message msg)
        {
            if (serialPort.IsOpen)
            {
                byte[] msgBuf = msg.getMessageBuffer();
                serialPort.Write(msgBuf, 0, msgBuf.Length);
            }
        }

        // TODO : parse message

        public static byte calcCrc7(byte[] src, int length)
        {
            byte crc7 = src[0]; // initial reminder = 0x0
            byte pn = 0x89; // polynominal

            int bitCount = 0;
            int byteCount = 1;

            byte nextByte = 0;
            if (length != 1)
                nextByte = src[byteCount];

            while (bitCount < length * 8)
            {
                // 1. xor if MSB = 1
                if ((crc7 & 0x80) != 0)
                    crc7 ^= pn;

                // 2. move next bit into LSB of crc7
                // store MSB of nextByte in nextBit
                byte nextBit = (byte)(nextByte | 0x7f);
                nextBit = (byte)(nextBit >> 7);
                // now, nextBit goes into crc7
                crc7 = (byte)(crc7 << 1);
                crc7 = (byte)(crc7 | nextBit);

                bitCount++;

                // 3. prepare nextByte
                // move next bit to MSB of nextByte
                nextByte = (byte)(nextByte << 1);
                // move next byte of src into nextByte if all 8 bits are used
                if (bitCount % 8 == 0)
                {
                    byteCount++;
                    if (byteCount == length)
                        nextByte = 0;
                    else
                        nextByte = src[byteCount];
                }
            }

            // move 1 bit right since the resulted 7 bits should be stored in lower bits
            return (byte)(crc7 >> 1);
        }
    }
}
