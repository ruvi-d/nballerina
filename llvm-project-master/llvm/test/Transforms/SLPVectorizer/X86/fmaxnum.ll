; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt < %s -mtriple=x86_64-unknown -basic-aa -slp-vectorizer -S | FileCheck %s --check-prefixes=CHECK,SSE
; RUN: opt < %s -mtriple=x86_64-unknown -mcpu=corei7-avx -basic-aa -slp-vectorizer -S | FileCheck %s --check-prefixes=CHECK,AVX,AVX256
; RUN: opt < %s -mtriple=x86_64-unknown -mcpu=bdver1 -basic-aa -slp-vectorizer -S | FileCheck %s --check-prefixes=CHECK,AVX,AVX256
; RUN: opt < %s -mtriple=x86_64-unknown -mcpu=core-avx2 -basic-aa -slp-vectorizer -S | FileCheck %s --check-prefixes=CHECK,AVX,AVX256
; RUN: opt < %s -mtriple=x86_64-unknown -mcpu=skylake-avx512 -mattr=-prefer-256-bit -basic-aa -slp-vectorizer -S | FileCheck %s --check-prefixes=CHECK,AVX,AVX512
; RUN: opt < %s -mtriple=x86_64-unknown -mcpu=skylake-avx512 -mattr=+prefer-256-bit -basic-aa -slp-vectorizer -S | FileCheck %s --check-prefixes=CHECK,AVX,AVX256

@srcA64 = common global [8 x double] zeroinitializer, align 64
@srcB64 = common global [8 x double] zeroinitializer, align 64
@srcC64 = common global [8 x double] zeroinitializer, align 64
@srcA32 = common global [16 x float] zeroinitializer, align 64
@srcB32 = common global [16 x float] zeroinitializer, align 64
@srcC32 = common global [16 x float] zeroinitializer, align 64
@dst64 = common global [8 x double] zeroinitializer, align 64
@dst32 = common global [16 x float] zeroinitializer, align 64

declare float @llvm.maxnum.f32(float, float)
declare double @llvm.maxnum.f64(double, double)

;
; CHECK
;

define void @fmaxnum_2f64() #0 {
; CHECK-LABEL: @fmaxnum_2f64(
; CHECK-NEXT:    [[TMP1:%.*]] = load <2 x double>, <2 x double>* bitcast ([8 x double]* @srcA64 to <2 x double>*), align 8
; CHECK-NEXT:    [[TMP2:%.*]] = load <2 x double>, <2 x double>* bitcast ([8 x double]* @srcB64 to <2 x double>*), align 8
; CHECK-NEXT:    [[TMP3:%.*]] = call <2 x double> @llvm.maxnum.v2f64(<2 x double> [[TMP1]], <2 x double> [[TMP2]])
; CHECK-NEXT:    store <2 x double> [[TMP3]], <2 x double>* bitcast ([8 x double]* @dst64 to <2 x double>*), align 8
; CHECK-NEXT:    ret void
;
  %a0 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 0), align 8
  %a1 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 1), align 8
  %b0 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 0), align 8
  %b1 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 1), align 8
  %fmaxnum0 = call double @llvm.maxnum.f64(double %a0, double %b0)
  %fmaxnum1 = call double @llvm.maxnum.f64(double %a1, double %b1)
  store double %fmaxnum0, double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 0), align 8
  store double %fmaxnum1, double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 1), align 8
  ret void
}

