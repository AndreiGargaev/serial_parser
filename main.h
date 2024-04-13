//
// Created by andrei on 21.02.24.
//

#ifndef TEST_PARSER_MAIN_H
#define TEST_PARSER_MAIN_H

void ReceiveData();
void ReceiveData1();

typedef struct
{
    short head;
    short  sync;
    short  ch1;
    short  ch2;
    short  accx;
    short  accy;
    short  accz;
    short  tail;
} TRecord;

typedef struct
{
    short  ch1;
    short  ch2;
    short  accx;
    short  accy;
    short  accz;
    short  sample;
    short  second;
    short  temp;
} TDecodedRecord;

union Code
{
    TRecord m_Record;
    unsigned char m_Record_raw[sizeof(TRecord)];
};

short  m_CurrentSecond;
short  m_CurrentTemp;
int m_CurrentRecordMatchIndex;

TDecodedRecord m_DecodedRecord;
Code code;

typedef void (*TDataCallbackFunc)(TDecodedRecord *);
TDataCallbackFunc m_pDataCallbackFunc;

#endif //TEST_PARSER_MAIN_H
