#ifndef COMMANDS_H
#define COMMANDS_H

/*int CN_GetBsi(void *ptr, quint32 size);
int CN_GetFile(void *ptr, quint32 filenum);
int CN_PutFile(void *ptr, quint32 filenum, quint32 size); */
int UH_GetBsi(void *ptr, quint32 size);
int UH_GetFile(void *ptr, quint32 filenum);
int UH_PutFile(void *ptr, quint32 filenum, quint32 size);

#endif // COMMANDS_H