define void @fmaxnum_4f64() #0 {
; SSE-LABEL: @fmaxnum_4f64(
; SSE-NEXT:    [[TMP1:%.*]] = load <2 x double>, <2 x double>* bitcast ([8 x double]* @srcA64 to <2 x double>*), align 8
; SSE-NEXT:    [[TMP2:%.*]] = load <2 x double>, <2 x double>* bitcast (double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 2) to <2 x double>*), align 8
; SSE-NEXT:    [[TMP3:%.*]] = load <2 x double>, <2 x double>* bitcast ([8 x double]* @srcB64 to <2 x double>*), align 8
; SSE-NEXT:    [[TMP4:%.*]] = load <2 x double>, <2 x double>* bitcast (double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 2) to <2 x double>*), align 8
; SSE-NEXT:    [[TMP5:%.*]] = call <2 x double> @llvm.maxnum.v2f64(<2 x double> [[TMP1]], <2 x double> [[TMP3]])
; SSE-NEXT:    [[TMP6:%.*]] = call <2 x double> @llvm.maxnum.v2f64(<2 x double> [[TMP2]], <2 x double> [[TMP4]])
; SSE-NEXT:    store <2 x double> [[TMP5]], <2 x double>* bitcast ([8 x double]* @dst64 to <2 x double>*), align 8
; SSE-NEXT:    store <2 x double> [[TMP6]], <2 x double>* bitcast (double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 2) to <2 x double>*), align 8
; SSE-NEXT:    ret void
;
; AVX-LABEL: @fmaxnum_4f64(
; AVX-NEXT:    [[TMP1:%.*]] = load <4 x double>, <4 x double>* bitcast ([8 x double]* @srcA64 to <4 x double>*), align 8
; AVX-NEXT:    [[TMP2:%.*]] = load <4 x double>, <4 x double>* bitcast ([8 x double]* @srcB64 to <4 x double>*), align 8
; AVX-NEXT:    [[TMP3:%.*]] = call <4 x double> @llvm.maxnum.v4f64(<4 x double> [[TMP1]], <4 x double> [[TMP2]])
; AVX-NEXT:    store <4 x double> [[TMP3]], <4 x double>* bitcast ([8 x double]* @dst64 to <4 x double>*), align 8
; AVX-NEXT:    ret void
;
  %a0 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 0), align 8
  %a1 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 1), align 8
  %a2 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 2), align 8
  %a3 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 3), align 8
  %b0 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 0), align 8
  %b1 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 1), align 8
  %b2 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 2), align 8
  %b3 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 3), align 8
  %fmaxnum0 = call double @llvm.maxnum.f64(double %a0, double %b0)
  %fmaxnum1 = call double @llvm.maxnum.f64(double %a1, double %b1)
  %fmaxnum2 = call double @llvm.maxnum.f64(double %a2, double %b2)
  %fmaxnum3 = call double @llvm.maxnum.f64(double %a3, double %b3)
  store double %fmaxnum0, double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 0), align 8
  store double %fmaxnum1, double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 1), align 8
  store double %fmaxnum2, double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 2), align 8
  store double %fmaxnum3, double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 3), align 8
  ret void
}

