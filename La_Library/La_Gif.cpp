#include "La_Gif.h"


namespace
{
#define _GIF_SWAP(h) ((GIF_BIGE)? ((uint16_t)(h << 8) | (h >> 8)) : h)

    /// <summary>
    /// 跳过 chunk
    /// </summary>
    long _GIF_SkipChunk(uint8_t** buff, long size)
    {
        long skip;
        for (skip = 2, ++size, ++(*buff); ((size -= skip) > 0) && (skip > 1);
            *buff += (skip = 1 + **buff));
        return size;
    }

    /// <summary>
    /// 通过帧图的文件头找到相应的信息
    /// </summary>
    /// <param name="buff">
    /// 图片的数据
    /// </param>
    /// <param name="rpal">
    /// 获取调色板
    /// </param>
    /// <param name="size">
    /// 获取图片大小
    /// </param>
    long _GIF_LoadHeader(unsigned gflg, uint8_t** buff, void** rpal, unsigned fflg, long* size, long flen)
    {
        if (flen && (!(*buff += flen) || ((*size -= flen) <= 0)))
            return -2;  /** v--[ 0x80: "palette is present" flag ]--, **/
        if (flen && (fflg & 0x80))
        { /** local palette has priority | **/
            *rpal = *buff; /** [ 3L: 3 uint8_t color channels ]--,  | **/
            *buff += (flen = 2 << (fflg & 7)) * 3L;       /** <--|  | **/
            return ((*size -= flen * 3L) > 0) ? flen : -1; /** <--'  | **/
        } /** no local palette found, checking for the global one   | **/
        return (gflg & 0x80) ? (2 << (gflg & 7)) : 0;      /** <-----' **/
    }


