/* This file was generated by the Hex-Rays decompiler.
   Copyright (c) 2007-2020 Hex-Rays <info@hex-rays.com>

   Detected compiler: GNU C++
*/

#include <defs.h>


//-------------------------------------------------------------------------
// Function declarations

void sub_E84();
// int __fastcall _cxa_finalize(void *);
// int __fastcall _cxa_atexit(void (__fastcall *lpfunc)(void *), void *obj, void *lpdso_handle);
// int _android_log_print(_DWORD, _DWORD, const char *, ...); weak
int __fastcall grallocUnmap(int, int);
// int close(int fd);
// void __fastcall operator delete(void *); idb
// void free(void *ptr);
// int strcmp(const char *s1, const char *s2);
// void *malloc(size_t size);
// void *memset(void *s, int c, size_t n);
// int ion_open(void); weak
int __fastcall fb_device_open(int, int, _DWORD *);
// int __fastcall ion_alloc_fd(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD); weak
// _DWORD __fastcall operator new(unsigned int); idb
int __fastcall gralloc_register_buffer(int, _DWORD *);
int __fastcall gralloc_unregister_buffer(int, _DWORD *);
int __fastcall gralloc_lock(int, int, int, int, int, int, int, _DWORD *);
int __fastcall gralloc_unlock(int, _DWORD *);
int __fastcall gralloc_lock_ycbcr(int, _DWORD *, int, int, int, int, int, int);
// int pthread_mutex_lock(pthread_mutex_t *mutex);
// int pthread_mutex_unlock(pthread_mutex_t *mutex);
// int open(const char *file, int oflag, ...);
// int ioctl(int fd, unsigned int request, ...);
// int __fastcall _errno(_DWORD); weak
// void *memcpy(void *dest, const void *src, size_t n);
int __fastcall init_fb(int);
// int munmap(void *addr, size_t len);
// char *strerror(int errnum);
// void *mmap(void *addr, size_t len, int prot, int flags, int fd, __off_t offset);
int __fastcall getIonFd(int);
// int __fastcall ion_sync_fd(_DWORD, _DWORD); weak
// int __fastcall ion_import(_DWORD, _DWORD, _DWORD); weak
// int __fastcall ion_free(_DWORD, _DWORD); weak
int sub_10C0();
int (*__fastcall sub_10E0(int (*result)(void)))(void);
int __fastcall sub_110C(void *a1);
int __fastcall sub_1154(int a1);
int __fastcall sub_1170(int a1, _DWORD *a2);
int __fastcall sub_11E8(void *a1);
int __fastcall sub_11F4(int a1, char *s1, _DWORD *a3);
int __fastcall sub_1270(int a1, int a2, int a3, int a4, int a5, _DWORD *a6, signed int *a7);
int __fastcall sub_18C0(void *a1);
int __fastcall sub_1908(_DWORD *a1);
int __fastcall sub_194C(int a1, _DWORD *a2);
int __fastcall init_fb(int a1);
int __fastcall fb_device_open(int a1, int a2, _DWORD *a3);
int __fastcall sub_1CBC(int a1);
int __fastcall getIonFd(int a1);
int __fastcall grallocMap(int a1, int a2);
int __fastcall grallocUnmap(int a1, int a2);
int __fastcall gralloc_register_buffer(int a1, _DWORD *a2);
int __fastcall gralloc_unregister_buffer(int a1, _DWORD *a2);
int __fastcall gralloc_lock(int a1, int a2, int a3, int a4, int a5, int a6, int a7, _DWORD *a8);
int __fastcall gralloc_unlock(int a1, _DWORD *a2);
int __fastcall gralloc_lock_ycbcr(int a1, _DWORD *a2, int a3, int a4, int a5, int a6, int a7, int a8);
int __fastcall j_fb_device_open(int a1, int a2, _DWORD *a3);

//-------------------------------------------------------------------------
// Data declarations

void *off_3E18 = (void *)0x18CD; // weak
void *off_4000 = &off_4000; // weak


//----- (00000E84) --------------------------------------------------------
void sub_E84()
{
  JUMPOUT(0);
}
// E90: control flows out of bounds to 0

//----- (00000EBC) --------------------------------------------------------
// attributes: thunk
int __fastcall grallocUnmap(int a1, int a2)
{
  return _Z12grallocUnmapPK16gralloc_module_tP16private_handle_t(a1, a2);
}

//----- (00000F1C) --------------------------------------------------------
// attributes: thunk
int __fastcall fb_device_open(int a1, int a2, _DWORD *a3)
{
  return _Z14fb_device_openPK11hw_module_tPKcPP11hw_device_t(a1, a2, a3);
}

//----- (00000F40) --------------------------------------------------------
// attributes: thunk
int __fastcall gralloc_register_buffer(int a1, _DWORD *a2)
{
  return _Z23gralloc_register_bufferPK16gralloc_module_tPK13native_handle(a1, a2);
}

//----- (00000F4C) --------------------------------------------------------
// attributes: thunk
int __fastcall gralloc_unregister_buffer(int a1, _DWORD *a2)
{
  return _Z25gralloc_unregister_bufferPK16gralloc_module_tPK13native_handle(a1, a2);
}

//----- (00000F58) --------------------------------------------------------
// attributes: thunk
int __fastcall gralloc_lock(int a1, int a2, int a3, int a4, int a5, int a6, int a7, _DWORD *a8)
{
  return _Z12gralloc_lockPK16gralloc_module_tPK13native_handleiiiiiPPv(a1, a2, a3, a4, a5, a6, a7, a8);
}