define void @fmaxnum_8f64() #0 {
; SSE-LABEL: @fmaxnum_8f64(
; SSE-NEXT:    [[TMP1:%.*]] = load <2 x double>, <2 x double>* bitcast ([8 x double]* @srcA64 to <2 x double>*), align 4
; SSE-NEXT:    [[TMP2:%.*]] = load <2 x double>, <2 x double>* bitcast (double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 2) to <2 x double>*), align 4
; SSE-NEXT:    [[TMP3:%.*]] = load <2 x double>, <2 x double>* bitcast (double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 4) to <2 x double>*), align 4
; SSE-NEXT:    [[TMP4:%.*]] = load <2 x double>, <2 x double>* bitcast (double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 6) to <2 x double>*), align 4
; SSE-NEXT:    [[TMP5:%.*]] = load <2 x double>, <2 x double>* bitcast ([8 x double]* @srcB64 to <2 x double>*), align 4
; SSE-NEXT:    [[TMP6:%.*]] = load <2 x double>, <2 x double>* bitcast (double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 2) to <2 x double>*), align 4
; SSE-NEXT:    [[TMP7:%.*]] = load <2 x double>, <2 x double>* bitcast (double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 4) to <2 x double>*), align 4
; SSE-NEXT:    [[TMP8:%.*]] = load <2 x double>, <2 x double>* bitcast (double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 6) to <2 x double>*), align 4
; SSE-NEXT:    [[TMP9:%.*]] = call <2 x double> @llvm.maxnum.v2f64(<2 x double> [[TMP1]], <2 x double> [[TMP5]])
; SSE-NEXT:    [[TMP10:%.*]] = call <2 x double> @llvm.maxnum.v2f64(<2 x double> [[TMP2]], <2 x double> [[TMP6]])
; SSE-NEXT:    [[TMP11:%.*]] = call <2 x double> @llvm.maxnum.v2f64(<2 x double> [[TMP3]], <2 x double> [[TMP7]])
; SSE-NEXT:    [[TMP12:%.*]] = call <2 x double> @llvm.maxnum.v2f64(<2 x double> [[TMP4]], <2 x double> [[TMP8]])
; SSE-NEXT:    store <2 x double> [[TMP9]], <2 x double>* bitcast ([8 x double]* @dst64 to <2 x double>*), align 4
; SSE-NEXT:    store <2 x double> [[TMP10]], <2 x double>* bitcast (double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 2) to <2 x double>*), align 4
; SSE-NEXT:    store <2 x double> [[TMP11]], <2 x double>* bitcast (double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 4) to <2 x double>*), align 4
; SSE-NEXT:    store <2 x double> [[TMP12]], <2 x double>* bitcast (double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 6) to <2 x double>*), align 4
; SSE-NEXT:    ret void
;
; AVX256-LABEL: @fmaxnum_8f64(
; AVX256-NEXT:    [[TMP1:%.*]] = load <4 x double>, <4 x double>* bitcast ([8 x double]* @srcA64 to <4 x double>*), align 4
; AVX256-NEXT:    [[TMP2:%.*]] = load <4 x double>, <4 x double>* bitcast (double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 4) to <4 x double>*), align 4
; AVX256-NEXT:    [[TMP3:%.*]] = load <4 x double>, <4 x double>* bitcast ([8 x double]* @srcB64 to <4 x double>*), align 4
; AVX256-NEXT:    [[TMP4:%.*]] = load <4 x double>, <4 x double>* bitcast (double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 4) to <4 x double>*), align 4
; AVX256-NEXT:    [[TMP5:%.*]] = call <4 x double> @llvm.maxnum.v4f64(<4 x double> [[TMP1]], <4 x double> [[TMP3]])
; AVX256-NEXT:    [[TMP6:%.*]] = call <4 x double> @llvm.maxnum.v4f64(<4 x double> [[TMP2]], <4 x double> [[TMP4]])
; AVX256-NEXT:    store <4 x double> [[TMP5]], <4 x double>* bitcast ([8 x double]* @dst64 to <4 x double>*), align 4
; AVX256-NEXT:    store <4 x double> [[TMP6]], <4 x double>* bitcast (double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 4) to <4 x double>*), align 4
; AVX256-NEXT:    ret void
;
; AVX512-LABEL: @fmaxnum_8f64(
; AVX512-NEXT:    [[TMP1:%.*]] = load <8 x double>, <8 x double>* bitcast ([8 x double]* @srcA64 to <8 x double>*), align 4
; AVX512-NEXT:    [[TMP2:%.*]] = load <8 x double>, <8 x double>* bitcast ([8 x double]* @srcB64 to <8 x double>*), align 4
; AVX512-NEXT:    [[TMP3:%.*]] = call <8 x double> @llvm.maxnum.v8f64(<8 x double> [[TMP1]], <8 x double> [[TMP2]])
; AVX512-NEXT:    store <8 x double> [[TMP3]], <8 x double>* bitcast ([8 x double]* @dst64 to <8 x double>*), align 4
; AVX512-NEXT:    ret void
;
  %a0 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 0), align 4
  %a1 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 1), align 4
  %a2 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 2), align 4
  %a3 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 3), align 4
  %a4 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 4), align 4
  %a5 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 5), align 4
  %a6 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 6), align 4
  %a7 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcA64, i32 0, i64 7), align 4
  %b0 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 0), align 4
  %b1 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 1), align 4
  %b2 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 2), align 4
  %b3 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 3), align 4
  %b4 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 4), align 4
  %b5 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 5), align 4
  %b6 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 6), align 4
  %b7 = load double, double* getelementptr inbounds ([8 x double], [8 x double]* @srcB64, i32 0, i64 7), align 4
  %fmaxnum0 = call double @llvm.maxnum.f64(double %a0, double %b0)
  %fmaxnum1 = call double @llvm.maxnum.f64(double %a1, double %b1)
  %fmaxnum2 = call double @llvm.maxnum.f64(double %a2, double %b2)
  %fmaxnum3 = call double @llvm.maxnum.f64(double %a3, double %b3)
  %fmaxnum4 = call double @llvm.maxnum.f64(double %a4, double %b4)
  %fmaxnum5 = call double @llvm.maxnum.f64(double %a5, double %b5)
  %fmaxnum6 = call double @llvm.maxnum.f64(double %a6, double %b6)
  %fmaxnum7 = call double @llvm.maxnum.f64(double %a7, double %b7)
  store double %fmaxnum0, double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 0), align 4
  store double %fmaxnum1, double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 1), align 4
  store double %fmaxnum2, double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 2), align 4
  store double %fmaxnum3, double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 3), align 4
  store double %fmaxnum4, double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 4), align 4
  store double %fmaxnum5, double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 5), align 4
  store double %fmaxnum6, double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 6), align 4
  store double %fmaxnum7, double* getelementptr inbounds ([8 x double], [8 x double]* @dst64, i32 0, i64 7), align 4
  ret void
}

