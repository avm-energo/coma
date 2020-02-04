#include <QDateTime>

#include "error.h"
#include "s2.h"

S2::S2()
{

}

int S2::StoreDataMem(void *mem, QVector<DataRec> *dr, int fname) //0 - успешно, иначе код ошибки
{
    quint32 crc=0xFFFFFFFF;
    FileHeader D;
    DataRec R;
    quint32 i;
    char *m=static_cast<char *>(mem);
    m+=sizeof(FileHeader);
    D.size=0;
    for(QVector<DataRec>::iterator it=dr->begin(); it!=dr->end(); ++it)
    {
        R = *it;
        void *Rptr = static_cast<void *>(it);
        quint32 tmpi = sizeof(DataRec)-sizeof(void*);
        memcpy(m,&R,tmpi);
        D.size += tmpi;
        for(i=0;i<tmpi;i++)
            updCRC32((static_cast<char *>(Rptr))[i],&crc);
        if(R.id==0xFFFFFFFF)
            break;
        m+=tmpi;
        if(R.thedata)
        {
            tmpi = R.num_byte;
            for(i=0;i<tmpi;i++)
                updCRC32((static_cast<char *>(R.thedata))[i],&crc);
            D.size += tmpi;
            memcpy(m,R.thedata,tmpi);
            m+=tmpi;
        }
    }
    D.crc32=crc;
    D.thetime=getTime32();
    D.service=0xFFFF;
    D.fname=static_cast<quint16>(fname);
    memcpy(mem,&D,sizeof(D));
    return Error::ER_NOERROR;
}

int S2::RestoreDataMem(void *mem, int memsize, QVector<DataRec> *dr)
{
  quint32 crc;
  int sz=0;
  char *m=static_cast<char *>(mem);
  DataRec R;
  DataRec *r;
  FileHeader dh;
  int i;
  bool NoIDs=true; // признак того, что не встретился ни один из ID в dr

  crc=0xFFFFFFFF;

  int tmpi = sizeof(FileHeader);
  int pos = tmpi;
  if (pos > memsize)
  {
      ERMSG("S2: выход за границу принятых байт"); // выход за границу принятых байт
      return S2_SIZEERROR;
  }
  size_t tmpt = static_cast<size_t>(tmpi);
  memcpy(&dh,m,tmpt);
  m+=tmpi;
  for (i=0; i<(memsize-tmpi); i++)
      updCRC32(m[i], &crc);
  if (dh.crc32!=crc)
  {
      ERMSG("S2: Ошибка CRC"); // выход за границу принятых байт
      return S2_CRCERROR;
  }
  for(;;)
  {
      tmpi = sizeof(DataRec)-sizeof(void*);
      pos += tmpi;
      if (pos > memsize)
      {
          ERMSG("S2: выход за границу принятых байт"); // выход за границу принятых байт
          return S2_SIZEERROR;
      }
      size_t tmpt = static_cast<size_t>(tmpi);
      memcpy(&R,m,tmpt);
      sz+=tmpi;
      m+=tmpi;
      if(R.id==0xFFFFFFFF)
          break;
      r=FindElem(dr,R.id);
      if(r == nullptr) //элемент не найден в описании, пропускаем
      {
          tmpi = static_cast<int>(R.num_byte);
          pos += tmpi;
          if (pos > memsize)
          {
              ERMSG("S2: выход за границу принятых байт"); // выход за границу принятых байт
              return S2_SIZEERROR;
          }
          m += tmpi;
          sz += tmpi;
          continue;
      }
      NoIDs = false;
      if (r->num_byte!=R.num_byte) //несовпадения описания прочитанного элемента с ожидаемым
      {
          ERMSG("Несовпадение описаний одного и того же блока"); // несовпадение описаний одного и того же блока
          return S2_DESCERROR;
      }
      tmpi = static_cast<int>(r->num_byte);
      pos += tmpi;
      if (pos > memsize)
      {
          ERMSG("S2: выход за границу принятых байт"); // выход за границу принятых байт
          return S2_SIZEERROR;
      }
      tmpt = static_cast<size_t>(tmpi);
      memcpy(r->thedata,m,tmpt);
      sz += tmpi;
      m += tmpi;
  }
  if(dh.size!=static_cast<quint32>(sz))
  {
      ERMSG("S2: ошибка длины"); // ошибка длины
      return S2_DHSZERROR;
  }
  if (NoIDs)
  {
      ERMSG("Не найдено ни одного элемента с заданным ID"); // не найдено ни одного ИД
      return S2_NOIDS;
  }
  return Error::ER_NOERROR;
}

S2::DataRec *S2::FindElem(QVector<DataRec> *dr, quint32 id)
{
    for(QVector<DataRec>::iterator it=dr->begin(); it!=dr->end(); ++it)
    {
        DataRec R = *it;
        if(R.id==id)
            return it;
        if(R.id==static_cast<quint32>(0xFFFF))
            return nullptr;
    }
    return nullptr;
}

void inline S2::updCRC32(char byte, quint32 *dwCRC32)
{
    *dwCRC32 = (( *dwCRC32 ) >> 8 )^ _crc32_t[static_cast<quint8>( byte )^(( *dwCRC32 ) & 0x000000FF )];
}

quint32 S2::GetCRC32(char *data, quint32 len)
{
  quint32 dwCRC32 = 0xFFFFFFFF;
  for(quint32 i=0;i<len;i++)
  {
      updCRC32(*data,&dwCRC32);
      data++;
  }
  return dwCRC32;
}

quint32 S2::getTime32()
{
    QDateTime dt;
    return dt.currentDateTime().toTime_t();
}
