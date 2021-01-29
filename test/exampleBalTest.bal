// RUN: export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64/jre
// RUN: ballerina build --dump-bir-file=%t %s
// RUN: %nballerinacc %t
// RUN: temp=$(echo %t) && filename="${temp%%%%.*}"
// RUN: clang-11 -O0 -o $filename.out $filename.ll
// RUN: echo RETVAL=$($filename.out)

int result = 0;
public function main(){
    result = 1;
}

// CHECK: RETVAL
// CHECK-SAME: 0