//----- (00000F64) --------------------------------------------------------
// attributes: thunk
int __fastcall gralloc_unlock(int a1, _DWORD *a2)
{
  return _Z14gralloc_unlockPK16gralloc_module_tPK13native_handle(a1, a2);
}

//----- (00000F70) --------------------------------------------------------
// attributes: thunk
int __fastcall gralloc_lock_ycbcr(int a1, _DWORD *a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
  return _Z18gralloc_lock_ycbcrPK16gralloc_module_tPK13native_handleiiiiiP13android_ycbcr(
           a1,
           a2,
           a3,
           a4,
           a5,
           a6,
           a7,
           a8);
}

//----- (00001000) --------------------------------------------------------
// attributes: thunk
int __fastcall init_fb(int a1)
{
  return _Z7init_fbP16private_module_t(a1);
}

//----- (00001090) --------------------------------------------------------
// attributes: thunk
int __fastcall getIonFd(int a1)
{
  return _Z8getIonFdPK16gralloc_module_t(a1);
}

//----- (000010C0) --------------------------------------------------------
int sub_10C0()
{
  return _cxa_finalize(&off_4000);
}
// 4000: using guessed type void *off_4000;

//----- (000010E0) --------------------------------------------------------
int (*__fastcall sub_10E0(int (*result)(void)))(void)
{
  if ( result )
    return (int (*)(void))result();
  return result;
}

//----- (0000110C) --------------------------------------------------------
int __fastcall sub_110C(void *a1)
{
  return _cxa_atexit((void (__fastcall *)(void *))sub_10E0, a1, &off_4000);
}
// 4000: using guessed type void *off_4000;

//----- (00001154) --------------------------------------------------------
int __fastcall sub_1154(int a1)
{
  if ( (a1 & 0x4000) != 0 )
  {
    if ( (a1 & 0x3000000) == 0x2000000 )
      return 1;
  }
  else if ( (a1 & 0x4000000) == 0 )
  {
    return 1;
  }
  return 16;
}

//----- (00001170) --------------------------------------------------------
static int gralloc_free(alloc_device_t* dev,
                        buffer_handle_t handle)
int __fastcall sub_1170(int a1, _DWORD *a2)
ok

//----- (000011E8) --------------------------------------------------------
static int gralloc_close(struct hw_device_t *dev)
int __fastcall sub_11E8(void *a1)
ok

//----- (000011F4) --------------------------------------------------------
int gralloc_device_open(const hw_module_t* module, const char* name,
                        hw_device_t** device)
int __fastcall sub_11F4(int a1, char *s1, _DWORD *a3)
ok

//----- (00001270) --------------------------------------------------------
static int gralloc_alloc(alloc_device_t* dev,
                         int w, int h, int format, int usage,
                         buffer_handle_t* pHandle, int* pStride)
