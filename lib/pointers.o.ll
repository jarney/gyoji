; ModuleID = 'gyoji LLVM Code Generator'
source_filename = "gyoji LLVM Code Generator"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%"gyoji::std::pointers::Owned" = type { ptr }

@0 = internal constant [17 x i8] c"Freeing pointer\0A\00"
@1 = internal constant [20 x i8] c"Allocating pointer\0A\00"

define void @do_write(ptr %buf) {
entry:
  %buf1 = alloca ptr, align 8
  store ptr %buf, ptr %buf1, align 8
  br label %BB0

BB0:                                              ; preds = %entry
  %0 = load ptr, ptr %buf1, align 8
  %1 = load ptr, ptr %buf1, align 8
  %2 = call i32 @strlen(ptr %1)
  call void @write(i32 0, ptr %0, i32 %2)
  ret void
}

declare void @write(i32, ptr, i32)

declare i32 @strlen(ptr)

define void @"gyoji::std::pointers::Owned::~Owned"(ptr %"<this>") {
entry:
  %"<this>1" = alloca ptr, align 8
  store ptr %"<this>", ptr %"<this>1", align 8
  br label %BB0

BB0:                                              ; preds = %entry
  %0 = load ptr, ptr %"<this>1", align 8
  %1 = load %"gyoji::std::pointers::Owned", ptr %0, align 8
  %2 = getelementptr inbounds %"gyoji::std::pointers::Owned", ptr %0, i32 0, i32 0
  %3 = load ptr, ptr %2, align 8
  call void @free(ptr %3)
  call void @do_write(ptr @0)
  ret void
}

declare void @free(ptr)

define %"gyoji::std::pointers::Owned" @"gyoji::std::pointers::Owned::create"(ptr %p) {
entry:
  %p1 = alloca ptr, align 8
  store ptr %p, ptr %p1, align 8
  br label %BB0

BB0:                                              ; preds = %entry
  %pointer = alloca %"gyoji::std::pointers::Owned", align 8
  %0 = load %"gyoji::std::pointers::Owned", ptr %pointer, align 8
  %1 = load ptr, ptr %p1, align 8
  %2 = getelementptr inbounds %"gyoji::std::pointers::Owned", ptr %pointer, i32 0, i32 0
  store ptr %1, ptr %2, align 8
  %3 = load %"gyoji::std::pointers::Owned", ptr %pointer, align 8
  %4 = load %"gyoji::std::pointers::Owned", ptr %pointer, align 8
  call void @"gyoji::std::pointers::Owned::~Owned"(ptr %pointer)
  ret %"gyoji::std::pointers::Owned" %3
}

define %"gyoji::std::pointers::Owned" @"gyoji::std::pointers::Owned::do_create"() {
entry:
  br label %BB0

BB0:                                              ; preds = %entry
  %0 = call ptr @malloc(i64 32)
  %1 = call %"gyoji::std::pointers::Owned" @"gyoji::std::pointers::Owned::create"(ptr %0)
  ret %"gyoji::std::pointers::Owned" %1
}

declare ptr @malloc(i64)

define ptr @"gyoji::std::pointers::Owned::get"(ptr %"<this>") {
entry:
  %"<this>1" = alloca ptr, align 8
  store ptr %"<this>", ptr %"<this>1", align 8
  br label %BB0

BB0:                                              ; preds = %entry
  %0 = load ptr, ptr %"<this>1", align 8
  %1 = load %"gyoji::std::pointers::Owned", ptr %0, align 8
  %2 = getelementptr inbounds %"gyoji::std::pointers::Owned", ptr %0, i32 0, i32 0
  %3 = load ptr, ptr %2, align 8
  ret ptr %3
}

define i32 @main(i32 %argc, ptr %argv) {
entry:
  %argv2 = alloca ptr, align 8
  %argc1 = alloca i32, align 4
  store i32 %argc, ptr %argc1, align 4
  store ptr %argv, ptr %argv2, align 8
  br label %BB0

BB0:                                              ; preds = %entry
  call void @do_write(ptr @1)
  %ptr = alloca %"gyoji::std::pointers::Owned", align 8
  %0 = load %"gyoji::std::pointers::Owned", ptr %ptr, align 8
  %1 = call ptr @malloc(i64 32)
  %2 = getelementptr inbounds %"gyoji::std::pointers::Owned", ptr %ptr, i32 0, i32 0
  store ptr %1, ptr %2, align 8
  %pref = alloca ptr, align 8
  %3 = load ptr, ptr %pref, align 8
  %4 = load %"gyoji::std::pointers::Owned", ptr %ptr, align 8
  store ptr %ptr, ptr %pref, align 8
  %another = alloca ptr, align 8
  %5 = load ptr, ptr %another, align 8
  %6 = load ptr, ptr %pref, align 8
  store ptr %6, ptr %another, align 8
  %7 = load %"gyoji::std::pointers::Owned", ptr %ptr, align 8
  call void @"gyoji::std::pointers::Owned::~Owned"(ptr %ptr)
  ret i32 0
}