define void @fmaxnum_4f32() #0 {
; CHECK-LABEL: @fmaxnum_4f32(
; CHECK-NEXT:    [[TMP1:%.*]] = load <4 x float>, <4 x float>* bitcast ([16 x float]* @srcA32 to <4 x float>*), align 4
; CHECK-NEXT:    [[TMP2:%.*]] = load <4 x float>, <4 x float>* bitcast ([16 x float]* @srcB32 to <4 x float>*), align 4
; CHECK-NEXT:    [[TMP3:%.*]] = call <4 x float> @llvm.maxnum.v4f32(<4 x float> [[TMP1]], <4 x float> [[TMP2]])
; CHECK-NEXT:    store <4 x float> [[TMP3]], <4 x float>* bitcast ([16 x float]* @dst32 to <4 x float>*), align 4
; CHECK-NEXT:    ret void
;
  %a0 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 0), align 4
  %a1 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 1), align 4
  %a2 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 2), align 4
  %a3 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 3), align 4
  %b0 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 0), align 4
  %b1 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 1), align 4
  %b2 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 2), align 4
  %b3 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 3), align 4
  %fmaxnum0 = call float @llvm.maxnum.f32(float %a0, float %b0)
  %fmaxnum1 = call float @llvm.maxnum.f32(float %a1, float %b1)
  %fmaxnum2 = call float @llvm.maxnum.f32(float %a2, float %b2)
  %fmaxnum3 = call float @llvm.maxnum.f32(float %a3, float %b3)
  store float %fmaxnum0, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 0), align 4
  store float %fmaxnum1, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 1), align 4
  store float %fmaxnum2, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 2), align 4
  store float %fmaxnum3, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 3), align 4
  ret void
}