int __fastcall sub_1270(int a1, int a2, int a3, int a4, int a5, _DWORD *a6, signed int *a7)
{
  int v7; // r10
  int v10; // r12
  int v11; // r11
  int v12; // r4
  int v13; // r12
  unsigned int v14; // r0
  signed int v15; // r7
  signed int v16; // r9
  int v17; // r10
  unsigned int v18; // lr
  int v19; // r0
  unsigned int v20; // r1
  int v21; // r0
  _DWORD *v22; // r4
  int v23; // r0
  int v24; // r7
  int v25; // r10
  unsigned int v26; // r3
  bool v27; // zf
  unsigned int v28; // r11
  int v29; // r0
  int v30; // r9
  int result; // r0
  _DWORD *v32; // r0
  int v33; // r3
  int v34; // r1
  unsigned int v35; // r12
  int v36; // r0
  int v37; // r1
  int v38; // r3
  int v39; // r9
  int v40; // r0
  int v41; // r3
  int v42; // r1
  int v43; // r0
  int v44; // r3
  int v45; // [sp+Ch] [bp-54h]
  int v46; // [sp+Ch] [bp-54h]
  unsigned int v47; // [sp+Ch] [bp-54h]
  unsigned int v48; // [sp+Ch] [bp-54h]
  int v49; // [sp+Ch] [bp-54h]
  int v51; // [sp+14h] [bp-4Ch]
  int v52; // [sp+18h] [bp-48h]
  int v53; // [sp+1Ch] [bp-44h]
  int v54; // [sp+20h] [bp-40h]
  int v55; // [sp+24h] [bp-3Ch]
  int v56; // [sp+28h] [bp-38h] BYREF
  int fd; // [sp+2Ch] [bp-34h] BYREF
  int v58; // [sp+30h] [bp-30h] BYREF
  int v59[11]; // [sp+34h] [bp-2Ch] BYREF

  if ( !a6 || !a7 || a2 <= 0 || a3 <= 0 )
    return -22;
  if ( (a5 & 0xF) == 3 )
  {
    if ( (a5 & 0x200) != 0 )
      v10 = 35;
    else
      v10 = 3;
  }
  else
  {
    v10 = 0;
  }
  v11 = *(_DWORD *)(a1 + 8);
  if ( (a5 & 0x8000000) != 0 )
    v10 |= 8u;
  v12 = *(_DWORD *)(v11 + 200);
  v45 = v10;
  v53 = sub_1154(a5);
  v13 = v45;
  if ( a4 != 5 )
  {
    if ( a4 > 5 )
    {
      if ( a4 == 33 )
      {
        v15 = a2;
        v16 = a3;
        v17 = a3 * a2;
        goto LABEL_35;
      }
      if ( a4 > 33 )
      {
        if ( a4 != 36 )
        {
          if ( a4 != 264 )
            goto LABEL_45;
          goto LABEL_24;
        }
      }
      else if ( a4 != 32 )
      {
        goto LABEL_45;
      }
    }
    else
    {
      if ( a4 == 3 )
      {
        v14 = 3;
        goto LABEL_28;
      }
      if ( a4 <= 3 )
      {
        if ( a4 < 1 )
          goto LABEL_45;
        goto LABEL_24;
      }
    }
    v14 = 2;
    goto LABEL_28;
  }
LABEL_24:
  v14 = 4;
LABEL_28:
  v16 = (a3 + 15) & 0xFFFFFFF0;
  if ( a3 + 1 >= v16 )
    v7 = a3 + 2;
  v18 = v14 * ((a2 + 15) & 0xFFFFFFF0);
  if ( a3 + 1 >= v16 )
    v7 *= v18;
  v15 = v18 / v14;
  if ( a3 + 1 < v16 )
    v7 = v16 * v18;
  v17 = v7 + 256;
LABEL_35:
  if ( (a5 & 0x4000) != 0 )
  {
    if ( (a5 & 0x3000000) == 50331648 )
    {
      v19 = v45 | 0x400000;
    }
    else
    {
      if ( (a5 & 0x200000) != 0 )
        v20 = v45 | 0x80000000;
      else
        v20 = v45 | 0x200000;
      v19 = v20 | 0x10;
    }
  }
  else
  {
    v19 = v45;
  }
  v21 = ion_alloc_fd(v12, v17, 0, v53, v19, v59);
  v13 = v45;
  if ( !v21 )
  {
    v22 = (_DWORD *)operator new(0x80u);
    v23 = v59[0];
    v22[4] = -1;
    v22[5] = -1;
    v22[6] = 51647890;
    v22[3] = v23;
    v22[7] = a5;
    v22[8] = v17;
    v22[10] = a4;
    v22[11] = a2;
    v22[12] = a3;
    v22[13] = v15;
    v22[9] = 0;
    v22[14] = v16;
    v22[15] = a4;
    v22[16] = 0;
    v22[17] = 0;
    v22[18] = 0;
LABEL_92:
    v34 = 1;
    *v22 = 12;
    v22[2] = 28;
    *((_QWORD *)v22 + 10) = 0LL;
    *((_QWORD *)v22 + 11) = 0LL;
    *((_QWORD *)v22 + 12) = 0LL;
LABEL_107:
    v22[1] = v34;
    result = 0;
    *a6 = v22;
    *a7 = v15;
    return result;
  }
LABEL_45:
  v24 = *(_DWORD *)(v11 + 200);
  v56 = -1;
  fd = -1;
  v58 = -1;
  v54 = v24;
  v15 = (a2 + 15) & 0xFFFFFFF0;
  if ( a4 == 34 )
  {
    if ( (a5 & 0x60000) == 393216 )
    {
      v25 = 20;
    }
    else
    {
      v25 = 285;
      if ( (a5 & 0x10100) == 0 )
        v25 = 34;
    }
  }
  else if ( a4 == 35 )
  {
    if ( (a5 & 0x20000) != 0 )
    {
      v25 = 17;
    }
    else
    {
      v25 = 35;
      v49 = v13;
      _android_log_print(
        6,
        "gralloc",
        "gralloc_alloc: Requested YCbCr_420_888, but no known specific format for this usage: %d x %d, usage %x",
        a2,
        a3,
        a5);
      v13 = v49;
    }
  }
  else
  {
    v25 = a4;
  }
  if ( (a5 & 0x4000) != 0 )
  {
    if ( (a5 & 0x200000) != 0 )
    {
      v26 = v13 | 0x80000000;
    }
    else if ( (a5 & 0x800000) != 0 )
    {
      v26 = v13 | 0x20000000;
    }
    else
    {
      v26 = v13 | 0x200000;
    }
    v13 = v26 | 0x10;
  }
  else if ( (a5 & 0x4000000) != 0 )
  {
    v13 |= 0x200000u;
  }
  if ( v25 == 263 )
  {
    v28 = (a3 + 31) & 0xFFFFFFE0;
    v52 = 2;
    v51 = v28 * v15 + 256;
    v55 = (((a3 >> 1) + 31) & 0xFFFFFFE0) * v15 + 256;
LABEL_96:
    v46 = v13;
    result = ion_alloc_fd(v54, v51, 0, v53, v13, &v56);
    v35 = v46;
    if ( result )
    {
      if ( (a5 & 0x800000) == 0 )
        return result;
      v47 = v46 & 0xDFDFFFFF | 0x200000;
      result = ion_alloc_fd(v54, v51, 0, v53, v47, &v56);
      v35 = v47;
      if ( result )
        return result;
    }
    if ( v52 == 1 )
    {
      v36 = operator new(0x80u);
      v37 = v56;
      v22 = (_DWORD *)v36;
      *(_DWORD *)(v36 + 16) = -1;
      *(_DWORD *)(v36 + 20) = -1;
      *(_DWORD *)(v36 + 12) = v37;
      *(_DWORD *)(v36 + 28) = a5;
      *(_DWORD *)(v36 + 48) = a3;
      *(_DWORD *)(v36 + 36) = 0;
      *(_QWORD *)(v36 + 80) = 0LL;
      *(_DWORD *)(v36 + 64) = 0;
      *(_QWORD *)(v36 + 88) = 0LL;
      *(_DWORD *)(v36 + 68) = 0;
      *(_QWORD *)(v36 + 96) = 0LL;
      *(_DWORD *)(v36 + 72) = 0;
      v38 = 28;
      *(_DWORD *)(v36 + 24) = 51647890;
      *(_DWORD *)(v36 + 32) = v51;
      *(_DWORD *)(v36 + 40) = v25;
      *(_DWORD *)(v36 + 44) = a2;
      *(_DWORD *)(v36 + 52) = v15;
      *(_DWORD *)(v36 + 56) = v28;
      *(_DWORD *)(v36 + 60) = a4;
      *(_DWORD *)v36 = 12;
LABEL_105:
      v22[2] = v38;
      v34 = v52;
      goto LABEL_107;
    }
    v48 = v35;
    v39 = ion_alloc_fd(v54, v55, 0, v53, v35, &fd);
    if ( !v39 )
    {
      if ( v52 != 3 )
      {
        v22 = (_DWORD *)operator new(0x80u);
        v43 = v56;
        v44 = fd;
        v22[7] = a5;
        v22[5] = -1;
        v34 = 2;
        v22[3] = v43;
        *((_QWORD *)v22 + 10) = 0LL;
        v22[4] = v44;
        *((_QWORD *)v22 + 11) = 0LL;
        v22[12] = a3;
        *((_QWORD *)v22 + 12) = 0LL;
        v22[6] = 51647890;
        v22[8] = v51;
        v22[9] = 0;
        v22[10] = v25;
        v22[11] = a2;
        v22[13] = v15;
        v22[14] = v28;
        v22[15] = a4;
        v22[16] = 0;
        v22[17] = 0;
        v22[18] = 0;
        *v22 = 12;
        v22[2] = 27;
        goto LABEL_107;
      }
      v39 = ion_alloc_fd(v54, v55, 0, v53, v48, &v58);
      if ( !v39 )
      {
        v22 = (_DWORD *)operator new(0x80u);
        v40 = v56;
        v41 = fd;
        v42 = v58;
        v22[7] = a5;
        v22[12] = a3;
        v22[3] = v40;
        *((_QWORD *)v22 + 10) = 0LL;
        v22[4] = v41;
        *((_QWORD *)v22 + 11) = 0LL;
        v38 = 26;
        v22[5] = v42;
        *((_QWORD *)v22 + 12) = 0LL;
        v22[6] = 51647890;
        v22[8] = v51;
        v22[9] = 0;
        v22[10] = v25;
        v22[11] = a2;
        v22[13] = v15;
        v22[14] = v28;
        v22[15] = a4;
        v22[16] = 0;
        v22[17] = 0;
        v22[18] = 0;
        *v22 = 12;
        goto LABEL_105;
      }
      close(fd);
    }
    close(v56);
    return v39;
  }
  if ( v25 <= 263 )
  {
    if ( v25 == 20 )
    {
      v28 = a3;
      v52 = 1;
      v55 = 0;
      v51 = v15 * 2 * a3 + 256;
      goto LABEL_96;
    }
    if ( v25 <= 20 )
    {
      v27 = v25 == 17;
      goto LABEL_78;
    }
    if ( v25 != 257 )
    {
      if ( v25 != 261 )
        goto LABEL_87;
      goto LABEL_93;
    }
    goto LABEL_80;
  }
  if ( v25 <= 286 )
  {
    if ( v25 > 284 )
    {
LABEL_93:
      v28 = (a3 + 15) & 0xFFFFFFF0;
      v29 = 2;
      v51 = v28 * v15 + 256;
      v55 = v15 * (v28 >> 1) + 256;
      goto LABEL_94;
    }
    if ( v25 != 284 )
      goto LABEL_87;
LABEL_80:
    v28 = (a3 + 15) & 0xFFFFFFF0;
    v15 = (a2 + 31) & 0xFFFFFFE0;
    v29 = 3;
    v51 = v28 * v15 + 256;
    v55 = (v28 >> 1) * ((v15 / 2 + 15) & 0xFFFFFFF0) + 256;
LABEL_94:
    v52 = v29;
    goto LABEL_96;
  }
  if ( v25 == 287 )
    goto LABEL_82;
  v27 = v25 == 842094169;
LABEL_78:
  if ( !v27 )
    goto LABEL_87;
LABEL_82:
  if ( v25 != 287 && v25 != 842094169 )
  {
    if ( v25 == 17 )
    {
      v15 = a2;
      v30 = ((3 * a3 * a2) >> 1) + 256;
      goto LABEL_88;
    }
LABEL_87:
    _android_log_print(6, "gralloc", "invalid yuv format %d\n", v25);
    return -22;
  }
  v30 = ((a3 + 15) & 0xFFFFFFF0) * (((v15 / 2 + 15) & 0xFFFFFFF0) + v15) + 256;
LABEL_88:
  if ( a4 == 35 )
    v15 = 0;
  result = ion_alloc_fd(v54, v30, 0, v53, v13, v59);
  if ( !result )
  {
    v32 = (_DWORD *)operator new(0x80u);
    v33 = v59[0];
    v32[7] = a5;
    v22 = v32;
    v32[8] = v30;
    v32[4] = -1;
    v32[5] = -1;
    v32[3] = v33;
    v32[6] = 51647890;
    v32[9] = 0;
    v32[10] = v25;
    v32[11] = a2;
    v32[12] = a3;
    v32[13] = v15;
    v32[14] = a3;
    v32[15] = a4;
    v32[16] = 0;
    v32[17] = 0;
    v32[18] = 0;
    goto LABEL_92;
  }
  return result;
}
// 131E: conditional instruction was optimized away because r8.4 is in (1..2|4..5|==20|==24|==108)
// 1820: conditional instruction was optimized away because r0.4!=0
// 1346: variable 'v7' is possibly undefined
// EB0: using guessed type int _android_log_print(_DWORD, _DWORD, const char *, ...);
// F28: using guessed type int __fastcall ion_alloc_fd(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD);


