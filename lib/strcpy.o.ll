; ModuleID = 'gyoji LLVM Code Generator'
source_filename = "gyoji LLVM Code Generator"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@0 = internal constant [5 x i8] c"abc\0A\00"

define void @"gyoji::std::string::strcpy"(ptr %dst, ptr %src) {
entry:
  %src2 = alloca ptr, align 8
  %dst1 = alloca ptr, align 8
  store ptr %dst, ptr %dst1, align 8
  store ptr %src, ptr %src2, align 8
  br label %BB0

BB0:                                              ; preds = %entry
  br label %BB1

BB1:                                              ; preds = %BB2, %BB0
  %0 = load ptr, ptr %src2, align 8
  %1 = load i8, ptr %0, align 1
  %2 = icmp ne i8 %1, 0
  br i1 %2, label %BB2, label %BB3

BB2:                                              ; preds = %BB1
  %3 = load ptr, ptr %dst1, align 8
  %4 = load i8, ptr %3, align 1
  %5 = load ptr, ptr %src2, align 8
  %6 = load i8, ptr %5, align 1
  store i8 %6, ptr %3, align 1
  %7 = load ptr, ptr %dst1, align 8
  %8 = getelementptr inbounds i8, ptr %7, i32 1
  store ptr %8, ptr %dst1, align 8
  %9 = load ptr, ptr %src2, align 8
  %10 = getelementptr inbounds i8, ptr %9, i32 1
  store ptr %10, ptr %src2, align 8
  br label %BB1

BB3:                                              ; preds = %BB1
  ret void
}

define i32 @"gyoji::std::string::strlen"(ptr %str) {
entry:
  %str1 = alloca ptr, align 8
  store ptr %str, ptr %str1, align 8
  br label %BB0

BB0:                                              ; preds = %entry
  %len = alloca i32, align 4
  %0 = load i32, ptr %len, align 4
  store i32 0, ptr %len, align 4
  br label %BB1

BB1:                                              ; preds = %BB2, %BB0
  %1 = load ptr, ptr %str1, align 8
  %2 = load i8, ptr %1, align 1
  %3 = icmp ne i8 %2, 0
  br i1 %3, label %BB2, label %BB3

BB2:                                              ; preds = %BB1
  %4 = load ptr, ptr %str1, align 8
  %5 = getelementptr inbounds i8, ptr %4, i32 1
  store ptr %5, ptr %str1, align 8
  %6 = load i32, ptr %len, align 4
  %7 = add i32 %6, 1
  store i32 %7, ptr %len, align 4
  br label %BB1

BB3:                                              ; preds = %BB1
  %8 = load i32, ptr %len, align 4
  ret i32 %8
}

define i32 @main(i32 %argc, ptr %argv) {
entry:
  %argv2 = alloca ptr, align 8
  %argc1 = alloca i32, align 4
  store i32 %argc, ptr %argc1, align 4
  store ptr %argv, ptr %argv2, align 8
  br label %BB0

BB0:                                              ; preds = %entry
  %source = alloca ptr, align 8
  %0 = load ptr, ptr %source, align 8
  store ptr @0, ptr %source, align 8
  %dest = alloca ptr, align 8
  %1 = load ptr, ptr %dest, align 8
  %2 = call ptr @malloc(i32 5)
  store ptr %2, ptr %dest, align 8
  %3 = load ptr, ptr %dest, align 8
  %4 = load ptr, ptr %source, align 8
  call void @"gyoji::std::string::strcpy"(ptr %3, ptr %4)
  %5 = load ptr, ptr %source, align 8
  %6 = load ptr, ptr %source, align 8
  %7 = call i32 @"gyoji::std::string::strlen"(ptr %6)
  %8 = call i32 @write(i32 1, ptr %5, i32 %7)
  %9 = load ptr, ptr %dest, align 8
  %10 = load ptr, ptr %dest, align 8
  %11 = call i32 @"gyoji::std::string::strlen"(ptr %10)
  %12 = call i32 @write(i32 1, ptr %9, i32 %11)
  %13 = load ptr, ptr %dest, align 8
  call void @free(ptr %13)
  ret i32 0
}

declare ptr @malloc(i32)

declare i32 @write(i32, ptr, i32)

declare void @free(ptr)
