#include "clCliFile.h"

clCliFile::clCliFile() : m_error("clCliFile")
{
	m_IndexTable = NULL;
	m_IndexTable_count = 0;
}

clCliFile::~clCliFile()
{
	m_file.closeFile();

	reset();
}

void clCliFile::reset()
{
	if (m_IndexTable != NULL) delete[] m_IndexTable;
	m_IndexTable = NULL;

}

bool clCliFile::readFromFile(const char *filename)
{
	//- alle werte zurücksetzen
	tyFileHeader m_FileHead;

	if (readFileHeader())
	{
		if (m_FileHead.isBinary)
		{
			indexFileBinary();
		}
		else
		{
			m_error.AddError("CLI-Ascii Format not supported");
			//readFileAscii();
		}

	}
}

void setOffset(int offset)
{
	if (offset > -1)
	{

		m_offset = offset;
	}
}
bool clCliFile::indexFileBinary()
{
	/*private bool indexFileBinary()
	{
	*/	setOffset(m_FileHead.dataOffset);


		m_IndexTable_count = m_FileHead.layers;
		m_IndexTable = new tyIndexTable[m_IndexTable_count];

		int currentLayer = -1;

		while ((currentLayer < (m_IndexTable_count - 1)) && (!m_eof))
		{
			int cmd = readIntBE(2);
			int pointCount = 0;

			switch (cmd)
			{
			case 127: //- Start Layer long 
				currentLayer++;
				m_IndexTable[currentLayer].layerPos = readRealBE() * m_FileHead.unit;
				m_IndexTable[currentLayer].FileOffset = m_offset;
				m_IndexTable[currentLayer].polyCount = 0;
				break;

			case 128: //- Start Layer short 
				currentLayer++;
				m_IndexTable[currentLayer].layerPos = readIntBE(2) * m_FileHead.unit;
				m_IndexTable[currentLayer].FileOffset = m_offset;
				m_IndexTable[currentLayer].polyCount = 0;
				break;

			case 129: //- Start PolyLine short
				m_IndexTable[currentLayer].polyCount++;

				readIntBE(2); //- id 
				readIntBE(2); //- dir

				pointCount = readIntBE(2); //- number of points 

				setOffset(m_offset + pointCount * 2 * 2); //- Daten überspringen (2 - für int ; 2 x+y)
				break;

			case 130: //- Start PolyLine long 
				m_IndexTable[currentLayer].polyCount++;

				readIntBE(4); //- id 
				readIntBE(4); //- dir
				pointCount = readIntBE(4); //- number of points 

				setOffset(m_offset + pointCount * 4 * 2); //- Daten überspringen
				break;

			default:
				m_error.AddError("indexFileBinary() : data out of sync. Unknown command: " + cmd + " at pos: " + m_offset);
				m_IndexTable_count = currentLayer + 1;
				return false;

			}

		}
		return true;
	}

bool clCliFile :: readFileHeader()
{
	/*
	$$HEADERSTART
	$$BINARY
	$$UNITS/00000000.010000
	$$VERSION/200
	$$LABEL/1,part1
	$$DATE/040513
	$$DIMENSION/00000000.000000,00000000.000000,00000000.000000,00000010.000000,00000012.000000,00000012.980000
	$$LAYERS/000650
	$$HEADEREND
	*/
	if (!m_file.openFile(filename))
	{
		m_error.AddError("readFromFile(): Unable to open file: %s", filename);
		return false;
	}
	char *buffer = new char[];
	m_file.readString(buffer, HEADER_BUFFER_LEN + 5,0);
}

