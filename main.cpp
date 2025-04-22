#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <fstream>

#include "FlvParser.h"

using namespace std;

void Process(fstream &fin, const char *filename);

int main(int argc, char *argv[])
{
    cout << "Hi, this is FLV parser test program!\n";

    if (argc != 3)
    {
        cout << "FlvParser.exe [input flv] [output flv]" << endl;
        return 0;
    }

    fstream fin;
    fin.open(argv[1], ios_base::in | ios_base::binary); //ios_base操作了流的属性
    if (!fin)
        return 0;

    Process(fin, argv[2]);

    fin.close();

    return 1;
}

void Process(fstream &fin, const char *filename)
{
    CFlvParser parser;

    int nBufSize = 2*1024 * 1024;
    int nFlvPos = 0;
    uint8_t *pBuf, *pBak;
    pBuf = new uint8_t[nBufSize];
    pBak = new uint8_t[nBufSize];

    while (1)
    {
        int nReadNum = 0;
        int nUsedLen = 0;
        //第一个参数是字节指针，表示存储的地址，第二个参数是表示缓冲区剩余的可写入空间，确保不会溢出。同时会填充pBuf缓冲区
        //+pBuf是加上偏移地址，加的是字节数
        fin.read((char *)pBuf + nFlvPos, nBufSize - nFlvPos);
        //返回读取的字节数
        nReadNum = fin.gcount();
        if (nReadNum == 0)
            break;
        nFlvPos += nReadNum;

        parser.Parse(pBuf, nFlvPos, nUsedLen);
        if (nFlvPos != nUsedLen)
        {
            //memcpy将一段内存中的数据（字节级）从源地址复制到目标地址
            memcpy(pBak, pBuf + nUsedLen, nFlvPos - nUsedLen);
            memcpy(pBuf, pBak, nFlvPos - nUsedLen);
        }
        nFlvPos -= nUsedLen;
    }
    parser.PrintInfo();
    parser.DumpH264("parser.264");
    parser.DumpAAC("parser.aac");

    //dump into flv
    parser.DumpFlv(filename);

    delete []pBak;
    delete []pBuf;
}
