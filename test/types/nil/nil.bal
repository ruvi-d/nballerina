// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function main() {
  () nilVal = bar();
}

function foo() {
  () nilVal = ();
}

function bar() returns () {
  return ();
}

// CHECK: RETVAL=0