    /** [ internal function, do not use ] **/
    long _GIF_LoadFrame(uint8_t** buff, long* size, uint8_t* bptr, uint8_t* blen)
    {
        typedef uint16_t GIF_H;
        const long GIF_HLEN = sizeof(GIF_H), /** to rid the scope of sizeof **/
            GIF_CLEN = 1 << 12;             /** code table length: 4096 items **/
        GIF_H accu, mask; /** bit accumulator / bit mask                    **/
        long  ctbl, iter, /** last code table index / index string iterator **/
            prev, curr, /** codes from the stream: previous / current     **/
            ctsz, ccsz, /** code table bit sizes: min LZW / current       **/
            bseq, bszc; /** counters: block sequence / bit size           **/
        uint32_t* code = (uint32_t*)bptr - GIF_CLEN; /** code table pointer **/

        /** preparing initial values **/
        if ((--(*size) <= GIF_HLEN) || !*++(*buff))
            return -4; /** unexpected end of the stream: insufficient size **/
        mask = (GIF_H)((1 << (ccsz = (ctsz = *(*buff - 1)) + 1)) - 1);
        if ((ctsz < 2) || (ctsz > 8))
            return -3; /** min LZW size is out of its nominal [2; 8] bounds **/
        if ((ctbl = (1L << ctsz)) != (mask & _GIF_SWAP(*(GIF_H*)(*buff + 1))))
            return -2; /** initial code is not equal to min LZW size **/
        for (curr = ++ctbl; curr; code[--curr] = 0); /** actual color codes **/

        /** getting codes from stream (--size makes up for end-of-stream mark) **/
        for (--(*size), bszc = -ccsz, prev = curr = 0; ((*size -= (bseq = *(*buff)++) + 1) >= 0) && bseq; *buff += bseq)
            for (; bseq > 0; bseq -= GIF_HLEN, *buff += GIF_HLEN)
                for (accu = (GIF_H)(_GIF_SWAP(*(GIF_H*)*buff) & ((bseq < GIF_HLEN) ? ((1U << (8 * bseq)) - 1U) : ~0U)),
                    curr |= accu << (ccsz + bszc), accu = (GIF_H)(accu >> -bszc),
                    bszc += 8 * ((bseq < GIF_HLEN) ? bseq : GIF_HLEN);
                    bszc >= 0; bszc -= ccsz, prev = curr, curr = accu,
                    accu = (GIF_H)(accu >> ccsz))
                    if (((curr &= mask) & ~1L) == (1L << ctsz))
                    {
                        if (~(ctbl = curr + 1) & 1) /** end-of-data code (ED). **/
                            /** -1: no end-of-stream mark after ED; 1: decoded **/
                            return (*((*buff += bseq + 1) - 1)) ? -1 : 1;
                        mask = (GIF_H)((1 << (ccsz = ctsz + 1)) - 1);
                    } /** ^- table drop code (TD). TD = 1 << ctsz, ED = TD + 1 **/
                    else
                    { /** single-pixel (SP) or multi-pixel (MP) code. **/
                        if (ctbl < GIF_CLEN)
                        { /** is the code table full? **/
                            if ((ctbl == mask) && (ctbl < GIF_CLEN - 1))
                            {
                                mask = (GIF_H)(mask + mask + 1);
                                ccsz++; /** yes; extending **/
                            } /** prev = TD? => curr < ctbl = prev **/
                            code[ctbl] = (uint32_t)prev + (code[prev] & 0xFFF000);
                        } /** appending SP / MP decoded pixels to the frame **/
                        prev = (long)code[iter = (ctbl > curr) ? curr : prev];
                        if ((bptr += (prev = (prev >> 12) & 0xFFF)) > blen)
                            continue; /** skipping pixels above frame capacity **/
                        for (prev++; (iter &= 0xFFF) >> ctsz;
                            *bptr-- = (uint8_t)((iter = (long)code[iter]) >> 24));
                        (bptr += prev)[-prev] = (uint8_t)iter;
                        if (ctbl < GIF_CLEN)
                        { /** appending the code table **/
                            if (ctbl == curr)
                                *bptr++ = (uint8_t)iter;
                            else if (ctbl < curr)
                                return -5; /** wrong code in the stream **/
                            code[ctbl++] += ((uint32_t)iter << 24) + 0x1000;
                        }
                    } /** 0: no ED before end-of-stream mark; -4: see above **/
        return (++(*size) >= 0) ? 0 : -4; /** ^- N.B.: 0 error is recoverable **/
    }

}

/** _________________________________________________________________________
        The main loading function. Returns the total number of frames if the data
        includes proper GIF ending, and otherwise it returns the number of frames
        loaded per current call, multiplied by -1. So, the data may be incomplete
        and in this case the function can be called again when more data arrives,
        just remember to keep SKIP up to date.
        _________________________________________________________________________
        DATA: raw data chunk, may be partial   一般是整个文件
        SIZE: size of the data chunk that`s currently present   data 的大小 或者文件大小
        GWFR: frame writer function, MANDATORY （强制的）
        EAMF: metadata reader function, set to 0 if not needed
        ANIM: implementation-specific data (e.g. a structure or a pointer to it)   给两个函数指针使用的参数入口
        SKIP: number of frames to skip before resuming
     **/
