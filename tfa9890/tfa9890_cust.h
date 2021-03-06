#ifndef TFA9890_CUST_H
#define TFA9890_CUST_H

#define TFA_I2CDEVICE "/dev/i2c-7" //dev node provides the i2c read/write interface.
#define TFA_I2CSLAVEBASE (0x34)  // tfa device slave address
#define SAMPLE_RATE 48000 //init sample rate(unit Hz)
//#define BYPASS_DSP //use smart pa as a normal class D PA.
/* Before the echo cancellation argo in handsfree mode is ok, when in call mode, bypass the dsp. 
 * When MTK's new EC argo with smart pa is ok, remove this part. */
//#define BYPASS_DSP_INCALL 
#ifdef BYPASS_DSP_INCALL
#undef BYPASS_DSP
#endif

/* the base speaker file, containing tCoef */
/* Dumbo speaker
#define SPEAKER_FILENAME "KS_13X18_DUMBO.speaker"
#define PRESET_FILENAME "Example_0_0.KS_13X18_DUMBO.preset"
#define EQ_FILENAME "Example_0_0.KS_13X18_DUMBO.eq"*/
/* Sambo speaker*/
//#define SPEAKER_FILENAME "KS_90_13x18_Sambo_V2R2.speaker"
//#define PRESET_FILENAME "Seltech_HQ_V2R2_0_0_KS_90_13x18_Sambo_V2R2.preset"
//#define EQ_FILENAME "Seltech_HQ_V2R2_0_0_KS_90_13x18_Sambo_V2R2.eq"
//#define CONFIG_FILENAME "Setup.config"
//#define PATCH_FILENAME "TFA9890_N1C3_1_5_1.patch"

#define COLDBOOTPATCH_FILENAME "coldboot.patch"


#define SPEAKER_FILENAME "MZ_M86_speaker.speaker"
#define PRESET_FILENAME "music_preset_0.preset"
#define EQ_FILENAME "music_eq_0.eq"
#define CONFIG_FILENAME "TFA9890_N1B12_N1C3_v3.config"
#define PATCH_FILENAME "TFA9890_N1C3_2_1_1.patch"


#define LOCATION_FILES "/etc/tfa98xx/"




#endif/*TFA9890_CUST_H*/