//----- (000018C0) --------------------------------------------------------
int __fastcall sub_18C0(void *a1)
{
  if ( a1 )
    free(a1);
  return 0;
}



//----- (00001908) --------------------------------------------------------
int __fastcall sub_1908(_DWORD *a1)
{
  if ( a1 && *a1 == 12 && a1[1] + a1[2] == 29 && a1[6] == 51647890 )
    return 0;
  _android_log_print(6, "gralloc", "invalid gralloc handle (at %p)", a1);
  return -22;
}
// EB0: using guessed type int _android_log_print(_DWORD, _DWORD, const char *, ...);

//----- (0000194C) --------------------------------------------------------
int __fastcall sub_194C(int a1, _DWORD *a2)
{
  int v4; // r6
  android::VectorImpl *v5; // r4
  pthread_mutex_t *v6; // r6
  _DWORD *v7; // r8
  int v8; // r1
  void (__fastcall *v9)(int, _DWORD *); // r7
  int v10; // r5

  if ( sub_1908(a2) < 0 )
    return -22;
  v4 = *(_DWORD *)(a1 + 8);
  v5 = *(android::VectorImpl **)(v4 + 456);
  v6 = (pthread_mutex_t *)(v4 + 460);
  pthread_mutex_lock(v6);
  if ( *((_DWORD *)v5 + 2) )
  {
    if ( sub_1908(a2) )
      v7 = 0;
    else
      v7 = a2;
    v8 = *((_DWORD *)v5 + 1) + 8 * (*((_DWORD *)v5 + 2) + 0x1FFFFFFF);
    v9 = *(void (__fastcall **)(int, _DWORD *))v8;
    v10 = *(_DWORD *)(v8 + 4);
    android::VectorImpl::pop(v5);
    pthread_mutex_unlock(v6);
    v9(v10, v7);
  }
  else
  {
    pthread_mutex_unlock(v6);
  }
  return 0;
}

