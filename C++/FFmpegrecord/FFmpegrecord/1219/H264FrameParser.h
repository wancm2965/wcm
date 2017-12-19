#ifndef _H264_FRAME_PARSER_H_
#define _H264_FRAME_PARSER_H_

const char* AVCFindStartCodeInternal(const char *p, const char *end);

const char* AVCFindStartCode(const char *p, const char *end);

void AVCParseNalUnits(const char *bufIn, int inSize, char* bufOut, int* outSize);

void ParseH264Frame(const char* nalsbuf, int size, char* outBuf, int& outLen,
    char* spsBuf, int& spsSize, char* ppsBuf, int& ppsSize,
    bool& isKeyframe, int& width, int& height);

bool h264_decode_seq_parameter_set(BYTE * buf, unsigned int nLen, int &width, int &height, int &fps);

bool GetWidthHeightFromFrame(const char* frameBuf, int bufLen, int& width, int& height);

#endif // _H264_FRAME_PARSER_H_
