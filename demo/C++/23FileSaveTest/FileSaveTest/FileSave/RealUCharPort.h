#ifndef REALUCHARPORT_H
#define REALUCHARPORT_H

#pragma once

class RealUCharPort
{
public:
    virtual void pushPacket(unsigned char* pPkt,unsigned int iLen)=0;
};

#endif // REALUCHARPORT_H
