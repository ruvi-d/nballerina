// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function bar(any z) returns string
{
    string strl = <string>z;
    return strl;
}

public function main() {
  string b = "Hello Wolrd";
  string c = bar(b);
}

// CHECK: RETVAL=
