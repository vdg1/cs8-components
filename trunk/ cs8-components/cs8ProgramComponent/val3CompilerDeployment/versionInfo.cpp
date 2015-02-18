/*
  Copyright (C) 2008 Laurent Cozic. All right reserved.
  Use of this source code is governed by a GNU/GPL license that can be
  found in the LICENSE file.
*/


#include "windows.h"
#include "versionInfo.h"
#include "tchar.h"
#include "Strsafe.h"

#include <QTextStream>

bool getProductNamme(const QString & fileName, QString & productName, QString & fileVersion)
{
    DWORD DWD;
    // Read size of info block.
    DWORD VerInfoSize = GetFileVersionInfoSize(fileName.utf16(),&DWD);
    if (VerInfoSize)
        {
            char *VerInfo = new char[VerInfoSize];
            // Read info block.
            if (!GetFileVersionInfo(fileName.utf16(),0,VerInfoSize,VerInfo))
                return false;

            struct LANGANDCODEPAGE
            {
                WORD wLanguage;
                WORD wCodePage;
            } *lpTranslate;

            UINT cbTranslate;

            // Read the list of languages and code pages.
            VerQueryValue(VerInfo,
                          TEXT("\\VarFileInfo\\Translation"),
                          (LPVOID*)&lpTranslate,
                          &cbTranslate);

            // Read the file description for each language and code page.
            QString SubBlock;
            char *LangInfo;
            LPSTR   lpVersion;

            UINT LangSize;
            for( UINT i=0; i < (cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
                {
                    SubBlock=QString("\\StringFileInfo\\%1%2\\ProductName")
                             .arg((uint)lpTranslate[i].wLanguage,4,16,QChar('0'))
                             .arg((uint)lpTranslate[i].wCodePage,4,16,QChar('0'));
                    VerQueryValue(VerInfo,SubBlock.utf16(), (LPVOID *)&lpVersion,&LangSize);
                    if (LangSize>0)
                        productName=QString::fromWCharArray((const wchar_t*)lpVersion,LangSize);
                    else
                        productName=QString("");

                    SubBlock=QString("\\StringFileInfo\\%1%2\\FileVersion")
                             .arg((uint)lpTranslate[i].wLanguage,4,16,QChar('0'))
                             .arg((uint)lpTranslate[i].wCodePage,4,16,QChar('0'));
                    VerQueryValue(VerInfo,SubBlock.utf16(), (LPVOID *)&lpVersion,&LangSize);
                    if (LangSize>0)
                        fileVersion=QString::fromWCharArray((const wchar_t*)lpVersion,LangSize);
                    else
                        fileVersion=QString("");

                    /*
                    SubBlock=QString("\\StringFileInfo\\%1%2\\FileVersion")
                            .arg((uint)lpTranslate[i].wLanguage,4,16,QChar('0'))
                            .arg((uint)lpTranslate[i].wCodePage,4,16,QChar('0'));
                    VerQueryValue(VerInfo,SubBlock.utf16(),(LPVOID *)&lpVersion,&LangSize);
                    SubBlock=QString("\\StringFileInfo\\%1%2\\Comments")
                            .arg((uint)lpTranslate[i].wLanguage,4,16,QChar('0'))
                            .arg((uint)lpTranslate[i].wCodePage,4,16,QChar('0'));
                    VerQueryValue(VerInfo,SubBlock.utf16(),(LPVOID *)&lpVersion,&LangSize);
                    */
                }
            delete[] VerInfo;
        }
    return true;
}