define void @fmaxnum_8f32() #0 {
; SSE-LABEL: @fmaxnum_8f32(
; SSE-NEXT:    [[TMP1:%.*]] = load <4 x float>, <4 x float>* bitcast ([16 x float]* @srcA32 to <4 x float>*), align 4
; SSE-NEXT:    [[TMP2:%.*]] = load <4 x float>, <4 x float>* bitcast (float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 4) to <4 x float>*), align 4
; SSE-NEXT:    [[TMP3:%.*]] = load <4 x float>, <4 x float>* bitcast ([16 x float]* @srcB32 to <4 x float>*), align 4
; SSE-NEXT:    [[TMP4:%.*]] = load <4 x float>, <4 x float>* bitcast (float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 4) to <4 x float>*), align 4
; SSE-NEXT:    [[TMP5:%.*]] = call <4 x float> @llvm.maxnum.v4f32(<4 x float> [[TMP1]], <4 x float> [[TMP3]])
; SSE-NEXT:    [[TMP6:%.*]] = call <4 x float> @llvm.maxnum.v4f32(<4 x float> [[TMP2]], <4 x float> [[TMP4]])
; SSE-NEXT:    store <4 x float> [[TMP5]], <4 x float>* bitcast ([16 x float]* @dst32 to <4 x float>*), align 4
; SSE-NEXT:    store <4 x float> [[TMP6]], <4 x float>* bitcast (float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 4) to <4 x float>*), align 4
; SSE-NEXT:    ret void
;
; AVX-LABEL: @fmaxnum_8f32(
; AVX-NEXT:    [[TMP1:%.*]] = load <8 x float>, <8 x float>* bitcast ([16 x float]* @srcA32 to <8 x float>*), align 4
; AVX-NEXT:    [[TMP2:%.*]] = load <8 x float>, <8 x float>* bitcast ([16 x float]* @srcB32 to <8 x float>*), align 4
; AVX-NEXT:    [[TMP3:%.*]] = call <8 x float> @llvm.maxnum.v8f32(<8 x float> [[TMP1]], <8 x float> [[TMP2]])
; AVX-NEXT:    store <8 x float> [[TMP3]], <8 x float>* bitcast ([16 x float]* @dst32 to <8 x float>*), align 4
; AVX-NEXT:    ret void
;
  %a0 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 0), align 4
  %a1 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 1), align 4
  %a2 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 2), align 4
  %a3 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 3), align 4
  %a4 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 4), align 4
  %a5 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 5), align 4
  %a6 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 6), align 4
  %a7 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 7), align 4
  %b0 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 0), align 4
  %b1 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 1), align 4
  %b2 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 2), align 4
  %b3 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 3), align 4
  %b4 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 4), align 4
  %b5 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 5), align 4
  %b6 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 6), align 4
  %b7 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 7), align 4
  %fmaxnum0 = call float @llvm.maxnum.f32(float %a0, float %b0)
  %fmaxnum1 = call float @llvm.maxnum.f32(float %a1, float %b1)
  %fmaxnum2 = call float @llvm.maxnum.f32(float %a2, float %b2)
  %fmaxnum3 = call float @llvm.maxnum.f32(float %a3, float %b3)
  %fmaxnum4 = call float @llvm.maxnum.f32(float %a4, float %b4)
  %fmaxnum5 = call float @llvm.maxnum.f32(float %a5, float %b5)
  %fmaxnum6 = call float @llvm.maxnum.f32(float %a6, float %b6)
  %fmaxnum7 = call float @llvm.maxnum.f32(float %a7, float %b7)
  store float %fmaxnum0, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 0), align 4
  store float %fmaxnum1, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 1), align 4
  store float %fmaxnum2, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 2), align 4
  store float %fmaxnum3, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 3), align 4
  store float %fmaxnum4, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 4), align 4
  store float %fmaxnum5, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 5), align 4
  store float %fmaxnum6, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 6), align 4
  store float %fmaxnum7, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 7), align 4
  ret void
}