//----- (000019C0) --------------------------------------------------------
int __fastcall init_fb(int a1)
{
  int v2; // r0
  int v3; // r7
  int v4; // r0
  int v6; // r0
  float v7; // s17
  float v8; // s19
  float v9; // s18
  int v10; // r0
  int v11; // r5
  int v12[40]; // [sp+38h] [bp-120h] BYREF
  char v13[68]; // [sp+D8h] [bp-80h] BYREF

  v2 = open("/dev/graphics/fb0", 2);
  v3 = v2;
  if ( v2 < 0 )
  {
    v4 = _android_log_print(6, "gralloc", "/dev/graphics/fb0 Open fail");
    return -*(_DWORD *)_errno(v4);
  }
  if ( ioctl(v2, 0x4602u, v13) == -1 )
  {
    _android_log_print(6, "gralloc", "Fail to get FB Screen Info");
LABEL_5:
    v4 = close(v3);
    return -*(_DWORD *)_errno(v4);
  }
  if ( ioctl(v3, 0x4600u, v12) == -1 )
  {
    _android_log_print(6, "gralloc", "First, Fail to get FB VScreen Info");
    goto LABEL_5;
  }
  v6 = 0x38D7EA4C68000LL
     / ((unsigned int)(v12[29] + v12[28] + v12[1])
      * (unsigned __int64)(unsigned int)(v12[27] + v12[26] + v12[0])
      * (unsigned int)v12[25]);
  if ( !v6 )
    v6 = 60000;
  v7 = (float)v6 / 1000.0;
  v8 = (float)((float)(unsigned int)v12[0] * 25.4) / (float)(unsigned int)v12[23];
  v9 = (float)((float)(unsigned int)v12[1] * 25.4) / (float)(unsigned int)v12[22];
  _android_log_print(
    4,
    "gralloc",
    "using (id=%s)\n"
    "xres         = %d px\n"
    "yres         = %d px\n"
    "width        = %d mm (%f dpi)\n"
    "height       = %d mm (%f dpi)\n"
    "refresh rate = %.2f Hz\n",
    v13,
    v12[0],
    v12[1],
    v12[23],
    v8,
    v12[22],
    v9,
    v7);
  *(float *)(a1 + 444) = v8;
  v10 = v12[0];
  v11 = v12[1];
  *(float *)(a1 + 448) = v9;
  *(float *)(a1 + 452) = v7;
  *(_DWORD *)(a1 + 432) = v10;
  *(_DWORD *)(a1 + 440) = v10;
  *(_DWORD *)(a1 + 436) = v11;
  memcpy((void *)(a1 + 204), v12, 0xA0u);
  memcpy((void *)(a1 + 364), v13, 0x44u);
  close(v3);
  return 0;
}
// EB0: using guessed type int _android_log_print(_DWORD, _DWORD, const char *, ...);
// FD0: using guessed type int __fastcall _errno(_DWORD);

