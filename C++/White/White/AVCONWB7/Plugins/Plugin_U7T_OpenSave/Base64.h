#ifndef __TRUE__BASE64__EMAIL__
#define __TRUE__BASE64__EMAIL__

#include <windows.h>
#include <string>
#include <fstream>
#include <strstream>

const static char BASE64_ALPHABET [64] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',   //   0 -   9
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',   // 10 - 19
	'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',   // 20 - 29
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',   // 30 - 39
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',   // 40 - 49
	'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',   // 50 - 59
	'8', '9', '+', '/'          // 60 - 63
};

const static char BASE64_DEALPHABET [128] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //   0 -   9
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 10 - 19
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 20 - 29
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 30 - 39
	0, 0, 0, 62, 0, 0, 0, 63, 52, 53,   // 40 - 49
	54, 55, 56, 57, 58, 59, 60, 61, 0, 0,   // 50 - 59
	0, 61, 0, 0, 0, 0, 1, 2, 3, 4,   // 60 - 69
	5, 6, 7, 8, 9, 10, 11, 12, 13, 14,   // 70 - 79
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24,   // 80 - 89
	25, 0, 0, 0, 0, 0, 0, 26, 27, 28,   // 90 - 99
	29, 30, 31, 32, 33, 34, 35, 36, 37, 38,   // 100 - 109
	39, 40, 41, 42, 43, 44, 45, 46, 47, 48,   // 110 - 119
	49, 50, 51, 0, 0, 0, 0, 0    // 120 - 127
};

#define  MAX_LINE_LENGTH 76
#define  MAX_BUF  MAX_LINE_LENGTH + 3
#define  DATA_BUF_SIZE  19*3

class CBase64
{
public:
	CBase64 (){;}

protected:
	UINT CalculateRecquiredEncodeOutputBufferSize (UINT p_InputByteCount)
	{
		div_t result = div (p_InputByteCount, 3);

		UINT RecquiredBytes = 0;
		if (result.rem == 0)
		{
			// Number of encoded characters
			RecquiredBytes = result.quot * 4;

			// CRLF -> "\r\n" each 76 characters
			result = div (RecquiredBytes, 76);
			RecquiredBytes += result.quot * 2;

			// Terminating null for the Encoded String
			RecquiredBytes += 1;

			return RecquiredBytes;
		}
		else
		{
			// Number of encoded characters
			RecquiredBytes = result.quot * 4 + 4;

			// CRLF -> "\r\n" each 76 characters
			result = div (RecquiredBytes, 76);
			RecquiredBytes += result.quot * 2;

			// Terminating null for the Encoded String
			RecquiredBytes += 1;

			return RecquiredBytes;
		}
	}

	UINT CalculateRecquiredDecodeOutputBufferSize (char* p_pInputBufferString)
	{
		UINT BufferLength = strlen (p_pInputBufferString);

		div_t result = div (BufferLength, 4);

		if (p_pInputBufferString [BufferLength - 1] != '=')
		{
			return result.quot * 3;
		}
		else
		{
			if (p_pInputBufferString [BufferLength - 2] == '=')
			{
				return result.quot * 3 - 2;
			}
			else
			{
				return result.quot * 3 - 1;
			}
		}
	}

	void EncodeByteTriple (char* p_pInputBuffer, UINT InputCharacters, char* p_pOutputBuffer)
	{
		if (InputCharacters <= 0)
		{
			return;
		}

		UINT mask = 0xfc000000;
		UINT buffer = 0;


		char* temp = (char*) & buffer;
		temp [3] = p_pInputBuffer [0];
		if (InputCharacters > 1)
			temp [2] = p_pInputBuffer [1];
		if (InputCharacters > 2)
			temp [1] = p_pInputBuffer [2];

		switch (InputCharacters)
		{
		case 3:
			{
				p_pOutputBuffer [0] = BASE64_ALPHABET [(buffer & mask) >> 26];
				buffer = buffer << 6;
				p_pOutputBuffer [1] = BASE64_ALPHABET [(buffer & mask) >> 26];
				buffer = buffer << 6;
				p_pOutputBuffer [2] = BASE64_ALPHABET [(buffer & mask) >> 26];
				buffer = buffer << 6;
				p_pOutputBuffer [3] = BASE64_ALPHABET [(buffer & mask) >> 26];
				break;
			}
		case 2:
			{
				p_pOutputBuffer [0] = BASE64_ALPHABET [(buffer & mask) >> 26];
				buffer = buffer << 6;
				p_pOutputBuffer [1] = BASE64_ALPHABET [(buffer & mask) >> 26];
				buffer = buffer << 6;
				p_pOutputBuffer [2] = BASE64_ALPHABET [(buffer & mask) >> 26];
				p_pOutputBuffer [3] = '=';
				break;
			}
		case 1:
			{
				p_pOutputBuffer [0] = BASE64_ALPHABET [(buffer & mask) >> 26];
				buffer = buffer << 6;
				p_pOutputBuffer [1] = BASE64_ALPHABET [(buffer & mask) >> 26];
				p_pOutputBuffer [2] = '=';
				p_pOutputBuffer [3] = '=';
				break;
			}
		}
	}

