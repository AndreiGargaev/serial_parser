#include <iostream>
#include <cstdlib>
#include "main.h"

void DataCallbackFunc(TDecodedRecord *pData)
{
    // writing to file
    //fprintf(fOutput, "%d;%d;%d;%d;%d;%d;%d\n", pData->second, pData->ch1, pData->ch2, pData->accx, pData->accy, pData->accz, pData->temp);

    std::cout <<  "pData->second: " <<  pData->second << std::endl
            <<  "pData->ch1: " <<  pData->ch1 << std::endl
            <<  "pData->ch2: " <<  pData->ch2 << std::endl
            <<  "pData->accx: " <<  pData->accx << std::endl
            <<  "pData->accy: " <<  pData->accy << std::endl
            <<  "pData->accz: " <<  pData->accz << std::endl
            <<  "pData->temp: " <<  pData->temp << std::endl;

}

int main() {
    std::cout << "test parser" << std::endl;

    m_pDataCallbackFunc = DataCallbackFunc;

    ReceiveData1();

    return 0;
}

int Read(unsigned char* buf, int size )
{
    buf[0] = buf[1] = 0;
    for(int i=2 ; i<size; i++) {
        if(i == 14 || i == 15) {
            buf[i] = 0xff;
            continue;
        }
        buf[i] = rand()/100000;
    }
}

void ReceiveData()
{
    unsigned char buffer[1024];
    int i, j;
    int readCount;

    readCount = Read(buffer, sizeof(buffer));
    for (i=0; i<readCount; i++)
    {
        code.m_Record_raw[m_CurrentRecordMatchIndex] = buffer[i];
        switch (m_CurrentRecordMatchIndex)
        {
            case 0:
            case 1:
                if (buffer[i]==0)
                {
                    m_CurrentRecordMatchIndex++;
                }
                else
                {
                    m_CurrentRecordMatchIndex = 0;
                    printf("unsync\n");
                }
                break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
                m_CurrentRecordMatchIndex++;
                break;
            case 14:
                if (buffer[i]==0xff)
                {
                    m_CurrentRecordMatchIndex++;
                }
                else
                {
                    m_CurrentRecordMatchIndex = (buffer[i]==0) ? 1 : 0;
                    printf("unsync\n");
                }
                break;
            case 15:
                if (buffer[i]==0xff)
                {
                    // parsing sync word. High 13 bits have different meaning for different low 3 bits
                    // 1     - second (sample number==0)
                    // 3     - temperature (sample number==1)
                    // other - sample number
                    switch (code.m_Record.sync & 7)
                    {
                        case 1:
                            m_CurrentSecond = (code.m_Record.sync >> 3);
                            m_DecodedRecord.sample = 0;
                            break;
                        case 3:
                            m_CurrentTemp = (code.m_Record.sync >> 3);
                            m_DecodedRecord.sample = 1;
                            break;
                        default:
                            m_DecodedRecord.sample = (code.m_Record.sync >> 3);
                            break;
                    }
                    // setting other fields
                    m_DecodedRecord.second = m_CurrentSecond;
                    m_DecodedRecord.temp = m_CurrentTemp;
                    m_DecodedRecord.ch1 = code.m_Record.ch1;
                    m_DecodedRecord.ch2 = code.m_Record.ch2;
                    m_DecodedRecord.accx = code.m_Record.accx;
                    m_DecodedRecord.accy = code.m_Record.accy;
                    m_DecodedRecord.accz = code.m_Record.accz;
                    // calling callback
                    m_pDataCallbackFunc(&m_DecodedRecord);
                    m_CurrentRecordMatchIndex=0;
                }
                else
                {
                    m_CurrentRecordMatchIndex = (buffer[i]==0) ? 1 : 0;
                    printf("unsync\n");
                }
                break;
        }
    }
}

void ReceiveData1()
{
    unsigned char buffer[1024];
    int readCount, i;
    bool isStart = false;

    readCount = Read(buffer, sizeof(buffer));
    for (i=1; i<readCount; i++)
    {
        if(buffer[i] == 0 && buffer[i-1] == 0) {
            m_CurrentRecordMatchIndex = 1;
            isStart = true;
            continue;
        }
        if(isStart) {
            code.m_Record_raw[++m_CurrentRecordMatchIndex] = buffer[i];
        }
        if(m_CurrentRecordMatchIndex == 15 && buffer[i] == 255 && buffer[i-1] == 255) {
            isStart = false;
            switch (code.m_Record.sync & 7)
            {
                case 1:
                    m_CurrentSecond = (code.m_Record.sync >> 3);
                    m_DecodedRecord.sample = 0;
                    break;
                case 3:
                    m_CurrentTemp = (code.m_Record.sync >> 3);
                    m_DecodedRecord.sample = 1;
                    break;
                default:
                    m_DecodedRecord.sample = (code.m_Record.sync >> 3);
                    break;
            }
            // setting other fields
            m_DecodedRecord.second = m_CurrentSecond;
            m_DecodedRecord.temp = m_CurrentTemp;
            m_DecodedRecord.ch1 = code.m_Record.ch1;
            m_DecodedRecord.ch2 = code.m_Record.ch2;
            m_DecodedRecord.accx = code.m_Record.accx;
            m_DecodedRecord.accy = code.m_Record.accy;
            m_DecodedRecord.accz = code.m_Record.accz;
            // calling callback
            m_pDataCallbackFunc(&m_DecodedRecord);
            m_CurrentRecordMatchIndex=0;
        }

    }
}