//----- (00001B90) --------------------------------------------------------
int __fastcall fb_device_open(int a1, int a2, _DWORD *a3)
{
  int inited; // r6
  _DWORD *v6; // r4
  android::VectorImpl *v7; // r8
  int v8; // r0
  int v9; // r3
  int v10; // r5

  inited = (**(int (__fastcall ***)(int))(a1 + 20))(a1);
  if ( inited >= 0 )
  {
    v6 = malloc(0xB8u);
    if ( v6 )
    {
      inited = init_fb(a1);
      if ( inited >= 0 )
      {
        inited = 0;
        memset(v6, 0, 0xB8u);
        v6[2] = a1;
        *v6 = 1213678676;
        v6[15] = sub_18C0;
        v6[36] = sub_194C;
        v7 = (android::VectorImpl *)operator new(0x14u);
        android::VectorImpl::VectorImpl(v7, 8u, 0);
        *(_DWORD *)v7 = &off_3E18;
        *(_DWORD *)(a1 + 456) = v7;
        pthread_mutex_init((pthread_mutex_t *)(a1 + 460), 0);
        v8 = *(_DWORD *)(a1 + 440);
        v6[16] = 0;
        v6[17] = *(_DWORD *)(a1 + 432);
        v9 = *(_DWORD *)(a1 + 436);
        v6[19] = v8 / 4;
        v6[20] = 1;
        v6[18] = v9;
        v6[21] = *(_DWORD *)(a1 + 444);
        v6[22] = *(_DWORD *)(a1 + 448);
        v10 = *(_DWORD *)(a1 + 452);
        v6[24] = 1;
        v6[25] = 1;
        v6[23] = v10;
        *a3 = v6;
        return inited;
      }
      _android_log_print(6, "gralloc", "Fail to init framebuffer");
      free(v6);
    }
    else
    {
      _android_log_print(6, "gralloc", "Failed to allocate memory for dev");
    }
    (*(void (**)(void))(a2 + 60))();
  }
  else
  {
    _android_log_print(6, "gralloc", "Fail to Open gralloc device");
  }
  return inited;
}
// EB0: using guessed type int _android_log_print(_DWORD, _DWORD, const char *, ...);
// 194C: using guessed type int sub_194C();
// 3E18: using guessed type void *off_3E18;

//----- (00001CBC) --------------------------------------------------------
int __fastcall sub_1CBC(int a1)
{
  unsigned int v2; // r5
  int v3; // r1
  int v4; // r5
  unsigned int v5; // r1
  unsigned int v6; // r5
  __int64 v7; // r0
  int v8; // r0
  int *v9; // r0
  char *v10; // r0
  int v11; // r1
  __int64 v12; // r0
  int v13; // r0
  int *v14; // r0
  char *v15; // r0
  __int64 v16; // r0
  int v17; // r0
  int *v18; // r0
  char *v19; // r0

  v2 = *(_DWORD *)(a1 + 40) - 257;
  if ( v2 > 0x1D )
  {
    v4 = 0;
  }
  else
  {
    v3 = 1 << v2;
    if ( ((1 << v2) & 0x8000001) != 0 )
    {
      v5 = *(_DWORD *)(a1 + 56) / 2;
      v6 = (*(_DWORD *)(a1 + 52) / 2 + 15) & 0xFFFFFFF0;
    }
    else
    {
      if ( (v3 & 0x30000010) != 0 )
      {
        v5 = (*(_DWORD *)(a1 + 56) / 2 + 7) & 0xFFFFFFF8;
      }
      else
      {
        v4 = (1 << v2) & 0x40;
        if ( (v3 & 0x40) == 0 )
          goto LABEL_11;
        v5 = (*(_DWORD *)(a1 + 48) / 2 + 31) & 0xFFFFFFE0;
      }
      v6 = *(_DWORD *)(a1 + 52);
    }
    v4 = v6 * v5 + 256;
  }
LABEL_11:
  v7 = *(_QWORD *)(a1 + 80);
  if ( v7 )
  {
    v8 = munmap((void *)v7, *(_DWORD *)(a1 + 32));
    if ( v8 < 0 )
    {
      v9 = (int *)_errno(v8);
      v10 = strerror(*v9);
      _android_log_print(
        6,
        "gralloc",
        "%s :could not unmap %s %llx %d",
        "int gralloc_unmap(const gralloc_module_t*, buffer_handle_t)",
        v10,
        *(_QWORD *)(a1 + 80),
        *(_DWORD *)(a1 + 32));
    }
    v11 = *(_DWORD *)(a1 + 16);
    *(_QWORD *)(a1 + 80) = 0LL;
    if ( v11 >= 0 )
    {
      v12 = *(_QWORD *)(a1 + 88);
      if ( !v12 )
        return 0;
      v13 = munmap((void *)v12, v4);
      if ( v13 < 0 )
      {
        v14 = (int *)_errno(v13);
        v15 = strerror(*v14);
        _android_log_print(
          6,
          "gralloc",
          "%s :could not unmap %s %llx %d",
          "int gralloc_unmap(const gralloc_module_t*, buffer_handle_t)",
          v15,
          *(_QWORD *)(a1 + 88),
          v4);
      }
      *(_QWORD *)(a1 + 88) = 0LL;
    }
    if ( *(int *)(a1 + 20) >= 0 )
    {
      v16 = *(_QWORD *)(a1 + 96);
      if ( v16 )
      {
        v17 = munmap((void *)v16, v4);
        if ( v17 < 0 )
        {
          v18 = (int *)_errno(v17);
          v19 = strerror(*v18);
          _android_log_print(
            6,
            "gralloc",
            "%s :could not unmap %s %llx %d",
            "int gralloc_unmap(const gralloc_module_t*, buffer_handle_t)",
            v19,
            *(_QWORD *)(a1 + 96),
            v4);
        }
        *(_QWORD *)(a1 + 96) = 0LL;
      }
    }
  }
  return 0;
}
// EB0: using guessed type int _android_log_print(_DWORD, _DWORD, const char *, ...);
// FD0: using guessed type int __fastcall _errno(_DWORD);

