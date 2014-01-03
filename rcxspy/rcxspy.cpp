#include <cstdio>
#include "RCX_Disasm.h"
#include "RCX_Pipe.h"
#include "RCX_PipeTransport.h"
#include "RCX_Link.h"

class Reader
{
public:
			Reader();
			~Reader();

	bool 	open(const char *name);
	void	close();

	void	poll();

	void	beginDownload();
	void	continueDownload();

private:
	static const int MAX_PACKET = 1024;

	RCX_PipeTransport*	transport;

	UByte		packet[MAX_PACKET];
	int			packetLength;

	UByte*		code;
	ULong		codeLength;
	ULong		codePos;
	ULong		lastBlock;
};


Reader::Reader()
{
}

Reader::~Reader()
{
	close();
}


bool Reader::open(const char *name)
{
	RCX_Pipe *pipe = RCX_NewUSBTowerPipe();
	transport = new RCX_PipeTransport(pipe);

	if (transport->Open(kRCX_RCX2Target, name, RCX_Link::kVerboseMode) != kRCX_OK) {
		delete transport;
		transport = 0;
		return false;
	}

	return true;
}



void Reader::close()
{
	if (!transport) return;

	transport->Close();
	delete transport;
	transport = 0;
}



Reader reader;

int main(int, char **)
{
	if (!reader.open("")) {
		printf("Error opening USB IR tower\n");
		return -1;
	}

	while(1) {
		reader.poll();
	}

	reader.close();

	return 0;
}


void Reader::poll()
{
	RCX_Result r = transport->Receive(packet, MAX_PACKET, false);
	if (r <= 0) return;

	UByte cmd = packet[0];
	UByte data[9];
	UByte *ptr = data;

	*ptr++ = ~cmd;

	cmd &= 0xf7;

	switch(cmd)
	{
		case 0x15:	// firmware version
			*ptr++ = 0;
			*ptr++ = 3;
			*ptr++ = 0;
			*ptr++ = 1;
			*ptr++ = 0;
			*ptr++ = 3;
			*ptr++ = 3;
			*ptr++ = 0;
			break;
		case 0x25:	// begin download
			beginDownload();
			*ptr++ = 0;
			break;
		case 0x45:	// continue download
			continueDownload();
			*ptr++ = 0;
			break;
		case 0x30:	// battery level
			*ptr++ = 35;
			*ptr++ = 40;
			break;
		default:
			break;
	}

//	link.Send1(data, ptr-data);
//	printf("%02x\n", cmd);
}


void Reader::beginDownload()
{
	UByte number = packet[2];
	codeLength = packet[4] + ((ULong)packet[5] << 8);

	delete [] code;

	code = new UByte[codeLength];
	codePos = 0;
	lastBlock = -1;
}


void Reader::continueDownload()
{
	ULong block = packet[1] + ((ULong)packet[2] << 8);
	ULong length = packet[3] + ((ULong)packet[4] << 8);

	if (block == lastBlock) return;

	lastBlock = block;

	for(int i=0; i<length && codePos < codeLength; ++i)
		code[codePos++] = packet[5 + i];

	if (block == 0)
	{
		RCX_StdioPrinter dst(stdout);
		gRCX_Disasm.Print(&dst, code, codeLength);
	}
}

