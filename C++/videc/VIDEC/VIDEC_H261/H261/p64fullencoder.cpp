/*P64FullEncoder.h copyright (c)Indranet Technologies ltd (lara@indranet.co.nz)
 *                       Author Derek J Smithies (derek@indranet.co.nz)
 *
 *This file contains code which is the top level of
 *      a)video grabbing
 *      b)transformation into h261 packets.
 *
 * Questions may be sent to Derek J Smithies.
 *
 ****************************************************************/



#include "P64FullEncoder.h"

P64FullEncoder::P64FullEncoder(int quant_level,int fillLevel)
{
  trans       = new Transmitter();
  h261_edr    = new H261FullEncoder(trans);
  h261_edr->setq(quant_level);

  vid_frame   = new VideoFrame(WIDTH,HEIGHT);
//  pre_vid     = new Pre_Vid_Coder();
//  pre_vid->SetBackgroundFill(fillLevel);

}



P64FullEncoder::~P64FullEncoder(){
//  delete pre_vid;
  delete vid_frame;
  delete h261_edr; 
  delete trans;

}

void P64FullEncoder::SetQualityLevel(int qLevel)
{
  h261_edr->setq(qLevel);
}

void P64FullEncoder::SetBackgroundFill(int idle)
{
//  pre_vid->SetBackgroundFill(idle);

}

void P64FullEncoder::SetSize(int width,int height) {
  vid_frame->SetSize(width,height);

}

int P64FullEncoder::ProcessOneFrame() {
	return -1;
	//暂时不实现

  // pre_vid->ProcessFrame(vid_frame);
  // return encoded length(byte) modify by lixq 2004-5-9
  return h261_edr->consume(vid_frame);
} 

void P64FullEncoder::PreProcessOneFrame() {
//  pre_vid->ProcessFrame(vid_frame);
  h261_edr->PreIncEncodeSetup(vid_frame);
} 

void P64FullEncoder::IncEncodeAndGetPacket(
  u_char * buffer,    // returns buffer of encoded data
  unsigned & length ) // returns actual length of encoded data buffer
{
  h261_edr->IncEncodeAndGetPacket(buffer, length);
}

void P64FullEncoder::ReadOnePacket(
      u_char * buffer,    /// Buffer of encoded data
      unsigned & length /// Actual length of encoded data buffer
      )
{
  length=0;
  return ;
//暂时不实现

 u_char * b_ptr;
 u_char * h_ptr;

 unsigned len_head,len_buff;

 trans->GetNextPacket(&h_ptr, &b_ptr, len_head, len_buff);
 length=len_head+len_buff;

 if(length!=0) {                          //Check to see if a packet was read.
    long int h261_hdr=*(long int *)h_ptr;   
    *(long int *)buffer= htonl(h261_hdr);
    memcpy(buffer+len_head,b_ptr,len_buff);
 }
 
}

u_char* P64FullEncoder::GetFramePtr()
{
  if (vid_frame)
    return vid_frame->frameptr;
  return NULL; 
}

void P64FullEncoder::FastUpdatePicture()
{
//  pre_vid->FastUpdatePicture();
	
	//使用I帧编码方式
	h261_edr->FastUpdatePicture();
}

void P64FullEncoder::SkipCurrentFrame()
{
	h261_edr->SkipCurrentFrame();
}
