#include "rar.hpp"

ComprDataIO::ComprDataIO()
{
  Init();
}


void ComprDataIO::Init()
{
  UnpPackedSize=0;
//  ShowProgress=true;
  TestMode=false;
  SkipUnpCRC=false;
  PackVolume=false;
  UnpVolume=false;
  NextVolumeMissing=false;
////  SrcFile=NULL;
////  DestFile=NULL;
  UnpWrSize=0;
  Command=NULL;
  Encryption=0;
  Decryption=0;
  TotalPackRead=0;
  CurPackRead=CurPackWrite=CurUnpRead=CurUnpWrite=0;
  PackFileCRC=UnpFileCRC=PackedCRC=0xffffffff;
  LastPercent=-1;
  SubHead=NULL;
  SubHeadPos=NULL;
  CurrentCommand=0;
////  ProcessedArcSize=TotalArcSize=0;
  OldFormat = false;
}

int ComprDataIO::UnpRead(byte *Addr,size_t Count)
{
  int RetCode=0,TotalRead=0;
  byte *ReadAddr;
  ReadAddr=Addr;
  while (Count > 0)
  {
////    Archive *SrcArc=(Archive *)SrcFile;

    size_t ReadSize=((int64)Count>UnpPackedSize) ? (size_t)UnpPackedSize:Count;

	memcpy(Addr, UnpackFromMemoryAddr, ReadSize);
    RetCode = (int)ReadSize;
    UnpackFromMemorySize -= ReadSize;
	UnpackFromMemoryAddr += ReadSize;

	CurUnpRead+=RetCode;
    TotalRead+=RetCode;
#ifndef NOVOLUME
    // These variable are not used in NOVOLUME mode, so it is better
    // to exclude commands below to avoid compiler warnings.
    ReadAddr+=RetCode;
    Count-=RetCode;
#endif
    UnpPackedSize-=RetCode;
////    if (UnpPackedSize == 0 && UnpVolume)
////    {
////#ifndef NOVOLUME
////      if (!MergeArchive(*SrcArc,this,true,CurrentCommand))
////#endif
////      {
////        NextVolumeMissing=true;
////        return(-1);
////      }
////    }
////    else
      break;
  }
////  Archive *SrcArc=(Archive *)SrcFile;
////  if (SrcArc!=NULL)
////    ShowUnpRead(SrcArc->CurBlockPos+CurUnpRead,UnpArcSize);
  if (RetCode!=-1)
  {
    RetCode=TotalRead;
#ifndef RAR_NOCRYPT
    if (Decryption)
#ifndef SFX_MODULE
      if (Decryption<20)
        Decrypt.Crypt(Addr,RetCode,(Decryption==15) ? NEW_CRYPT : OLD_DECODE);
      else
        if (Decryption==20)
          for (int I=0;I<RetCode;I+=16)
            Decrypt.DecryptBlock20(&Addr[I]);
        else
#endif
        {
          int CryptSize=(RetCode & 0xf)==0 ? RetCode:((RetCode & ~0xf)+16);
          Decrypt.DecryptBlock(Addr,CryptSize);
        }
#endif
  }
////  Wait();
  return(RetCode);
}


#if defined(RARDLL) && defined(_MSC_VER) && !defined(_WIN_64)
// Disable the run time stack check for unrar.dll, so we can manipulate
// with ProcessDataProc call type below. Run time check would intercept
// a wrong ESP before we restore it.
#pragma runtime_checks( "s", off )
#endif

void ComprDataIO::UnpWrite(byte *Addr,size_t Count)
{
	UnpWrAddr=Addr;
	UnpWrSize=Count;
	if (Count <= UnpackToMemorySize)
	{
	  memcpy(UnpackToMemoryAddr,Addr,Count);
	  UnpackToMemoryAddr+=Count;
	  UnpackToMemorySize-=Count;
	}
	CurUnpWrite+=Count;
	if (!SkipUnpCRC)
    if (OldFormat)
      UnpFileCRC=OldCRC((ushort)UnpFileCRC,Addr,Count);
    else
      UnpFileCRC=CRC(UnpFileCRC,Addr,Count);
////  ShowUnpWrite();
////  Wait();
}

#if defined(RARDLL) && defined(_MSC_VER) && !defined(_WIN_64)
// Restore the run time stack check for unrar.dll.
#pragma runtime_checks( "s", restore )
#endif



////void ComprDataIO::ShowUnpRead(int64 ArcPos,int64 ArcSize)
////{
////  if (ShowProgress && SrcFile!=NULL)
////  {
////    if (TotalArcSize!=0)
////    {
////      // important when processing several archives or multivolume archive
////      ArcSize=TotalArcSize;
////      ArcPos+=ProcessedArcSize;
////    }
////
////    Archive *SrcArc=(Archive *)SrcFile;
////    RAROptions *Cmd=SrcArc->GetRAROptions();
////
////    int CurPercent=ToPercent(ArcPos,ArcSize);
////    if (!Cmd->DisablePercentage && CurPercent!=LastPercent)
////    {
////      mprintf("\b\b\b\b%3d%%",CurPercent);
////      LastPercent=CurPercent;
////    }
////  }
////}

////void ComprDataIO::ShowUnpWrite()
////{
////}





////void ComprDataIO::SetFiles(File *SrcFile,File *DestFile)
////{
////  if (SrcFile!=NULL)
////    ComprDataIO::SrcFile=SrcFile;
////  if (DestFile!=NULL)
////    ComprDataIO::DestFile=DestFile;
////  LastPercent=-1;
////}


void ComprDataIO::GetUnpackedData(byte **Data,size_t *Size)
{
  *Data=UnpWrAddr;
  *Size=UnpWrSize;
}


////void ComprDataIO::SetEncryption(int Method,const wchar *Password,const byte *Salt,bool Encrypt,bool HandsOffHash)
////{
////  if (Encrypt)
////  {
////    Encryption=*Password ? Method:0;
////#ifndef RAR_NOCRYPT
////    Crypt.SetCryptKeys(Password,Salt,Encrypt,false,HandsOffHash);
////#endif
////  }
////  else
////  {
////    Decryption=*Password ? Method:0;
////#ifndef RAR_NOCRYPT
////    Decrypt.SetCryptKeys(Password,Salt,Encrypt,Method<29,HandsOffHash);
////#endif
////  }
////}

#if !defined(SFX_MODULE) && !defined(RAR_NOCRYPT)
void ComprDataIO::SetAV15Encryption()
{
  Decryption=15;
  Decrypt.SetAV15Encryption();
}
#endif


#if !defined(SFX_MODULE) && !defined(RAR_NOCRYPT)
void ComprDataIO::SetCmt13Encryption()
{
  Decryption=13;
  Decrypt.SetCmt13Encryption();
}
#endif

void ComprDataIO::SetUnpackToMemory(byte *Addr,uint Size)
{
  UnpackToMemory=true;
  UnpackToMemoryAddr=Addr;
  UnpackToMemorySize=Size;
}

void ComprDataIO::SetUnpackFromMemory(byte *Addr,uint Size)
{
  UnpackFromMemory=true;
  UnpackFromMemoryAddr=Addr;
  UnpackFromMemorySize=Size;
}