long GIF_Load(void* data, long size,
    void (*gwfr)(void*, struct GIF_WHDR*),
    void (*eamf)(void*, struct GIF_WHDR*),
    void* anim, long skip)
{
    const long    GIF_BLEN = (1 << 12) * sizeof(uint32_t);
    const uint8_t GIF_EHDM = 0x21, /** extension header mark              **/
        GIF_FHDM = 0x2C, /** frame header mark                  **/
        GIF_EOFM = 0x3B, /** end-of-file mark                   **/
        GIF_EGCM = 0xF9, /** extension: graphics control mark   **/
        GIF_EAMM = 0xFF; /** extension: app metadata mark       **/
#pragma pack(push, 1)
    struct GIF_GHDR
    {                       /** ========== GLOBAL GIF HEADER: ========== **/
        uint8_t head[6];     /** 'GIF87a' / 'GIF89a' header signature     **/
        uint16_t xdim, ydim; /** total image width, total image height    **/
        uint8_t flgs;        /** FLAGS:
                              GlobalPlt    bit 7     1: global palette exists
                                                     0: local in each frame
                              ClrRes       bit 6-4   bits/channel = ClrRes+1
                              [reserved]   bit 3     0
                              PixelBits    bit 2-0   |Plt| = 2 * 2^PixelBits
                              **/
        uint8_t bkgd, aspr;  /** background color index, aspect ratio     **/
    } *ghdr = (struct GIF_GHDR*)data; //总的文件头
    struct GIF_FHDR
    {                        /** ======= GIF FRAME MASTER HEADER: ======= **/
        uint16_t frxo, fryo; /** offset of this frame in a "full" image   **/
        uint16_t frxd, fryd; /** frame width, frame height                **/
        uint8_t flgs;        /** FLAGS:
                              LocalPlt     bit 7     1: local palette exists
                                                     0: global is used
                              Interlaced   bit 6     1: interlaced frame
                                                     0: non-interlaced frame
                              Sorted       bit 5     usually 0
                              [reserved]   bit 4-3   [undefined]
                              PixelBits    bit 2-0   |Plt| = 2 * 2^PixelBits
                              **/
    } *fhdr;
    struct GIF_EGCH
    {                       /** ==== [EXT] GRAPHICS CONTROL HEADER: ==== **/
        uint8_t flgs;        /** FLAGS:
                              [reserved]   bit 7-5   [undefined]
                              BlendMode    bit 4-2   000: not set; static GIF
                                                     001: leave result as is
                                                     010: restore background
                                                     011: restore previous
                                                     1--: [undefined]
                              UserInput    bit 1     1: show frame till input
                                                     0: default; ~99% of GIFs
                              TransColor   bit 0     1: got transparent color
                                                     0: frame is fully opaque
                              **/
        uint16_t time;       /** delay in GIF time units; 1 unit = 10 ms  **/
        uint8_t tran;        /** transparent color index                  **/
    } *egch = 0;
#pragma pack(pop)
    struct GIF_WHDR wtmp, whdr = { 0 };
    long desc, blen;
    uint8_t* buff;

    //检验总的文件头
    /** checking if the stream is not empty and has a 'GIF8[79]a' signature,
        the data has sufficient size and frameskip value is non-negative **/
    if (!ghdr || (size <= (long)sizeof(*ghdr)) || (*(buff = ghdr->head) != 71)
        || (buff[1] != 73) || (buff[2] != 70) || (buff[3] != 56) || (skip < 0)
        || ((buff[4] != 55) && (buff[4] != 57)) || (buff[5] != 97) || !gwfr)
        return 0;

    /** skipping the global header and palette **/
    //跳过总的文件头
    buff = (uint8_t*)(ghdr + 1) + _GIF_LoadHeader(ghdr->flgs, 0, 0, 0, 0, 0L) * 3L;
    if ((size -= buff - (uint8_t*)ghdr) <= 0)
        return 0;

    whdr.xdim = _GIF_SWAP(ghdr->xdim);
    whdr.ydim = _GIF_SWAP(ghdr->ydim);
    for (whdr.bptr = buff, whdr.bkgd = ghdr->bkgd, blen = --size;
        (blen >= 0) && ((desc = *whdr.bptr++) != GIF_EOFM); /** sic: '>= 0' **/
        //遍历所有的 chunk ，直到找到 FHDM
        blen = _GIF_SkipChunk(&whdr.bptr, blen) - 1) /** count all frames **/
        if (desc == GIF_FHDM)//如果有帧图的文件头
        {
            fhdr = (struct GIF_FHDR*)whdr.bptr;//一帧图片的文件头
            if (_GIF_LoadHeader(ghdr->flgs, &whdr.bptr, (void**)&whdr.cpal, fhdr->flgs, &blen, sizeof(*fhdr)) <= 0)
                break;
            whdr.frxd = _GIF_SWAP(fhdr->frxd);
            whdr.fryd = _GIF_SWAP(fhdr->fryd);
            whdr.frxo = (whdr.frxd > whdr.frxo) ? whdr.frxd : whdr.frxo;
            whdr.fryo = (whdr.fryd > whdr.fryo) ? whdr.fryd : whdr.fryo;
            whdr.ifrm++;//计算帧数   --- 当前帧数
        }
    blen = whdr.frxo * whdr.fryo * (long)sizeof(*whdr.bptr);
    GIF_MGET(whdr.bptr, (unsigned long)(blen + GIF_BLEN + 2), anim, 1);
    whdr.nfrm = (desc != GIF_EOFM) ? -whdr.ifrm : whdr.ifrm;  //总的帧数


    for (whdr.bptr += GIF_BLEN, whdr.ifrm = -1; blen /** load all frames **/
        && (skip < ((whdr.nfrm < 0) ? -whdr.nfrm : whdr.nfrm)) && (size >= 0);
        //下一帧图片
        size = (desc != GIF_EOFM) ? ((desc != GIF_FHDM) || (skip > whdr.ifrm)) ?
        _GIF_SkipChunk(&buff, size) - 1 : size - 1 : -1)
        if ((desc = *buff++) == GIF_FHDM)
        {   /** found a frame **/
            whdr.intr = !!((fhdr = (struct GIF_FHDR*)buff)->flgs & 0x40);
            *(void**)&whdr.cpal = (void*)(ghdr + 1); /** interlaced? -^ **/
            whdr.clrs = _GIF_LoadHeader(ghdr->flgs, &buff, (void**)&whdr.cpal,
                fhdr->flgs, &size, sizeof(*fhdr));
            if ((skip <= ++whdr.ifrm) && ((whdr.clrs <= 0)
                || (_GIF_LoadFrame(&buff, &size,
                    whdr.bptr, whdr.bptr + blen) < 0)))
                size = -(whdr.ifrm--) - 1; /** failed to load the frame **/
            else if (skip <= whdr.ifrm)
            {
                whdr.frxd = _GIF_SWAP(fhdr->frxd);
                whdr.fryd = _GIF_SWAP(fhdr->fryd);
                whdr.frxo = _GIF_SWAP(fhdr->frxo);
                whdr.fryo = _GIF_SWAP(fhdr->fryo);
                whdr.time = (egch) ? _GIF_SWAP(egch->time) : 0;
                whdr.tran = (egch && (egch->flgs & 0x01)) ? egch->tran : -1;
                whdr.time = (egch && (egch->flgs & 0x02)) ? -whdr.time - 1 : whdr.time;
                whdr.mode = (egch && !(egch->flgs & 0x10)) ?
                    (egch->flgs & 0x0C) >> 2 : GIF_NONE;
                egch = 0;
                wtmp = whdr;
                gwfr(anim, &wtmp); /** passing the frame to the caller **/
            }
        }
        else if (desc == GIF_EHDM)
        { /** found an extension **/
            if (*buff == GIF_EGCM) /** graphics control ext. **/
                egch = (struct GIF_EGCH*)(buff + 1 + 1);
            else if ((*buff == GIF_EAMM) && eamf)
            { /** app metadata ext. **/
                wtmp = whdr;
                wtmp.bptr = buff + 1 + 1; /** just passing the raw chunk **/
                eamf(anim, &wtmp);
            }
        }
    whdr.bptr -= GIF_BLEN; /** for excess pixel codes ----v (here & above) **/
    GIF_MGET(whdr.bptr, (unsigned long)(blen + GIF_BLEN + 2), anim, 0);
    return (whdr.nfrm < 0) ? (skip - whdr.ifrm - 1) : (whdr.ifrm + 1);
}
