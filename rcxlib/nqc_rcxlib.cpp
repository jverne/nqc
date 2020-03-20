/* C/C++ ABI wrapper for NQC's rcxlib */

#include <stdlib.h>

#include "RCX_Pipe.h";

#ifdef USE_TRANSPORT
#include "RCX_Link.h"
#include "RCX_PipeTransport.h"
static RCX_PipeTransport *rcx_pipe_transport = 0;

#define BUFFERSIZE 4096
extern int __comm_debug;
#endif

#include "nqc_rcxlib.h"

void *rcx_pipe_init()
{
  RCX_Pipe *pipe = RCX_NewUSBTowerPipe();
#ifdef USE_TRANSPORT
  rcx_pipe_transport = new RCX_PipeTransport(pipe);
  if (rcx_pipe_transport->Open((RCX_TargetType) 0, "short", (__comm_debug ? RCX_Link::kVerboseMode : 0)) != kRCX_OK) {
    delete rcx_pipe_transport;
    rcx_pipe_transport = 0;
    exit(1);
  }
#else
  pipe->Open("short", RCX_Pipe::kNormalIrMode);
  // pipe->SetMode(RCX_Pipe::kFastIrMode);
#endif
  return (void *) pipe;
}

void rcx_pipe_close(void *pipe)
{
  return ((RCX_Pipe *) pipe)->Close();
}

long rcx_pipe_read(void *pipe, void *ptr, long count, long timeout_ms)
{
  return ((RCX_Pipe *) pipe)->Read(ptr, count, timeout_ms);
}

long rcx_pipe_write(void *pipe, const void *ptr, long count)
{
  return ((RCX_Pipe *) pipe)->Write(ptr, count);
}

#ifdef USE_TRANSPORT
long rcx_pipe_send(void *pipe, void *send, int slen, void *recv, int rlen,
		  int timeout, int retries, int use_comp)
{
  long result = rcx_pipe_transport->Send((const UByte*) send, slen, (UByte*) recv, rlen, BUFFERSIZE, 1);
  if (!RCX_ERROR(result))
    return rlen;
  else
    return result;
}
#endif
