#pragma once
#include "stdafx.h"

class d3dUtil
{
    public:
        static UINT CalcConstantBufferByteSize(UINT byteSize){
            return (byteSize + 255) & ~255;
        }

};