//----- (00001E3C) --------------------------------------------------------
int __fastcall getIonFd(int a1)
{
  if ( *(_DWORD *)(a1 + 200) == -1 )
    *(_DWORD *)(a1 + 200) = ion_open();
  return *(_DWORD *)(a1 + 200);
}
// F10: using guessed type int ion_open(void);

//----- (00001E58) --------------------------------------------------------
int __fastcall grallocMap(int a1, int a2)
{
  unsigned int v4; // r6
  int v5; // r0
  int v6; // r6
  unsigned int v7; // r6
  unsigned int v8; // r5
  int v9; // r2
  void *v11; // r0
  int *v12; // r7
  char *v13; // r0
  int IonFd; // r0
  int v15; // r0
  int v16; // r0

  v4 = *(_DWORD *)(a2 + 40) - 257;
  if ( v4 > 0x1D )
  {
    v6 = 0;
  }
  else
  {
    v5 = 1 << v4;
    if ( ((1 << v4) & 0x8000001) != 0 )
    {
      v7 = *(_DWORD *)(a2 + 56) / 2;
      v8 = (*(_DWORD *)(a2 + 52) / 2 + 15) & 0xFFFFFFF0;
    }
    else
    {
      if ( (v5 & 0x30000010) != 0 )
      {
        v7 = (*(_DWORD *)(a2 + 56) / 2 + 7) & 0xFFFFFFF8;
      }
      else
      {
        v6 = (1 << v4) & 0x40;
        if ( (v5 & 0x40) == 0 )
          goto LABEL_11;
        v7 = (*(_DWORD *)(a2 + 48) / 2 + 31) & 0xFFFFFFE0;
      }
      v8 = *(_DWORD *)(a2 + 52);
    }
    v6 = v8 * v7 + 256;
  }
LABEL_11:
  v9 = *(_DWORD *)(a2 + 28);
  if ( (v9 & 0x2004000) == 0x4000 )
    return 0;
  if ( (v9 & 0x8004000) != 0 )
  {
    *(_QWORD *)(a2 + 96) = 0LL;
    *(_QWORD *)(a2 + 88) = 0LL;
    *(_QWORD *)(a2 + 80) = 0LL;
    return 0;
  }
  v11 = mmap(0, *(_DWORD *)(a2 + 32), 3, 1, *(_DWORD *)(a2 + 12), 0);
  if ( v11 == (void *)-1 )
  {
    v12 = (int *)_errno(-1);
    v13 = strerror(*v12);
    _android_log_print(
      6,
      "gralloc",
      "%s: could not mmap %s",
      "int gralloc_map(const gralloc_module_t*, buffer_handle_t)",
      v13);
    return -*v12;
  }
  else
  {
    *(_QWORD *)(a2 + 80) = (int)v11;
    IonFd = getIonFd(a1);
    ion_sync_fd(IonFd, *(_DWORD *)(a2 + 12));
    if ( *(int *)(a2 + 16) >= 0 )
    {
      *(_QWORD *)(a2 + 88) = vshrd_n_s64(
                               vdup_n_s32((unsigned int)mmap(0, v6, 3, 1, *(_DWORD *)(a2 + 16), 0)).n64_i64[0],
                               0x20u);
      v15 = getIonFd(a1);
      ion_sync_fd(v15, *(_DWORD *)(a2 + 16));
    }
    if ( *(int *)(a2 + 20) < 0 )
      return 0;
    *(_QWORD *)(a2 + 96) = vshrd_n_s64(
                             vdup_n_s32((unsigned int)mmap(0, v6, 3, 1, *(_DWORD *)(a2 + 20), 0)).n64_i64[0],
                             0x20u);
    v16 = getIonFd(a1);
    ion_sync_fd(v16, *(_DWORD *)(a2 + 20));
    return 0;
  }
}
// EB0: using guessed type int _android_log_print(_DWORD, _DWORD, const char *, ...);
// FD0: using guessed type int __fastcall _errno(_DWORD);
// 109C: using guessed type int __fastcall ion_sync_fd(_DWORD, _DWORD);

//----- (00001FCC) --------------------------------------------------------
int __fastcall grallocUnmap(int a1, int a2)
{
  return sub_1CBC(a2);
}

//----- (00001FD4) --------------------------------------------------------
int __fastcall gralloc_register_buffer(int a1, _DWORD *a2)
{
  int v4; // r6
  int IonFd; // r0
  int v6; // r0
  int v7; // r0

  if ( sub_1908(a2) < 0 )
    return -22;
  v4 = grallocMap(a1, (int)a2);
  IonFd = getIonFd(a1);
  if ( ion_import(IonFd, a2[3], a2 + 16) )
    _android_log_print(6, "gralloc", "error importing handle %d %x\n", a2[3], a2[10]);
  if ( (int)a2[4] >= 0 )
  {
    v6 = getIonFd(a1);
    if ( ion_import(v6, a2[4], a2 + 17) )
      _android_log_print(6, "gralloc", "error importing handle1 %d %x\n", a2[4], a2[10]);
  }
  if ( (int)a2[5] >= 0 )
  {
    v7 = getIonFd(a1);
    if ( ion_import(v7, a2[5], a2 + 18) )
      _android_log_print(6, "gralloc", "error importing handle2 %d %x\n", a2[5], a2[10]);
  }
  return v4;
}
// EB0: using guessed type int _android_log_print(_DWORD, _DWORD, const char *, ...);
// 10A8: using guessed type int __fastcall ion_import(_DWORD, _DWORD, _DWORD);