define void @fmaxnum_16f32() #0 {
; SSE-LABEL: @fmaxnum_16f32(
; SSE-NEXT:    [[TMP1:%.*]] = load <4 x float>, <4 x float>* bitcast ([16 x float]* @srcA32 to <4 x float>*), align 4
; SSE-NEXT:    [[TMP2:%.*]] = load <4 x float>, <4 x float>* bitcast (float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 4) to <4 x float>*), align 4
; SSE-NEXT:    [[TMP3:%.*]] = load <4 x float>, <4 x float>* bitcast (float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 8) to <4 x float>*), align 4
; SSE-NEXT:    [[TMP4:%.*]] = load <4 x float>, <4 x float>* bitcast (float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 12) to <4 x float>*), align 4
; SSE-NEXT:    [[TMP5:%.*]] = load <4 x float>, <4 x float>* bitcast ([16 x float]* @srcB32 to <4 x float>*), align 4
; SSE-NEXT:    [[TMP6:%.*]] = load <4 x float>, <4 x float>* bitcast (float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 4) to <4 x float>*), align 4
; SSE-NEXT:    [[TMP7:%.*]] = load <4 x float>, <4 x float>* bitcast (float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 8) to <4 x float>*), align 4
; SSE-NEXT:    [[TMP8:%.*]] = load <4 x float>, <4 x float>* bitcast (float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 12) to <4 x float>*), align 4
; SSE-NEXT:    [[TMP9:%.*]] = call <4 x float> @llvm.maxnum.v4f32(<4 x float> [[TMP1]], <4 x float> [[TMP5]])
; SSE-NEXT:    [[TMP10:%.*]] = call <4 x float> @llvm.maxnum.v4f32(<4 x float> [[TMP2]], <4 x float> [[TMP6]])
; SSE-NEXT:    [[TMP11:%.*]] = call <4 x float> @llvm.maxnum.v4f32(<4 x float> [[TMP3]], <4 x float> [[TMP7]])
; SSE-NEXT:    [[TMP12:%.*]] = call <4 x float> @llvm.maxnum.v4f32(<4 x float> [[TMP4]], <4 x float> [[TMP8]])
; SSE-NEXT:    store <4 x float> [[TMP9]], <4 x float>* bitcast ([16 x float]* @dst32 to <4 x float>*), align 4
; SSE-NEXT:    store <4 x float> [[TMP10]], <4 x float>* bitcast (float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 4) to <4 x float>*), align 4
; SSE-NEXT:    store <4 x float> [[TMP11]], <4 x float>* bitcast (float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 8) to <4 x float>*), align 4
; SSE-NEXT:    store <4 x float> [[TMP12]], <4 x float>* bitcast (float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 12) to <4 x float>*), align 4
; SSE-NEXT:    ret void
;
; AVX256-LABEL: @fmaxnum_16f32(
; AVX256-NEXT:    [[TMP1:%.*]] = load <8 x float>, <8 x float>* bitcast ([16 x float]* @srcA32 to <8 x float>*), align 4
; AVX256-NEXT:    [[TMP2:%.*]] = load <8 x float>, <8 x float>* bitcast (float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 8) to <8 x float>*), align 4
; AVX256-NEXT:    [[TMP3:%.*]] = load <8 x float>, <8 x float>* bitcast ([16 x float]* @srcB32 to <8 x float>*), align 4
; AVX256-NEXT:    [[TMP4:%.*]] = load <8 x float>, <8 x float>* bitcast (float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 8) to <8 x float>*), align 4
; AVX256-NEXT:    [[TMP5:%.*]] = call <8 x float> @llvm.maxnum.v8f32(<8 x float> [[TMP1]], <8 x float> [[TMP3]])
; AVX256-NEXT:    [[TMP6:%.*]] = call <8 x float> @llvm.maxnum.v8f32(<8 x float> [[TMP2]], <8 x float> [[TMP4]])
; AVX256-NEXT:    store <8 x float> [[TMP5]], <8 x float>* bitcast ([16 x float]* @dst32 to <8 x float>*), align 4
; AVX256-NEXT:    store <8 x float> [[TMP6]], <8 x float>* bitcast (float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 8) to <8 x float>*), align 4
; AVX256-NEXT:    ret void
;
; AVX512-LABEL: @fmaxnum_16f32(
; AVX512-NEXT:    [[TMP1:%.*]] = load <16 x float>, <16 x float>* bitcast ([16 x float]* @srcA32 to <16 x float>*), align 4
; AVX512-NEXT:    [[TMP2:%.*]] = load <16 x float>, <16 x float>* bitcast ([16 x float]* @srcB32 to <16 x float>*), align 4
; AVX512-NEXT:    [[TMP3:%.*]] = call <16 x float> @llvm.maxnum.v16f32(<16 x float> [[TMP1]], <16 x float> [[TMP2]])
; AVX512-NEXT:    store <16 x float> [[TMP3]], <16 x float>* bitcast ([16 x float]* @dst32 to <16 x float>*), align 4
; AVX512-NEXT:    ret void
;
  %a0  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64  0), align 4
  %a1  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64  1), align 4
  %a2  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64  2), align 4
  %a3  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64  3), align 4
  %a4  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64  4), align 4
  %a5  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64  5), align 4
  %a6  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64  6), align 4
  %a7  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64  7), align 4
  %a8  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64  8), align 4
  %a9  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64  9), align 4
  %a10 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 10), align 4
  %a11 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 11), align 4
  %a12 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 12), align 4
  %a13 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 13), align 4
  %a14 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 14), align 4
  %a15 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcA32, i32 0, i64 15), align 4
  %b0  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64  0), align 4
  %b1  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64  1), align 4
  %b2  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64  2), align 4
  %b3  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64  3), align 4
  %b4  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64  4), align 4
  %b5  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64  5), align 4
  %b6  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64  6), align 4
  %b7  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64  7), align 4
  %b8  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64  8), align 4
  %b9  = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64  9), align 4
  %b10 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 10), align 4
  %b11 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 11), align 4
  %b12 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 12), align 4
  %b13 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 13), align 4
  %b14 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 14), align 4
  %b15 = load float, float* getelementptr inbounds ([16 x float], [16 x float]* @srcB32, i32 0, i64 15), align 4
  %fmaxnum0  = call float @llvm.maxnum.f32(float %a0 , float %b0 )
  %fmaxnum1  = call float @llvm.maxnum.f32(float %a1 , float %b1 )
  %fmaxnum2  = call float @llvm.maxnum.f32(float %a2 , float %b2 )
  %fmaxnum3  = call float @llvm.maxnum.f32(float %a3 , float %b3 )
  %fmaxnum4  = call float @llvm.maxnum.f32(float %a4 , float %b4 )
  %fmaxnum5  = call float @llvm.maxnum.f32(float %a5 , float %b5 )
  %fmaxnum6  = call float @llvm.maxnum.f32(float %a6 , float %b6 )
  %fmaxnum7  = call float @llvm.maxnum.f32(float %a7 , float %b7 )
  %fmaxnum8  = call float @llvm.maxnum.f32(float %a8 , float %b8 )
  %fmaxnum9  = call float @llvm.maxnum.f32(float %a9 , float %b9 )
  %fmaxnum10 = call float @llvm.maxnum.f32(float %a10, float %b10)
  %fmaxnum11 = call float @llvm.maxnum.f32(float %a11, float %b11)
  %fmaxnum12 = call float @llvm.maxnum.f32(float %a12, float %b12)
  %fmaxnum13 = call float @llvm.maxnum.f32(float %a13, float %b13)
  %fmaxnum14 = call float @llvm.maxnum.f32(float %a14, float %b14)
  %fmaxnum15 = call float @llvm.maxnum.f32(float %a15, float %b15)
  store float %fmaxnum0 , float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64  0), align 4
  store float %fmaxnum1 , float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64  1), align 4
  store float %fmaxnum2 , float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64  2), align 4
  store float %fmaxnum3 , float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64  3), align 4
  store float %fmaxnum4 , float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64  4), align 4
  store float %fmaxnum5 , float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64  5), align 4
  store float %fmaxnum6 , float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64  6), align 4
  store float %fmaxnum7 , float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64  7), align 4
  store float %fmaxnum8 , float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64  8), align 4
  store float %fmaxnum9 , float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64  9), align 4
  store float %fmaxnum10, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 10), align 4
  store float %fmaxnum11, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 11), align 4
  store float %fmaxnum12, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 12), align 4
  store float %fmaxnum13, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 13), align 4
  store float %fmaxnum14, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 14), align 4
  store float %fmaxnum15, float* getelementptr inbounds ([16 x float], [16 x float]* @dst32, i32 0, i64 15), align 4
  ret void
}

attributes #0 = { nounwind }