/**
  @file GtFile.h
  
  @author Kaiming

  @brief �ļ���

  ������־ history
  ver:1.0
   
 */

#ifndef GtFile_h__
#define GtFile_h__

class GtFile
{
public:
	GtFile(const std::string& name);
	~GtFile(void);

	void	Open();
	bool	IsOpen() {return m_open;}
	void	Close();
	char*	Data() const;
	uint32  Size() const {return m_size;}

private:
	bool m_open;
	std::string m_name;
	char* m_data;
	uint32 m_size;
};

inline GtFile::GtFile( const std::string& name ):
m_name(name), 
	m_open(false), 
	m_data(NULL)
{

}

inline GtFile::~GtFile(void)
{
	Close();
}

inline void GtFile::Open()
{
	std::string realpath = getMediaPath() + m_name;

	FILE* pFile = NULL;
	fopen_s( &pFile, realpath.c_str(), "rb");

	if (pFile)
	{
		// ��ȡ�ļ�����
		fseek(pFile, 0, SEEK_END);
		m_size = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		// �ļ���СΪ0
		if ( m_size == -1)
		{
			fclose(pFile);
			return;
		}

		// �����ڴ��е��ļ�
		char* pTmp = new char[m_size + 1];
		if (!pTmp)
		{
			m_size = 0;
			fclose(pFile);
			return;
		}

		// ���ļ��п������ݵ��ڴ�
		size_t BytesRead = fread(pTmp, 1, m_size, pFile);

		// �����Ƿ�ɹ�
		if (BytesRead != m_size)
		{
			delete [] pTmp;
			m_size = 0;
		}
		else
		{
			m_data = pTmp;
			m_data[m_size] = '\0';
			m_open = true;
		}

		fclose(pFile);				
	}
	else
	{
		// cannot open
	}
}

inline void GtFile::Close()
{
	if (m_open)
	{
		m_open = false;
		delete[] m_data;
	}
}

inline char* GtFile::Data() const
{
	return m_data;
}

#endif



