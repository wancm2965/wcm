/*p64fullencoder.h copyright (c)Indranet Technologies ltd (lara@indranet.co.nz)
 *                        Author Derek J Smithies (derek@indranet.co.nz)
 *
 *
 * This file defines the p64fullencoder class, which is the combined total of
 * the grabber, pre encoder, and encoder classes.
 */



#ifndef lib_p64fullencoder_h
#define lib_p64fullencoder_h

#include "config.h"
#include "p64.h"
#include "vid_coder.h"
#include "videoframe.h"

#include "transmitter.h"
#include "h261fullencoder.h"
#define WIDTH 352
#define HEIGHT 288



class P64FullEncoder{
  public:
	  void SkipCurrentFrame();
  /** Constructor for using a grabber. The grabber may elect to read data from a 
      hardware device, or from data in memory, Data in memory is placed there by 
      programs such as openmcu.
	*/
  P64FullEncoder(int quant_level,int idle);

  ~P64FullEncoder();
      
  /** Set the quantization level (or quality level) in the h261 encoder class.
      Has a value of 1 (good quality) to 31 (poor quality).
      Quality is improved at the expense of bit rate.
  */
  void SetQualityLevel(int qLevel);

  /** Set the number of blocks in the background that need
      to be transmitted with each frame
  */
  void SetBackgroundFill(int idle);

  /** Set size of the grabbing window, which sets the
      size of the vid_frame class. When the vid_frame class
      is passed to grabber and encoder, the grabber and
      encoder automatically resize
  */
  void SetSize(int width,int height);

  /** Called by the display routine, so we can display
      local video using the grabbed 411 format stuff.  
      Returns the address of the grabbed 411 format frame.
  */
  u_char* GetFramePtr();

  //  /** Grabs one video frame,
  //      or generates test pattern if grabber is not operational
  //  */
  //  void GrabOneFrame();


  /** On a previously grabbed frame,
      1)do some motion and block changed checking,
      2)h261 format conversion and
      3)store result in a series of packets in a list 
        in the transmitter class.
  */
	int ProcessOneFrame();//modify void-->int by lixq 2004-5-9

  /** Retrieves the first packet in the list recorded in
      in the transmitter class
  */
  void ReadOnePacket(
    u_char * buffer,  // Buffer of encoded data
    unsigned & length // Actual length of encoded data buffer
  );

  /** Pre-process one video frame for later incremental encoding
      same as ProcessOneFrame() except that no encoding is done
  */
  void PreProcessOneFrame();

  /** Incrementally encodes and retrieves one packet 
      Must call PreProcesOneFrame() first, then call IncEncodeAndGetPacket()
      the same way as ReadOnePacket() is called
  */
  void IncEncodeAndGetPacket(
    u_char * buffer,  // Buffer of encoded data
    unsigned & length // Actual length of encoded data buffer
  );
 
	/** Return true if there are packets waiting to be transmitted. These
      packets were created at the last invocation of the video codec.
	*/ 
  int PacketsOutStanding()
    { return  trans->PacketsOutStanding(); }

	/** When incrementally encoding, return true if there is still 
      more encoding waiting to be done for the current video frame.
	*/ 
  int MoreToIncEncode()
    { return  h261_edr->MoreToIncEncode(); }

	/** Return the count of packets waiting to be transmitted. These
      packets were created at the last invocation of the video codec.
	*/ 
  int GetCountPacketsOutStanding()
	  { return trans->GetCountPacketsOutStanding();}

  void FastUpdatePicture();

protected:
  //variables used in grabbing/processing the image. 
  Transmitter      *trans;
  H261FullEncoder  *h261_edr;
  VideoFrame       *vid_frame;
//  Pre_Vid_Coder    *pre_vid;


};


#endif   //#ifndef lib_p64encoder_h
