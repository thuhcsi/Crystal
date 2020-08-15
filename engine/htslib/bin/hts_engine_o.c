/* ----------------------------------------------------------------- */
/*           The HMM-Based Speech Synthesis Engine "hts_engine API"  */
/*           developed by HTS Working Group                          */
/*           http://hts-engine.sourceforge.net/                      */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2001-2011  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                                                                   */
/*                2001-2008  Tokyo Institute of Technology           */
/*                           Interdisciplinary Graduate School of    */
/*                           Science and Engineering                 */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the HTS working group nor the names of its  */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission.   */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

#ifndef HTS_ENGINE_C
#define HTS_ENGINE_C

#ifdef __cplusplus
#define HTS_ENGINE_C_START extern "C" {
#define HTS_ENGINE_C_END   }
#else
#define HTS_ENGINE_C_START
#define HTS_ENGINE_C_END
#endif                          /* __CPLUSPLUS */

HTS_ENGINE_C_START;

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "HTS_engine.h"

/* Usage: output usage */
void Usage(void)
{
   HTS_show_copyright(stderr);
   fprintf(stderr, "\n");
   fprintf(stderr, "hts_engine - The HMM-based speech synthesis engine \"hts_engine API\"\n");
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       hts_engine [ options ] [ infile ] \n");
   fprintf(stderr, "  options:                                                                   [  def][ min--max]\n");
   fprintf(stderr, "    -td tree       : decision tree files for state duration                  [  N/A]\n");
   fprintf(stderr, "    -tm tree       : decision tree files for spectrum                        [  N/A]\n");
   fprintf(stderr, "    -tf tree       : decision tree files for Log F0                          [  N/A]\n");
   fprintf(stderr, "    -tl tree       : decision tree files for low-pass filter                 [  N/A]\n");
   fprintf(stderr, "    -md pdf        : model files for state duration                          [  N/A]\n");
   fprintf(stderr, "    -mm pdf        : model files for spectrum                                [  N/A]\n");
   fprintf(stderr, "    -mf pdf        : model files for Log F0                                  [  N/A]\n");
   fprintf(stderr, "    -ml pdf        : model files for low-pass filter                         [  N/A]\n");
   fprintf(stderr, "    -dm win        : window files for calculation delta of spectrum          [  N/A]\n");
   fprintf(stderr, "    -df win        : window files for calculation delta of Log F0            [  N/A]\n");
   fprintf(stderr, "    -dl win        : window files for calculation delta of low-pass filter   [  N/A]\n");
   fprintf(stderr, "    -od s          : filename of output label with duration                  [  N/A]\n");
   fprintf(stderr, "    -om s          : filename of output spectrum                             [  N/A]\n");
   fprintf(stderr, "    -of s          : filename of output Log F0                               [  N/A]\n");
   fprintf(stderr, "    -ol s          : filename of output low-pass filter                      [  N/A]\n");
   fprintf(stderr, "    -or s          : filename of output raw audio (generated speech)         [  N/A]\n");
   fprintf(stderr, "    -ow s          : filename of output wav audio (generated speech)         [  N/A]\n");
   fprintf(stderr, "    -ot s          : filename of output trace information                    [  N/A]\n");
   fprintf(stderr, "    -vp            : use phoneme alignment for duration                      [  N/A]\n");
   fprintf(stderr, "    -i  i f1 .. fi : enable interpolation & specify number(i),coefficient(f) [    1][   1-- ]\n");
   fprintf(stderr, "    -s  i          : sampling frequency                                      [16000][   1--48000]\n");
   fprintf(stderr, "    -p  i          : frame period (point)                                    [   80][   1--]\n");
   fprintf(stderr, "    -a  f          : all-pass constant                                       [ 0.42][ 0.0--1.0]\n");
   fprintf(stderr, "    -g  i          : gamma = -1 / i (if i=0 then gamma=0)                    [    0][   0-- ]\n");
   fprintf(stderr, "    -b  f          : postfiltering coefficient                               [  0.0][-0.8--0.8]\n");
   fprintf(stderr, "    -l             : regard input as log gain and output linear one (LSP)    [  N/A]\n");
   fprintf(stderr, "    -r  f          : speech speed rate                                       [  1.0][ 0.0--10.0]\n");
   fprintf(stderr, "    -fm f          : add half-tone                                           [  0.0][-24.0--24.0]\n");
   fprintf(stderr, "    -u  f          : voiced/unvoiced threshold                               [  0.5][ 0.0--1.0]\n");
   fprintf(stderr, "    -em tree       : decision tree files for GV of spectrum                  [  N/A]\n");
   fprintf(stderr, "    -ef tree       : decision tree files for GV of Log F0                    [  N/A]\n");
   fprintf(stderr, "    -el tree       : decision tree files for GV of low-pass filter           [  N/A]\n");
   fprintf(stderr, "    -cm pdf        : filenames of GV for spectrum                            [  N/A]\n");
   fprintf(stderr, "    -cf pdf        : filenames of GV for Log F0                              [  N/A]\n");
   fprintf(stderr, "    -cl pdf        : filenames of GV for low-pass filter                     [  N/A]\n");
   fprintf(stderr, "    -jm f          : weight of GV for spectrum                               [  1.0][ 0.0--2.0]\n");
   fprintf(stderr, "    -jf f          : weight of GV for Log F0                                 [  1.0][ 0.0--2.0]\n");
   fprintf(stderr, "    -jl f          : weight of GV for low-pass filter                        [  1.0][ 0.0--2.0]\n");
   fprintf(stderr, "    -k  tree       : GV switch                                               [  N/A]\n");
   fprintf(stderr, "    -z  i          : audio buffer size                                       [ 1600][   0--48000]\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "    label file\n");
   fprintf(stderr, "  note:\n");
   fprintf(stderr, "    option '-d' may be repeated to use multiple delta parameters.\n");
   fprintf(stderr, "    generated spectrum, log F0, and low-pass filter coefficient\n");
   fprintf(stderr, "    sequences are saved in natural endian, binary (float) format.\n");
   fprintf(stderr, "\n");

   exit(0);
}

/* Error: output error message */
void Error(const int error, char *message, ...)
{
   va_list arg;

   fflush(stdout);
   fflush(stderr);

   if (error > 0)
      fprintf(stderr, "\nError: ");
   else
      fprintf(stderr, "\nWarning: ");

   va_start(arg, message);
   vfprintf(stderr, message, arg);
   va_end(arg);

   fflush(stderr);

   if (error > 0)
      exit(error);
}

/* GetNumInterp: get number of speakers for interpolation from argv */
int GetNumInterp(int argc, char **argv_search)
{
   int num_interp = 1;
   while (--argc) {
      if (**++argv_search == '-') {
         if (*(*argv_search + 1) == 'i') {
            num_interp = atoi(*++argv_search);
            if (num_interp < 1) {
               num_interp = 1;
            }
            --argc;
         }
      }
   }
   return (num_interp);
}

int main(int argc, char **argv)
{
   int i;
   double f;
   char *labfn = NULL;
   HTS_File *durfp = NULL, *mgcfp = NULL, *lf0fp = NULL, *lpffp = NULL;
   HTS_File *wavfp = NULL, *rawfp = NULL, *tracefp = NULL;

   /* number of speakers for interpolation */
   int num_interp = 0;
   double *rate_interp = NULL;

   /* file names of models */
   char **fn_ms_dur;
   char **fn_ms_mgc;
   char **fn_ms_lf0;
   char **fn_ms_lpf;
   /* number of each models for interpolation */
   int num_ms_dur = 0, num_ms_mgc = 0, num_ms_lf0 = 0, num_ms_lpf = 0;

   /* file names of trees */
   char **fn_ts_dur;
   char **fn_ts_mgc;
   char **fn_ts_lf0;
   char **fn_ts_lpf;
   /* number of each trees for interpolation */
   int num_ts_dur = 0, num_ts_mgc = 0, num_ts_lf0 = 0, num_ts_lpf = 0;

   /* file names of windows */
   char **fn_ws_mgc;
   char **fn_ws_lf0;
   char **fn_ws_lpf;
   int num_ws_mgc = 0, num_ws_lf0 = 0, num_ws_lpf = 0;

   /* file names of global variance */
   char **fn_ms_gvm = NULL;
   char **fn_ms_gvl = NULL;
   char **fn_ms_gvf = NULL;
   int num_ms_gvm = 0, num_ms_gvl = 0, num_ms_gvf = 0;

   /* file names of global variance trees */
   char **fn_ts_gvm = NULL;
   char **fn_ts_gvl = NULL;
   char **fn_ts_gvf = NULL;
   int num_ts_gvm = 0, num_ts_gvl = 0, num_ts_gvf = 0;

   /* file name of global variance switch */
   char *fn_gv_switch = NULL;

   /* global parameter */
   int sampling_rate = 16000;
   int fperiod = 80;
   double alpha = 0.42;
   int stage = 0;               /* Gamma=-1/stage: if stage=0 then Gamma=0 */
   double beta = 0.0;
   int audio_buff_size = 1600;
   double uv_threshold = 0.5;
   double gv_weight_mgc = 1.0;
   double gv_weight_lf0 = 1.0;
   double gv_weight_lpf = 1.0;

   double half_tone = 0.0;
   HTS_Boolean phoneme_alignment = FALSE;
   double speech_speed = 1.0;
   HTS_Boolean use_log_gain = FALSE;

   /* engine */
   HTS_Engine engine;

   /* parse command line */
   if (argc == 1)
      Usage();

   /* delta window handler for mel-cepstrum */
   fn_ws_mgc = (char **) calloc(argc, sizeof(char *));
   /* delta window handler for log f0 */
   fn_ws_lf0 = (char **) calloc(argc, sizeof(char *));
   /* delta window handler for low-pass filter */
   fn_ws_lpf = (char **) calloc(argc, sizeof(char *));

   /* prepare for interpolation */
   num_interp = GetNumInterp(argc, argv);
   rate_interp = (double *) calloc(num_interp, sizeof(double));
   for (i = 0; i < num_interp; i++)
      rate_interp[i] = 1.0;

   fn_ms_dur = (char **) calloc(num_interp, sizeof(char *));
   fn_ms_mgc = (char **) calloc(num_interp, sizeof(char *));
   fn_ms_lf0 = (char **) calloc(num_interp, sizeof(char *));
   fn_ms_lpf = (char **) calloc(num_interp, sizeof(char *));
   fn_ts_dur = (char **) calloc(num_interp, sizeof(char *));
   fn_ts_mgc = (char **) calloc(num_interp, sizeof(char *));
   fn_ts_lf0 = (char **) calloc(num_interp, sizeof(char *));
   fn_ts_lpf = (char **) calloc(num_interp, sizeof(char *));
   fn_ms_gvm = (char **) calloc(num_interp, sizeof(char *));
   fn_ms_gvl = (char **) calloc(num_interp, sizeof(char *));
   fn_ms_gvf = (char **) calloc(num_interp, sizeof(char *));
   fn_ts_gvm = (char **) calloc(num_interp, sizeof(char *));
   fn_ts_gvl = (char **) calloc(num_interp, sizeof(char *));
   fn_ts_gvf = (char **) calloc(num_interp, sizeof(char *));

   /* read command */
   while (--argc) {
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'v':
            switch (*(*argv + 2)) {
            case 'p':
               phoneme_alignment = TRUE;
               break;
            default:
               Error(1, "hts_engine: Invalid option '-v%c'.\n", *(*argv + 2));
            }
            break;
         case 't':
            switch (*(*argv + 2)) {
            case 'd':
               fn_ts_dur[num_ts_dur++] = *++argv;
               break;
            case 'm':
               fn_ts_mgc[num_ts_mgc++] = *++argv;
               break;
            case 'f':
            case 'p':
               fn_ts_lf0[num_ts_lf0++] = *++argv;
               break;
            case 'l':
               fn_ts_lpf[num_ts_lpf++] = *++argv;
               break;
            default:
               Error(1, "hts_engine: Invalid option '-t%c'.\n", *(*argv + 2));
            }
            --argc;
            break;
         case 'm':
            switch (*(*argv + 2)) {
            case 'd':
               fn_ms_dur[num_ms_dur++] = *++argv;
               break;
            case 'm':
               fn_ms_mgc[num_ms_mgc++] = *++argv;
               break;
            case 'f':
            case 'p':
               fn_ms_lf0[num_ms_lf0++] = *++argv;
               break;
            case 'l':
               fn_ms_lpf[num_ms_lpf++] = *++argv;
               break;
            default:
               Error(1, "hts_engine: Invalid option '-m%c'.\n", *(*argv + 2));
            }
            --argc;
            break;
         case 'd':
            switch (*(*argv + 2)) {
            case 'm':
               fn_ws_mgc[num_ws_mgc++] = *++argv;
               break;
            case 'f':
            case 'p':
               fn_ws_lf0[num_ws_lf0++] = *++argv;
               break;
            case 'l':
               fn_ws_lpf[num_ws_lpf++] = *++argv;
               break;
            default:
               Error(1, "hts_engine: Invalid option '-d%c'.\n", *(*argv + 2));
            }
            --argc;
            break;
         case 'o':
            switch (*(*argv + 2)) {
            case 'w':
               wavfp = HTS_fopen(*++argv, "wb");
               break;
            case 'r':
               rawfp = HTS_fopen(*++argv, "wb");
               break;
            case 'd':
               durfp = HTS_fopen(*++argv, "wt");
               break;
            case 'm':
               mgcfp = HTS_fopen(*++argv, "wb");
               break;
            case 'f':
            case 'p':
               lf0fp = HTS_fopen(*++argv, "wb");
               break;
            case 'l':
               lpffp = HTS_fopen(*++argv, "wb");
               break;
            case 't':
               tracefp = HTS_fopen(*++argv, "wt");
               break;
            default:
               Error(1, "hts_engine: Invalid option '-o%c'.\n", *(*argv + 2));
            }
            --argc;
            break;
         case 'h':
            Usage();
            break;
         case 's':
            sampling_rate = atoi(*++argv);
            --argc;
            break;
         case 'p':
            fperiod = atoi(*++argv);
            --argc;
            break;
         case 'a':
            alpha = atof(*++argv);
            --argc;
            break;
         case 'g':
            stage = atoi(*++argv);
            --argc;
            break;
         case 'l':
            use_log_gain = TRUE;
            break;
         case 'b':
            beta = atof(*++argv);
            --argc;
            break;
         case 'r':
            speech_speed = atof(*++argv);
            --argc;
            break;
         case 'f':
            switch (*(*argv + 2)) {
            case 'm':
               f = atof(*++argv);
               if (f < -24.0)
                  f = -24.0;
               if (f > 24.0)
                  f = 24.0;
               half_tone = f;
               break;
            default:
               Error(1, "hts_engine: Invalid option '-f%c'.\n", *(*argv + 2));
            }
            --argc;
            break;
         case 'u':
            uv_threshold = atof(*++argv);
            --argc;
            break;
         case 'i':
            ++argv;
            argc--;
            for (i = 0; i < num_interp; i++) {
               rate_interp[i] = atof(*++argv);
               argc--;
            }
            break;
         case 'e':
            switch (*(*argv + 2)) {
            case 'm':
               fn_ts_gvm[num_ts_gvm++] = *++argv;
               break;
            case 'f':
            case 'p':
               fn_ts_gvl[num_ts_gvl++] = *++argv;
               break;
            case 'l':
               fn_ts_gvf[num_ts_gvf++] = *++argv;
               break;
            default:
               Error(1, "hts_engine: Invalid option '-e%c'.\n", *(*argv + 2));
            }
            --argc;
            break;
         case 'c':
            switch (*(*argv + 2)) {
            case 'm':
               fn_ms_gvm[num_ms_gvm++] = *++argv;
               break;
            case 'f':
            case 'p':
               fn_ms_gvl[num_ms_gvl++] = *++argv;
               break;
            case 'l':
               fn_ms_gvf[num_ms_gvf++] = *++argv;
               break;
            default:
               Error(1, "hts_engine: Invalid option '-c%c'.\n", *(*argv + 2));
            }
            --argc;
            break;
         case 'j':
            switch (*(*argv + 2)) {
            case 'm':
               gv_weight_mgc = atof(*++argv);
               break;
            case 'f':
            case 'p':
               gv_weight_lf0 = atof(*++argv);
               break;
            case 'l':
               gv_weight_lpf = atof(*++argv);
               break;
            default:
               Error(1, "hts_engine: Invalid option '-j%c'.\n", *(*argv + 2));
            }
            --argc;
            break;
         case 'k':
            fn_gv_switch = *++argv;
            --argc;
            break;
         case 'z':
            audio_buff_size = atoi(*++argv);
            --argc;
            break;
         default:
            Error(1, "hts_engine: Invalid option '-%c'.\n", *(*argv + 1));
         }
      } else {
         labfn = *argv;
      }
   }
   /* number of models,trees check */
   if (num_interp != num_ts_dur || num_interp != num_ts_mgc || num_interp != num_ts_lf0 || num_interp != num_ms_dur || num_interp != num_ms_mgc || num_interp != num_ms_lf0) {
      Error(1, "hts_engine: specify %d models(trees) for each parameter.\n", num_interp);
   }
   if (num_ms_lpf > 0 || num_ts_lpf > 0) {
      if (num_interp != num_ms_lpf || num_interp != num_ts_lpf) {
         Error(1, "hts_engine: specify %d models(trees) for each parameter.\n", num_interp);
      }
   }

   /* initialize (stream[0] = spectrum, stream[1] = lf0, stream[2] = low-pass filter) */
   if (num_ms_lpf > 0 || num_ts_lpf > 0) {
      HTS_Engine_initialize(&engine, 3);
   } else {
      HTS_Engine_initialize(&engine, 2);
   }

   /* load duration model */
   HTS_Engine_load_duration_from_fn(&engine, fn_ms_dur, fn_ts_dur, num_interp);
   /* load stream[0] (spectrum model) */
   HTS_Engine_load_parameter_from_fn(&engine, fn_ms_mgc, fn_ts_mgc, fn_ws_mgc, 0, FALSE, num_ws_mgc, num_interp);
   /* load stream[1] (lf0 model) */
   HTS_Engine_load_parameter_from_fn(&engine, fn_ms_lf0, fn_ts_lf0, fn_ws_lf0, 1, TRUE, num_ws_lf0, num_interp);
   /* load stream[2] (low-pass filter model) */
   if (num_ms_lpf > 0 || num_ts_lpf > 0)
      HTS_Engine_load_parameter_from_fn(&engine, fn_ms_lpf, fn_ts_lpf, fn_ws_lpf, 2, FALSE, num_ws_lpf, num_interp);
   /* load gv[0] (GV for spectrum) */
   if (num_interp == num_ms_gvm) {
      if (num_ms_gvm == num_ts_gvm)
         HTS_Engine_load_gv_from_fn(&engine, fn_ms_gvm, fn_ts_gvm, 0, num_interp);
      else
         HTS_Engine_load_gv_from_fn(&engine, fn_ms_gvm, NULL, 0, num_interp);
   }
   /* load gv[1] (GV for lf0) */
   if (num_interp == num_ms_gvl) {
      if (num_ms_gvl == num_ts_gvl)
         HTS_Engine_load_gv_from_fn(&engine, fn_ms_gvl, fn_ts_gvl, 1, num_interp);
      else
         HTS_Engine_load_gv_from_fn(&engine, fn_ms_gvl, NULL, 1, num_interp);
   }
   /* load gv[2] (GV for low-pass filter) */
   if (num_interp == num_ms_gvf && (num_ms_lpf > 0 || num_ts_lpf > 0)) {
      if (num_ms_gvf == num_ts_gvf)
         HTS_Engine_load_gv_from_fn(&engine, fn_ms_gvf, fn_ts_gvf, 0, num_interp);
      else
         HTS_Engine_load_gv_from_fn(&engine, fn_ms_gvf, NULL, 2, num_interp);
   }
   /* load GV switch */
   if (fn_gv_switch != NULL)
      HTS_Engine_load_gv_switch_from_fn(&engine, fn_gv_switch);

   /* set parameter */
   HTS_Engine_set_sampling_rate(&engine, sampling_rate);
   HTS_Engine_set_fperiod(&engine, fperiod);
   HTS_Engine_set_alpha(&engine, alpha);
   HTS_Engine_set_gamma(&engine, stage);
   HTS_Engine_set_log_gain(&engine, use_log_gain);
   HTS_Engine_set_beta(&engine, beta);
   HTS_Engine_set_audio_buff_size(&engine, audio_buff_size);
   HTS_Engine_set_msd_threshold(&engine, 1, uv_threshold);      /* set voiced/unvoiced threshold for stream[1] */
   HTS_Engine_set_gv_weight(&engine, 0, gv_weight_mgc);
   HTS_Engine_set_gv_weight(&engine, 1, gv_weight_lf0);
   if (num_ms_lpf > 0 || num_ts_lpf > 0)
      HTS_Engine_set_gv_weight(&engine, 2, gv_weight_lpf);
   for (i = 0; i < num_interp; i++) {
      HTS_Engine_set_duration_interpolation_weight(&engine, i, rate_interp[i]);
      HTS_Engine_set_parameter_interpolation_weight(&engine, 0, i, rate_interp[i]);
      HTS_Engine_set_parameter_interpolation_weight(&engine, 1, i, rate_interp[i]);
      if (num_ms_lpf > 0 || num_ts_lpf > 0)
         HTS_Engine_set_parameter_interpolation_weight(&engine, 2, i, rate_interp[i]);
   }
   if (num_interp == num_ms_gvm)
      for (i = 0; i < num_interp; i++)
         HTS_Engine_set_gv_interpolation_weight(&engine, 0, i, rate_interp[i]);
   if (num_interp == num_ms_gvl)
      for (i = 0; i < num_interp; i++)
         HTS_Engine_set_gv_interpolation_weight(&engine, 1, i, rate_interp[i]);
   if (num_interp == num_ms_gvf && (num_ms_lpf > 0 || num_ts_lpf > 0))
      for (i = 0; i < num_interp; i++)
         HTS_Engine_set_gv_interpolation_weight(&engine, 2, i, rate_interp[i]);

   /* synthesis */
   HTS_Engine_load_label_from_fn(&engine, labfn);       /* load label file */
   if (phoneme_alignment)       /* modify label */
      HTS_Label_set_frame_specified_flag(&engine.label, TRUE);
   if (speech_speed != 1.0)     /* modify label */
      HTS_Label_set_speech_speed(&engine.label, speech_speed);
   HTS_Engine_create_sstream(&engine);  /* parse label and determine state duration */
   if (half_tone != 0.0) {      /* modify f0 */
      for (i = 0; i < HTS_SStreamSet_get_total_state(&engine.sss); i++) {
         f = HTS_SStreamSet_get_mean(&engine.sss, 1, i, 0);
         f += half_tone * log(2.0) / 12;
         if (f < log(10.0))
            f = log(10.0);
         HTS_SStreamSet_set_mean(&engine.sss, 1, i, 0, f);
      }
   }
   HTS_Engine_create_pstream(&engine);  /* generate speech parameter vector sequence */
   HTS_Engine_create_gstream(&engine);  /* synthesize speech */

   /* output */
   if (tracefp != NULL)
      HTS_Engine_save_information(&engine, tracefp);
   if (durfp != NULL)
      HTS_Engine_save_label(&engine, durfp);
   if (rawfp)
      HTS_Engine_save_generated_speech(&engine, rawfp);
   if (wavfp)
      HTS_Engine_save_riff(&engine, wavfp);
   if (mgcfp)
      HTS_Engine_save_generated_parameter(&engine, mgcfp, 0);
   if (lf0fp)
      HTS_Engine_save_generated_parameter(&engine, lf0fp, 1);
   if (lpffp)
      HTS_Engine_save_generated_parameter(&engine, lpffp, 2);

   /* free */
   HTS_Engine_refresh(&engine);

   /* free memory */
   HTS_Engine_clear(&engine);
   free(rate_interp);
   free(fn_ws_mgc);
   free(fn_ws_lf0);
   free(fn_ws_lpf);
   free(fn_ms_mgc);
   free(fn_ms_lf0);
   free(fn_ms_lpf);
   free(fn_ms_dur);
   free(fn_ts_mgc);
   free(fn_ts_lf0);
   free(fn_ts_lpf);
   free(fn_ts_dur);
   free(fn_ms_gvm);
   free(fn_ms_gvl);
   free(fn_ms_gvf);
   free(fn_ts_gvm);
   free(fn_ts_gvl);
   free(fn_ts_gvf);

   /* close files */
   if (durfp != NULL)
      HTS_fclose(durfp);
   if (mgcfp != NULL)
      HTS_fclose(mgcfp);
   if (lf0fp != NULL)
      HTS_fclose(lf0fp);
   if (lpffp != NULL)
      HTS_fclose(lpffp);
   if (wavfp != NULL)
      HTS_fclose(wavfp);
   if (rawfp != NULL)
      HTS_fclose(rawfp);
   if (tracefp != NULL)
      HTS_fclose(tracefp);

   return 0;
}

HTS_ENGINE_C_END;

#endif                          /* !HTS_ENGINE_C */
