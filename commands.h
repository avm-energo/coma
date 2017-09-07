#ifndef COMMANDS_H
#define COMMANDS_H

#ifdef COMPORTENABLE
int CN_GetBsi(void *ptr, quint32 size);
int CN_GetFile(void *ptr, quint32 filenum);
int CN_PutFile(void *ptr, quint32 filenum, quint32 size);
#else
#ifdef USBENABLE
int UH_GetBsi(void *ptr, quint32 size);
int UH_GetFile(void *ptr, quint32 filenum);
int UH_PutFile(void *ptr, quint32 filenum, quint32 size);
#endif
#endif

#endif // COMMANDS_H