//----- (00002094) --------------------------------------------------------
int __fastcall gralloc_unregister_buffer(int a1, _DWORD *a2)
{
  int IonFd; // r0
  int v5; // r0
  int result; // r0
  int v7; // r0

  if ( sub_1908(a2) < 0 )
    return -22;
  sub_1CBC((int)a2);
  if ( a2[16] )
  {
    IonFd = getIonFd(a1);
    ion_free(IonFd, a2[16]);
  }
  if ( a2[17] )
  {
    v5 = getIonFd(a1);
    ion_free(v5, a2[17]);
  }
  result = a2[18];
  if ( result )
  {
    v7 = getIonFd(a1);
    ion_free(v7, a2[18]);
    return 0;
  }
  return result;
}
// 10B4: using guessed type int __fastcall ion_free(_DWORD, _DWORD);

//----- (000020E4) --------------------------------------------------------
int __fastcall gralloc_lock(int a1, int a2, int a3, int a4, int a5, int a6, int a7, _DWORD *a8)
{
  if ( sub_1908((_DWORD *)a2) < 0 )
    return -22;
  if ( *(_DWORD *)(a2 + 60) == 35 )
  {
    _android_log_print(6, "gralloc", "gralloc_lock can't be used with YCbCr_420_888 format");
    return -22;
  }
  if ( !*(_QWORD *)(a2 + 80) )
    grallocMap(a1, a2);
  *a8 = *(_DWORD *)(a2 + 80);
  if ( *(int *)(a2 + 16) >= 0 )
    a8[1] = *(_DWORD *)(a2 + 88);
  if ( *(int *)(a2 + 20) >= 0 )
    a8[2] = *(_DWORD *)(a2 + 96);
  return 0;
}
// EB0: using guessed type int _android_log_print(_DWORD, _DWORD, const char *, ...);

//----- (0000214C) --------------------------------------------------------
int __fastcall gralloc_unlock(int a1, _DWORD *a2)
{
  int IonFd; // r0
  int v5; // r0
  int v6; // r0

  if ( sub_1908(a2) < 0 )
    return -22;
  if ( (a2[7] & 0xF) == 3 )
  {
    IonFd = getIonFd(a1);
    ion_sync_fd(IonFd, a2[3]);
    if ( (int)a2[4] >= 0 )
    {
      v5 = getIonFd(a1);
      ion_sync_fd(v5, a2[4]);
    }
    if ( (int)a2[5] >= 0 )
    {
      v6 = getIonFd(a1);
      ion_sync_fd(v6, a2[5]);
    }
  }
  return 0;
}
// 109C: using guessed type int __fastcall ion_sync_fd(_DWORD, _DWORD);

//----- (000021A0) --------------------------------------------------------
int __fastcall gralloc_lock_ycbcr(int a1, _DWORD *a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
  int v10; // r3
  int v11; // r3
  int v12; // r3
  int v13; // r0
  int v14; // r5

  if ( sub_1908(a2) >= 0 )
  {
    if ( a8 )
    {
      v10 = a2[15];
      if ( v10 == 35 )
      {
        if ( (a3 & 0x2000F) != 0 )
        {
          v11 = a2[10];
          if ( v11 == 17 )
          {
            v12 = a2[11];
            v13 = a2[12] * v12;
            *(_DWORD *)a8 = a2[20];
            *(_DWORD *)(a8 + 4) = a2[20] + 1 + v13;
            v14 = a2[20];
            *(_DWORD *)(a8 + 12) = v12;
            *(_DWORD *)(a8 + 16) = v12;
            *(_DWORD *)(a8 + 20) = 2;
            *(_DWORD *)(a8 + 8) = v13 + v14;
            memset((void *)(a8 + 24), 0, 0x20u);
            _android_log_print(
              3,
              "gralloc",
              "gralloc_lock_ycbcr success. usage: %x, ycbcr.y: %p, .cb: %p, .cr: %p, .ystride: %d , .cstride: %d, .chroma_step: %d",
              a3,
              *(const void **)a8,
              *(const void **)(a8 + 4),
              *(const void **)(a8 + 8),
              *(_DWORD *)(a8 + 12),
              *(_DWORD *)(a8 + 16),
              *(_DWORD *)(a8 + 20));
            return 0;
          }
          _android_log_print(6, "gralloc", "gralloc_lock_ycbcr unexpected internal format %x", v11);
        }
        else
        {
          _android_log_print(6, "gralloc", "gralloc_lock_ycbcr usage mismatch usage:0x%x\n", a3);
        }
      }
      else
      {
        _android_log_print(
          6,
          "gralloc",
          "gralloc_lock_ycbcr can only be used with HAL_PIXEL_FORMAT_YCbCr_420_888, got %x instead",
          v10);
      }
    }
    else
    {
      _android_log_print(6, "gralloc", "gralloc_lock_ycbcr got NULL ycbcr struct");
    }
  }
  return -22;
}
// EB0: using guessed type int _android_log_print(_DWORD, _DWORD, const char *, ...);

//----- (00002290) --------------------------------------------------------
// attributes: thunk
int __fastcall j_fb_device_open(int a1, int a2, _DWORD *a3)
{
  return fb_device_open(a1, a2, a3);
}

// nfuncs=110 queued=44 decompiled=44 lumina nreq=0 worse=0 better=0
// ALL OK, 44 function(s) have been successfully decompiled