	UINT DecodeByteQuartet (char* p_pInputBuffer, char* p_pOutputBuffer)
	{
		UINT buffer = 0;

		if (p_pInputBuffer[3] == '=')
		{
			if (p_pInputBuffer[2] == '=')
			{
				buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[0]]) << 6;
				buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[1]]) << 6;
				buffer = buffer << 14;

				char* temp = (char*) & buffer;
				p_pOutputBuffer [0] = temp [3];

				return 1;
			}
			else
			{
				buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[0]]) << 6;
				buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[1]]) << 6;
				buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[2]]) << 6;
				buffer = buffer << 8;

				char* temp = (char*) & buffer;
				p_pOutputBuffer [0] = temp [3];
				p_pOutputBuffer [1] = temp [2];

				return 2;
			}
		}
		else
		{
			buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[0]]) << 6;
			buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[1]]) << 6;
			buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[2]]) << 6;
			buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[3]]) << 6;
			buffer = buffer << 2;

			char* temp = (char*) & buffer;
			p_pOutputBuffer [0] = temp [3];
			p_pOutputBuffer [1] = temp [2];
			p_pOutputBuffer [2] = temp [1];

			return 3;
		}

		return -1;
	}

public:
	void EncodeBuffer (char* p_pInputBuffer, UINT p_InputBufferLength, char* p_pOutputBufferString)
	{
		if (p_InputBufferLength <= 0)
		{
			return;
		}

		UINT FinishedByteQuartetsPerLine = 0;
		UINT InputBufferIndex = 0;
		UINT OutputBufferIndex = 0;

		memset (p_pOutputBufferString, 0, CalculateRecquiredEncodeOutputBufferSize (p_InputBufferLength));

		while (InputBufferIndex < p_InputBufferLength)
		{
			if (p_InputBufferLength - InputBufferIndex <= 2)
			{
				FinishedByteQuartetsPerLine ++;
				EncodeByteTriple (p_pInputBuffer + InputBufferIndex, p_InputBufferLength - InputBufferIndex, p_pOutputBufferString + OutputBufferIndex);
				break;
			}
			else
			{
				FinishedByteQuartetsPerLine++;
				EncodeByteTriple (p_pInputBuffer + InputBufferIndex, 3, p_pOutputBufferString + OutputBufferIndex);
				InputBufferIndex += 3;
				OutputBufferIndex += 4;
			}

			if (FinishedByteQuartetsPerLine == 19)
			{
				p_pOutputBufferString [OutputBufferIndex ] = '\r';
				p_pOutputBufferString [OutputBufferIndex + 1] = '\n';
				p_pOutputBufferString += 2;
				FinishedByteQuartetsPerLine = 0;
			}
		}
	}

	UINT DecodeBuffer (char* p_pInputBufferString, char* p_pOutputBuffer)
	{
		UINT InputBufferIndex = 0;
		UINT OutputBufferIndex = 0;
		UINT InputBufferLength = strlen (p_pInputBufferString);

		char ByteQuartet [4];

		while (InputBufferIndex < InputBufferLength)
		{
			for (int i = 0; i < 4; i++)
			{
				ByteQuartet [i] = p_pInputBufferString [InputBufferIndex];

				// Ignore all characters except the ones in BASE64_ALPHABET
				if ((ByteQuartet [i] >= 48 && ByteQuartet [i] <= 57) ||
					(ByteQuartet [i] >= 65 && ByteQuartet [i] <= 90) ||
					(ByteQuartet [i] >= 97 && ByteQuartet [i] <= 122) ||
					ByteQuartet [i] == '+' || ByteQuartet [i] == '/' || ByteQuartet [i] == '=')
				{}
				else
				{
					// Invalid character
					i--;
				}

				InputBufferIndex++;
			}

			OutputBufferIndex += DecodeByteQuartet (ByteQuartet, p_pOutputBuffer + OutputBufferIndex);
		}

		// OutputBufferIndex gives us the next position of the next decoded character
		// inside our output buffer and thus represents the number of decoded characters
		// in our buffer.
		return OutputBufferIndex;
	}

	BOOL EncodeStream (LPCSTR inStream, int inSize, std::string & outFileData)
	{
		if (inSize <= 0)
		{
			return FALSE;
		}

		outFileData.resize(0);

		int nNeedSize = CalculateRecquiredEncodeOutputBufferSize (inSize);
		if (nNeedSize <= 0)
			return FALSE;
		char* pOutputBuffer = new char[nNeedSize];
		memset(pOutputBuffer, 0, nNeedSize);

		EncodeBuffer ((char*)inStream, inSize, pOutputBuffer);
		outFileData.assign(pOutputBuffer);

		delete []pOutputBuffer;

		return outFileData.size() > 0;
	}

	BOOL DecodeStream (LPCSTR inStream, int inSize, std::string & outFileData)
	{
		if (inSize <= 0)
		{
			return FALSE;
		}
		outFileData.resize(0);
		std::istrstream stm( inStream, inSize );

		char * inFileData = new char[inSize];
		char * lpBuffer = new char[inSize];

		char line[MAX_BUF];
		int pos = 0;
		int nLineLen = 0;
		while ( stm.getline(line, MAX_BUF) )
		{
			memcpy( inFileData + pos, line, nLineLen = strlen(line) );
			pos += nLineLen;
		}
		inFileData[pos] = '\0';

		UINT DecodedBytes = DecodeBuffer ( inFileData, lpBuffer);
		outFileData.assign( (LPCSTR)lpBuffer, DecodedBytes);

		delete []lpBuffer;
		delete []inFileData;

		return outFileData.size() > 0;
	}

	BOOL EncodeFile (char* p_pSourceFileName, char* p_pEncodedFileName)
	{
		std::ifstream InputFile;
		InputFile.open ( p_pSourceFileName, std::ios::binary | std::ios::in ); //二进制
		if (!InputFile.is_open())
			return FALSE;

		std::ofstream OutputFile;
		OutputFile.open ( p_pEncodedFileName , std::ios::binary | std::ios::out ); //二进制
		if (!OutputFile.is_open())
			return FALSE;

		char InputBuffer [DATA_BUF_SIZE];
		int nNeedSize = CalculateRecquiredEncodeOutputBufferSize( DATA_BUF_SIZE ) + 1;
		if (nNeedSize == 0)
			return FALSE;
		char* pOutputBuffer = new char[nNeedSize];
		memset(pOutputBuffer, 0, nNeedSize);

		InputFile.seekg(0, std::ios::end);
		int nInFileSize = InputFile.tellg();
		InputFile.seekg(0, std::ios::beg);

		int nHasRead = 0;
		do
		{
			int ReadBytes = DATA_BUF_SIZE;
			if ( nHasRead + DATA_BUF_SIZE > nInFileSize )
			{
				ReadBytes = nInFileSize - nHasRead;
			}
			InputFile.read (InputBuffer, ReadBytes );
			nHasRead += ReadBytes;

			EncodeBuffer (InputBuffer, ReadBytes, pOutputBuffer);
			OutputFile.write (pOutputBuffer, strlen (pOutputBuffer));
		}
		while ( !(nHasRead >= nInFileSize) );

		delete []pOutputBuffer;

		OutputFile.close ();
		InputFile.close ();

		return TRUE;
	}

	BOOL DecodeFile (char* p_pSourceFileName, char* p_pDecodedFileName)
	{
		std::ifstream InputFile;
		std::ofstream OutputFile;
		InputFile.open ( p_pSourceFileName , std::ios::in ); //文本
		if ( ! InputFile.is_open() )
			return FALSE;
		OutputFile.open ( p_pDecodedFileName, std::ios::binary | std::ios::out ); //二进制
		if ( !OutputFile.is_open() )
			return FALSE;

		char InBuf[MAX_BUF];
		char OutputBuffer[DATA_BUF_SIZE];
		while ( InputFile.getline (InBuf, MAX_BUF) )
		{
			int nInBufLen = strlen(InBuf);
			std::string InputBuffer( (LPCSTR)InBuf, nInBufLen );
			int posR = InputBuffer.find('\r');
			if (posR != -1)
			{
				InputBuffer.erase(posR, 1);
			}
			int posN = InputBuffer.find('\n');
			if (posN != -1)
			{
				InputBuffer.erase(posN, 1);
			}
			UINT DecodedBytes = DecodeBuffer ( (char *)InputBuffer.c_str(), (char *)OutputBuffer );
			OutputFile.write ( (LPCSTR)OutputBuffer, DecodedBytes );
		}

		OutputFile.close();
		InputFile.close();
		return 0;
	}

};

#endif