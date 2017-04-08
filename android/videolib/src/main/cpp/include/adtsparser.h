#ifndef ADTSPARSER_H
#define ADTSPARSER_H

 struct Adts_Header
{
    unsigned  long  syncword;
    unsigned  long  id;
    unsigned  long  layer;
    unsigned  long  protection_absent;
    unsigned  long  profile;
    unsigned  long  sf_index;
    unsigned  long  private_bit;
    unsigned  long  channel_configuration;
    unsigned  long  original;
    unsigned  long  home;
    unsigned  long  emphasis;
    unsigned  long  copyright_identification_bit;
    unsigned  long  copyright_identification_start;
    unsigned  long  aac_frame_length;
    unsigned  long  adts_buffer_fullness;
    unsigned  long  no_raw_data_blocks_in_frame;
    unsigned  long  crc_check;

    /* control param */
     unsigned  long   old_format;
};

 struct faacDecHandle
{
     unsigned  long   adts_header_present;
     unsigned  long   sf_index;
     unsigned  long   object_type;
     unsigned  long   channelConfiguration;
     unsigned  long   frameLength;
};
bool isValideAdtsFrame(unsigned char *pBuf,int nBufSize);
int    GetSampleRate(unsigned char *pBuf,int nBufSize);//得到采样率
int    GetChannels(unsigned char *pBuf,int nBufSize);//得到声道数
bool GetAdtsInfo(unsigned char *pBuf,int nBufSize,Adts_Header* pInfo);
extern int adts_sample_rates[] ;
#endif // ADTSPARSER_H
