#pragma once
#include "clError.h"
#include "clFile.h"
#include "clSliceData.h"
#include "abstractSliceFile.h"
class clCliFile : public abstractSliceFile
{
public:
	clCliFile();
	void reset();
	~clCliFile();

	bool readFromFile(const char *filename);
	virtual bool readSliceData(clSliceData *sliceData, int PartIndex, int LayerIndex, int storeAsPartIndex = -1);
	int getLayerCount(int PartIndex);
	float getLayerPos(int PartIndex, int layerIndex);
	int getPartCount();
	char *getPartName(int PartIndex);
	char *getPartProperty(int PartIndex);

	int getLayerIndexByPos(int PartIndex, float LayerPos);
	float getMaxLayerPos(int PartIndex);

private:
	struct tyFileHeader
	{
		bool isBinary;
		float unit;
		int version;
		int fileDate;
		double d_x1;
		double d_y1;
		double d_z1;
		double d_x2;
		double d_y2;
		double d_z2;
		int layers;
		bool align;
		int dataOffset;
	};

	struct tyIndexTable
	{
		int FileOffset;   // offset in File
		float layerPos;   // 
		int polyCount;    // count of Polylines
	};

	clError m_error;
	clFile m_file;

	int m_IndexTable_count;
	tyIndexTable *m_IndexTable;

	tyFileHeader m_FileHead;
	bool readFileHeader();
	bool indexFileBinary();


};